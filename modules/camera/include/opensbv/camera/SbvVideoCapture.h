//
// Created by harut on 8/4/17.
//

#ifndef OPENSBV_MODULE_CAMERA_CAPTURE_H
#define OPENSBV_MODULE_CAMERA_CAPTURE_H

#include <iostream>
#include <thread>

#include "opensbv/camera/CameraUVC.h"
#include "opensbv/helpers/image/ImageHelper.h"
#include "opensbv/helpers/general/GeneralHelper.h"
#include <boost/thread.hpp>
#include <mutex>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace opensbv::helpers::image;

namespace opensbv {
    namespace camera {

/// Capture class
/**
 * Capture Object class for t
 * racking object
 */
        class SbvVideoCapture: protected CameraUVC {
        public:
            SbvVideoCapture(string camera, unsigned short width, unsigned short height, capture_format format); ///< Constructor
            ~SbvVideoCapture(); ///< Destructor

            /**
             * is opened
             * @return
             */
            bool isOpened();

            /**
             * open
             */
            void open();

            /**
             * close
             */
            void close();

            /**
             * get cv Mat
             * @return
             */
            cv::Mat readMat();

            static void runWorker(SbvVideoCapture *cap);

        private:

            cv::Mat m_frame; ///< Frames from SbvVideoCapture and CopyTo
            cv::Mat m_frame_yuyv; ///< YUYV Mat object
            buffer_image m_rgbimage; ///< Decompress image struct
            capture_format m_cap_format; ///< Capture format
            unsigned short mWidth, mHeight; ///< Width and height of capture
            double mFps; ///< capture fps
            boost::thread mWorkerThread; ///< worker thread
            std::mutex mTx; ///<mutex
            bool mRead; ///< read data or not

            // fps counter begin
            time_t start, end;
            int counter = 0;
            double sec;
            double fps;
            // fps counter end

            /// Process Image function
            /**
             * This function is virtual overloaded from Camera class and is called when frame is ready
             * @param p
             * @param size
             */
            void OnFrameReady(const void *p, unsigned long size);


        };

    }
}


#endif //OPENSBV_MODULE_CAMERA_CAPTURE_H