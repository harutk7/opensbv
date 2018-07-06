//
// Created by harut on 8/9/17.
//

#include "opensbv/camera/CameraUVC.h"

namespace opensbv {
    namespace camera {

        CameraUVC::CameraUVC(char device[20], unsigned short width, unsigned short height) {
            m_io = IO_METHOD_MMAP; // set read method
            m_fd = -1; // set fd value
            this->m_dev_name = (char *)malloc(20);
            memcpy(this->m_dev_name, device, 20);
            this->m_cap_format = CAP_MJPG;
            this->m_width = width;
            this->m_height = height;

        }

        CameraUVC::~CameraUVC() {
            if (this->m_dev_name != nullptr)
                free(this->m_dev_name);
        }

        bool CameraUVC::StartCapture() {

            prepare_capture();

            // open devicef
            while(this->m_hasError) {

                std::cout << "restarting video capture" << std::endl;

                StopCapture();

                prepare_capture();
            }

            return true;
        }

        bool CameraUVC::StopCapture() {
            try {
                if (this->m_running) {
                    m_running = false;

                    pthread_mutex_lock(&this->m_main_mutex);
                    stop_capturing();
                    uninit_device();
                    close_device();

                    std::cout << "video capture stopped" << std::endl;

                    pthread_mutex_unlock(&this->m_main_mutex);
                }
            } catch (...) {
                std::cerr << "error in CameraUVC::StopCapture()" << std::endl;
                pthread_mutex_unlock(&this->m_main_mutex);
                return false;
            }
            return true;
        }

        bool CameraUVC::prepare_capture() {
            pthread_mutex_unlock(&this->m_main_mutex);

            if (!open_device()) {
                close_device();
                pthread_mutex_unlock(&this->m_main_mutex);
                return false;
            }

            if (!init_device()){
                uninit_device();
                close_device();
                pthread_mutex_unlock(&this->m_main_mutex);
                return false;
            }

            if (!start_capturing()) {
                stop_capturing();
                uninit_device();
                close_device();
                pthread_mutex_unlock(&this->m_main_mutex);
                return false;
            }

            std::cout << "video capture started" << std::endl;

            m_running = true;
            m_hasError = false;

            pthread_mutex_unlock(&this->m_main_mutex);

            return mainloop();
        }

        void CameraUVC::switch_device() {
            if (m_dev_num == m_dev_list.size() - 1)
                m_dev_num = 0;

            memcpy(this->m_dev_name, (char *)m_dev_list[m_dev_num].c_str(), m_dev_list[m_dev_num].length());

            m_dev_num++;
        }

        void CameraUVC::errno_exit(const char *s) {
            m_numErrors++;

            if (m_numErrors > 5) {
                this->m_hasError = true;
            }
            fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
//    exit(EXIT_FAILURE);
        }

        int CameraUVC::xioctl(int fh, int request, void *arg) {
            int r;

            do {
                r = ioctl(fh, request, arg);
            } while (-1 == r && EINTR == errno);

            return r;
        }

        void CameraUVC::OnFrameReady(const void *p, int size) {};

        int CameraUVC::read_frame(void) {
            struct v4l2_buffer buf;
            unsigned int i;

            switch (this->m_io) {
                case IO_METHOD_READ:
                    if (-1 == read(this->m_fd, this->m_buffers[0].start, this->m_buffers[0].length)) {
                        switch (errno) {
                            case EAGAIN:
                                return 0;

                            case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                            default:
                                errno_exit("read");
                                return 0;
                        }
                    }

                    OnFrameReady(this->m_buffers[0].start, this->m_buffers[0].length);
                    break;

                case IO_METHOD_MMAP:
                    CLEAR(buf);

                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_MMAP;
                    if (-1 == xioctl(this->m_fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                            case EAGAIN:
                                return 0;

                            case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                            default:
                                errno_exit("VIDIOC_DQBUF");
                                return 0;
                        }
                    }

                    assert(buf.index < this->m_n_buffers);

                    if (buf.bytesused > 307200 && m_cap_format == CAP_MJPG) {
                        return 0;
                    }

                    if (buf.bytesused != 0)
                        OnFrameReady(this->m_buffers[buf.index].start, buf.bytesused);
                    else
                        std::cerr << "CameraUVC: frame is empty" << std::endl;

                    if (-1 == xioctl(this->m_fd, VIDIOC_QBUF, &buf)){
                        errno_exit("VIDIOC_QBUF");
                        return 0;
                    }
                    break;

                case IO_METHOD_USERPTR:
                    CLEAR(buf);

                    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    buf.memory = V4L2_MEMORY_USERPTR;

                    if (-1 == xioctl(this->m_fd, VIDIOC_DQBUF, &buf)) {
                        switch (errno) {
                            case EAGAIN:
                                return 0;

                            case EIO:
                                /* Could ignore EIO, see spec. */

                                /* fall through */

                            default:
                                errno_exit("VIDIOC_DQBUF");
                                return 0;
                        }
                    }

                    for (i = 0; i < this->m_n_buffers; ++i)
                        if (buf.m.userptr == (unsigned long)this->m_buffers[i].start
                            && buf.length == this->m_buffers[i].length)
                            break;

                    assert(i < this->m_n_buffers);

                    OnFrameReady((void *)buf.m.userptr, buf.bytesused);

                    if (-1 == xioctl(this->m_fd, VIDIOC_QBUF, &buf)) {
                        errno_exit("VIDIOC_QBUF");
                        return 0;
                    }
                    break;
            }
            return 1;
        }

        bool CameraUVC::mainloop(void) {

            fd_set fds;
            struct timeval tv;
            int r;

            for (;;) {
                pthread_mutex_lock(&this->m_main_mutex);
                if (!this->m_running) {
                    pthread_mutex_unlock(&this->m_main_mutex);
                    return false;
                }
                if (this->m_hasError) {
                    pthread_mutex_unlock(&this->m_main_mutex);
                    return false;
                }

                FD_ZERO(&fds);
                FD_SET(this->m_fd, &fds);

                /* Timeout. */
                tv.tv_sec = 2;
                tv.tv_usec = 0;

                r = select(this->m_fd + 1, &fds, NULL, NULL, &tv);

                if (-1 == r) {
                    if (EINTR == errno) {
                        pthread_mutex_unlock(&this->m_main_mutex);
                        continue;
                    }
                    errno_exit("select");
                }

                if (0 == r) {
                    fprintf(stderr, "select timeout\n");
                }

                if (read_frame() == 0) {
                }
                pthread_mutex_unlock(&this->m_main_mutex);

                /* EAGAIN - continue select loop. */
            }

            return true;
        }

        bool CameraUVC::stop_capturing(void) {
            enum v4l2_buf_type type;

            switch (this->m_io) {
                case IO_METHOD_READ:
                    /* Nothing to do. */
                    break;

                case IO_METHOD_MMAP:
                case IO_METHOD_USERPTR:
                    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    if (-1 == xioctl(this->m_fd, VIDIOC_STREAMOFF, &type)) {
                        errno_exit("VIDIOC_STREAMOFF");
                        return false;
                    }
                    break;
            }

            return true;
        }

        bool CameraUVC::start_capturing(void) {
            unsigned int i;
            enum v4l2_buf_type type;

            switch (this->m_io) {
                case IO_METHOD_READ:
                    /* Nothing to do. */
                    break;

                case IO_METHOD_MMAP:
                    for (i = 0; i < this->m_n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_MMAP;
                        buf.index = i;

                        if (-1 == xioctl(this->m_fd, VIDIOC_QBUF, &buf)) {
                            errno_exit("VIDIOC_QBUF");
                            return false;
                        }
                    }
                    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    if (-1 == xioctl(this->m_fd, VIDIOC_STREAMON, &type)) {
                        errno_exit("VIDIOC_STREAMON");
                        return false;
                    }
                    break;

                case IO_METHOD_USERPTR:
                    for (i = 0; i < this->m_n_buffers; ++i) {
                        struct v4l2_buffer buf;

                        CLEAR(buf);
                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        buf.memory = V4L2_MEMORY_USERPTR;
                        buf.index = i;
                        buf.m.userptr = (unsigned long)this->m_buffers[i].start;
                        buf.length = this->m_buffers[i].length;

                        if (-1 == xioctl(this->m_fd, VIDIOC_QBUF, &buf)) {
                            errno_exit("VIDIOC_QBUF");
                            return false;
                        }
                    }
                    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                    if (-1 == xioctl(this->m_fd, VIDIOC_STREAMON, &type)) {
                        errno_exit("VIDIOC_STREAMON");
                        return false;
                    }
                    break;
            }

            return true;
        }

        bool CameraUVC::uninit_device(void) {
            unsigned int i;
            switch (this->m_io) {
                case IO_METHOD_READ:
                    free(this->m_buffers[0].start);
                    break;

                case IO_METHOD_MMAP:
                    try {
                        for (i = 0; i < this->m_n_buffers; ++i)
                            if (this->m_buffers != NULL) {
                                if (-1 == munmap(this->m_buffers[i].start, this->m_buffers[i].length)) {
                                    errno_exit("munmap");
                                    return false;
                                }
                            }
                        break;
                    } catch(...){
                    }

                case IO_METHOD_USERPTR:
                    for (i = 0; i < this->m_n_buffers; ++i)
                        free(this->m_buffers[i].start);
                    break;
            }

            if (this->m_buffers != nullptr)
                free(this->m_buffers);

            this->m_buffers = NULL;

            return true;
        }

        bool CameraUVC::init_read(unsigned int buffer_size) {
            this->m_buffers = (buffer *)calloc(1, sizeof(*this->m_buffers));

            if (!this->m_buffers) {
                fprintf(stderr, "Out of memory\n");
                return false;
            }
            this->m_buffers[0].length = buffer_size;
            this->m_buffers[0].start = malloc(buffer_size);

            if (!this->m_buffers[0].start) {
                fprintf(stderr, "Out of memory\n");
                return false;
            }

            return true;
        }

        bool CameraUVC::init_mmap(void) {
            struct v4l2_requestbuffers req;

            CLEAR(req);

            req.count = 4;
            req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            req.memory = V4L2_MEMORY_MMAP;

            if (-1 == xioctl(this->m_fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                    fprintf(stderr, "%s does not support "
                                    "memory mapping\n", this->m_dev_name);
                    return false;
                } else {
                    errno_exit("VIDIOC_REQBUFS");
                    return false;
                }
            }

            if (req.count < 2) {
                fprintf(stderr, "Insufficient buffer memory on %s\n",
                        this->m_dev_name);
                return false;
            }

            this->m_buffers = (buffer *)calloc(req.count, sizeof(*this->m_buffers));

            if (!this->m_buffers) {
                fprintf(stderr, "Out of memory\n");
                return false;
            }

            for (this->m_n_buffers = 0; this->m_n_buffers < req.count; ++this->m_n_buffers) {
                struct v4l2_buffer buf;

                CLEAR(buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = this->m_n_buffers;

                if (-1 == xioctl(this->m_fd, VIDIOC_QUERYBUF, &buf)) {
                    errno_exit("VIDIOC_QUERYBUF");
                    return false;
                }

                this->m_buffers[this->m_n_buffers].length = buf.length;
                this->m_buffers[this->m_n_buffers].start =
                        mmap(NULL /* start anywhere */,
                             buf.length,
                             PROT_READ | PROT_WRITE /* required */,
                             MAP_SHARED /* recommended */,
                             this->m_fd, buf.m.offset);

                if (MAP_FAILED == this->m_buffers[this->m_n_buffers].start) {
                    errno_exit("mmap");
                    return false;
                }
            }

            return true;
        }

        bool CameraUVC::init_userp(unsigned int buffer_size) {
            struct v4l2_requestbuffers req;

            CLEAR(req);

            req.count  = 4;
            req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            req.memory = V4L2_MEMORY_USERPTR;

            if (-1 == xioctl(this->m_fd, VIDIOC_REQBUFS, &req)) {
                if (EINVAL == errno) {
                    fprintf(stderr, "%s does not support "
                                    "user pointer i/o\n", this->m_dev_name);
                    return false;
                } else {
                    errno_exit("VIDIOC_REQBUFS");
                    return false;
                }
            }

            this->m_buffers = (buffer *)calloc(4, sizeof(*this->m_buffers));

            if (!this->m_buffers) {
                fprintf(stderr, "Out of memory\n");
                return false;
            }

            for (this->m_n_buffers = 0; this->m_n_buffers < 4; ++this->m_n_buffers) {
                this->m_buffers[this->m_n_buffers].length = buffer_size;
                this->m_buffers[this->m_n_buffers].start = malloc(buffer_size);

                if (!this->m_buffers[this->m_n_buffers].start) {
                    fprintf(stderr, "Out of memory\n");
                    return false;
                }
            }

            return true;
        }

        bool CameraUVC::init_device(void) {
            struct v4l2_capability cap;
            struct v4l2_cropcap cropcap;
            struct v4l2_crop crop;
            struct v4l2_format fmt;
            unsigned int min;

            if (-1 == xioctl(this->m_fd, VIDIOC_QUERYCAP, &cap)) {
                if (EINVAL == errno) {
                    fprintf(stderr, "%s is no V4L2 device\n",
                            this->m_dev_name);
                    return false;
                } else {
                    errno_exit("VIDIOC_QUERYCAP");
                    return false;
                }
            }

            if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                fprintf(stderr, "%s is no video capture device\n",
                        this->m_dev_name);
                return false;
            }

            switch (this->m_io) {
                case IO_METHOD_READ:
                    if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
                        fprintf(stderr, "%s does not support read i/o\n",
                                this->m_dev_name);
                        return false;
                    }
                    break;

                case IO_METHOD_MMAP:
                case IO_METHOD_USERPTR:
                    if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
                        fprintf(stderr, "%s does not support streaming i/o\n",
                                this->m_dev_name);
                        return false;
                    }
                    break;
            }


            /* Select video input, video standard and tune here. */


            CLEAR(cropcap);

            cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

            if (0 == xioctl(this->m_fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */

                if (-1 == xioctl(this->m_fd, VIDIOC_S_CROP, &crop)) {
                    switch (errno) {
                        case EINVAL:
                            /* Cropping not supported. */
                            break;
                        default:
                            /* Errors ignored. */
                            break;
                    }
                }
            } else {
                /* Errors ignored. */
            }


            CLEAR(fmt);

            fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            if (false) {
                fprintf(stderr, "Set H264\r\n");
                fmt.fmt.pix.width       = 640; //replace
                fmt.fmt.pix.height      = 480; //replace
                fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_H264; //replace
                fmt.fmt.pix.field       = V4L2_FIELD_ANY;

                if (-1 == xioctl(this->m_fd, VIDIOC_S_FMT, &fmt)) {
                    errno_exit("VIDIOC_S_FMT");
                    return false;
                }

                /* Note VIDIOC_S_FMT may change width and height. */
            } else {
                /* Preserve original settings as set by v4l2-ctl for example */
                if (-1 == xioctl(this->m_fd, VIDIOC_G_FMT, &fmt)) {
                    errno_exit("VIDIOC_G_FMT");
                    return false;
                }
            }

            fmt.fmt.pix.width       = this->m_width; //replace
            fmt.fmt.pix.height      = this->m_height; //replace
            if (m_cap_format == CAP_MJPG)
                fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
            else if (m_cap_format == CAP_YUYV)
                fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
            fmt.fmt.pix.field       = V4L2_FIELD_ANY;

            if (-1 == xioctl(this->m_fd, VIDIOC_S_FMT, &fmt)) {
                errno_exit("VIDIOC_S_FMT");
                return false;
            }

            /* Buggy driver paranoia. */
            min = fmt.fmt.pix.width * 2;
            if (fmt.fmt.pix.bytesperline < min)
                fmt.fmt.pix.bytesperline = min;
            min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
            if (fmt.fmt.pix.sizeimage < min)
                fmt.fmt.pix.sizeimage = min;

            switch (this->m_io) {
                case IO_METHOD_READ:
                    init_read(fmt.fmt.pix.sizeimage);
                    break;

                case IO_METHOD_MMAP:
                    init_mmap();
                    break;

                case IO_METHOD_USERPTR:
                    init_userp(fmt.fmt.pix.sizeimage);
                    break;
            }

            return true;
        }

        bool CameraUVC::close_device() {
            if (-1 == close(this->m_fd)) {
                errno_exit("close");
                return false;
            }

            this->m_fd = -1;

            return true;
        }

        bool CameraUVC::open_device(void) {
            struct stat st;

            if (-1 == stat(this->m_dev_name, &st)) {
                fprintf(stderr, "Cannot identify '%s': %d, %s\n",
                        this->m_dev_name, errno, strerror(errno));
                return false;
            }

            if (!S_ISCHR(st.st_mode)) {
                fprintf(stderr, "%s is no device\n", this->m_dev_name);
                return false;
            }

            this->m_fd = open(this->m_dev_name, O_RDWR /* required */ | O_NONBLOCK, 0);

            if (-1 == this->m_fd) {
                fprintf(stderr, "Cannot open '%s': %d, %s\n",
                        this->m_dev_name, errno, strerror(errno));
                return false;
            }

            return true;
        }

        void CameraUVC::SetCaptureFormat(enum capture_format cap_fromat) {
            this->m_cap_format = cap_fromat;
        }

        bool CameraUVC::IsRunning() {
            return m_running;
        }

        void *CameraUVC::handleFailures(void *ptr) {
            auto *params = (failureHandlestruct *)ptr;

            params->m_inst->StopCapture();

            params->m_inst->StartCapture();
        }
    }
}