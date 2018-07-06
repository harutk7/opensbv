//
// Created by harut on 8/9/17.
//

#ifndef PB_MAIN_CAMERA_H
#define PB_MAIN_CAMERA_H

#define CLEAR(x) memset(&(x), 0, sizeof(x))

#ifndef V4L2_PIX_FMT_H264
#define V4L2_PIX_FMT_H264     v4l2_fourcc('H', '2', '6', '4') /* H264 with start codes */
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>             /* getopt_long() */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <vector>
#include <mutex>

namespace opensbv {
    namespace camera {

        class CameraUVC;

        /// UVC capture method struct
        enum io_method {
            IO_METHOD_READ, ///< READ
            IO_METHOD_MMAP, ///< MMAP, We use this part
            IO_METHOD_USERPTR, /// USERPTR
        };

        /// UVC capture format
        enum capture_format {
            CAP_MJPG,
            CAP_YUYV,
        };

        /// UVC capture method struct
        struct buffer {
            void   *start; ///< Start of buffer
            size_t  length; ///< Length of buffer
        };

        /// Handle Filaure thread struct
        struct failureHandlestruct {
            CameraUVC *m_inst; /// pointer to CameraUVC
        };

/// UVC Camera class
/**
  Camera class for capturing frame
*/
        class CameraUVC {

        protected:

            /// Open camera
            /**
               Open Camera function for inherited class
             */
            bool StartCapture();

            /// Close camera
            /**
               Close Camera function for inherited class
             */
            bool StopCapture();


            /// Process Image called on frame ready
            /**
               This function is called when frame is ready
             */
            virtual void OnFrameReady(const void *p, int size); // process image

            /// Set Capture method
            /**
             * Capture MJPEG OR YUYV
             * @param cap_fromat
             */
            void SetCaptureFormat(enum capture_format cap_fromat);

            /// Get running status
            bool IsRunning();
        public:
            /**
             * Camera UVC constructor
             * @param device
             * @param height
             * @param width
             */
            CameraUVC(char device[20], unsigned short height, unsigned short width);
            ~CameraUVC(); ///< Destructor


            static void *handleFailures(void *); ///< handle error failure thread


        private:
            char *m_dev_name; ///< Device to open
            bool m_running = false; ///< Capture running or not
            bool m_hasError = false; ///< Capture has error or not
            buffer  *m_buffers; ///< Capture frame buffer
            enum io_method  m_io; ///< Input out method
            enum capture_format m_cap_format; ///< Capture format
            int m_fd; ///< Device fd
            int m_cfd; ///< Device fd
            unsigned int m_n_buffers; ///< Buffer for image
            unsigned short m_width; ///< Frame width
            unsigned short m_height; ///< Frame height
            std::mutex m_mtx; ///< mutex for reading frame and stoping it
            pthread_mutex_t m_main_mutex = PTHREAD_MUTEX_INITIALIZER; ///< Main Mutex
            int m_numErrors; ///< number of errors

            void errno_exit(const char *s); ///< Error handling
            int xioctl(int fh, int request, void *arg); ///< Apply camera params
            int read_frame(void); ///< Read frames
            bool mainloop(void); ///< Main loop
            bool stop_capturing(void); ///< Stop capture
            bool start_capturing(void); ///< Start capture
            bool uninit_device(void); ///< Uninit camera
            bool init_read(unsigned int buffer_size); ///< Init read
            bool init_mmap(void); ///< Init mmap
            bool init_userp(unsigned int buffer_size); ///< Init userp
            bool init_device(void); ///< Init device
            bool close_device(); ///< Close camera
            bool open_device(void); ///< Open camera

            bool prepare_capture(); ///< Prepare capture
            void switch_device(); ///< Switch camera device

            unsigned short m_dev_num = 0; /// current device num
            std::vector<std::string> m_dev_list {
                    "/dev/video0",
                    "/dev/video1",
                    "/dev/video2",
                    "/dev/video3",
                    "/dev/video4",
                    "/dev/video5",
                    "/dev/video6",
                    "/dev/video7",
                    "/dev/video8",
                    "/dev/video9",
                    "/dev/video10",
                    "/dev/video11",
                    "/dev/video12",
            }; /// for switching between devices
        };

    }
}

#endif //PB_MAIN_CAMERA_H
