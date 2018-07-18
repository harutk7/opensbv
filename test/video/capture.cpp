//
// Created by harut on 8/4/17.
//

#include "capture.h"

// Constructor and call super Constructor
Capture::Capture(string camera) : CameraUVC((char *)camera.c_str(), FRAME_WIDTH, FRAME_HEIGHT), fs("/data/out_camera_data.xml", cv::FileStorage::READ)
{

    // set capture method on CameraUVC
    SetCaptureFormat(m_cap_format);

    this->m_frame = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC3);

    this->m_frame_yuyv = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC2);

    fs["camera_matrix"] >> camera_matrix;
    fs["distortion_coefficients"] >> distortion;

    mMRtpStreamer = new StreamerMRTP(8554);
    mMRtpStreamer->setQuality(90);
    mMRtpStreamer->setWidth(FRAME_WIDTH);
    mMRtpStreamer->setHeight(FRAME_HEIGHT);
    mMRtpStreamer->setColorType(IMAGE_COLOR_BGR);
    mStreamer.setStreamer(mMRtpStreamer);

    mStreamer.run();
}

// Destructor
Capture::~Capture() {
    mStreamer.stop();

    if (mMRtpStreamer != nullptr)
        delete mMRtpStreamer;

    if (m_grayData != nullptr)
        free(m_grayData);
}

// run tracking
void Capture::Run() {
    try {
        this->StartCapture(); // open camera from Camera class with main loop

    } catch(...) {
        std::cerr << "error on running Capture" << std::endl;
    }
}

// Camera Process image override
void Capture::OnFrameReady(const void *p, int size) {
    try {
        if (counter == 0){
            time(&start);
        }
//        int start_s=clock();

        if (m_cap_format == CAP_YUYV) {
            memcpy( m_frame_yuyv.data, (char *)p, sizeof(unsigned char) * (FRAME_WIDTH * 2 * FRAME_HEIGHT) );
            cvtColor(m_frame_yuyv, this->m_frame, CV_YUV2BGR_YUYV);
        }
        else if (m_cap_format == CAP_MJPG) {
            m_rgbimage.buffersize = 0;
            m_rgbimage.buffer = NULL;

            // decode image
            if (!ImageHelper::decompress_jpeg((unsigned char *)p, size, &m_rgbimage, IMAGE_COLOR_BGR))
                return;

            // apply to Mat
            this->m_frame.data = m_rgbimage.buffer;
        }

        if (first)  {
            first = false;

            mSize = new_size.area() != 0 ? new_size : m_frame.size();

            cv::fisheye::initUndistortRectifyMap(camera_matrix, distortion, cv::Matx33d::eye(), camera_matrix, mSize, CV_16SC2, map1, map2 );
        }

        cv::remap(m_frame, undistort_img, map1, map2, cv::INTER_LINEAR, cv::BORDER_CONSTANT);

        mMRtpStreamer->Write(undistort_img.data, undistort_img.total() * undistort_img.elemSize());

        if (this->m_display) {
            cv::imshow("m_frame", this->m_frame); // show frame in nameWindow
            cv::waitKey(1);
        }

        if (m_cap_format == CAP_MJPG && m_rgbimage.buffer != nullptr)
            free(m_rgbimage.buffer);        // free the buffer

        // fps counter begin
        time(&end);
        counter++;
        sec = difftime(end, start);
        fps = counter/sec;
        if (counter > 30) {
            char buffer[30];
            sprintf (buffer, "%.2f", fps);
//            printf("%.2f\n", fps);
//            cout << "\r" <<fps << " fps\n" << flush;
//            fflush(stdout);
            //cv::putText(m_frame, buffer, cvPoint(50,50),
            //      cv::FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200,200,250), 1, CV_AA);
        }
//        printf("%.2f", fps);
        // overflow protection
        if (counter == (INT_MAX - 1000))
            counter = 0;

    } catch(Exception &e) {
        std::cerr << "error in StartCapture" << e.what() << std::endl;
    }
}