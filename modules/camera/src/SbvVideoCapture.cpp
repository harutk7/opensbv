//
// Created by harut on 8/4/17.
//

#include "opensbv/camera/SbvVideoCapture.h"

namespace opensbv {
    namespace camera {

        SbvVideoCapture::SbvVideoCapture(string camera, unsigned short width, unsigned short height,
                         opensbv::camera::capture_format format) : CameraUVC((char *)camera.c_str(), width, height), mWidth(width), mHeight(height), m_cap_format(format) {

            try {
                // set capture method on CameraUVC
                SetCaptureFormat(m_cap_format);

                this->m_frame = cv::Mat(mHeight, mWidth, CV_8UC3);

                this->m_frame_yuyv = cv::Mat(mHeight, mWidth, CV_8UC2);

                mRead = false;
            } catch (std::exception& e) {
                throw SbvVideoCaptureException("SbvVideoCapture()", e.what());
            }
        }

        SbvVideoCapture::~SbvVideoCapture() {
            try {
                StopCapture();
            } catch (std::exception& e) {
                throw SbvVideoCaptureException("~SbvVideoCapture()", e.what());
            }
        }

        void SbvVideoCapture::open() {
            try {
                mWorkerThread = boost::thread(&SbvVideoCapture::runWorker, this);
            } catch (std::exception& e) {
                throw SbvVideoCaptureException("open()", e.what());
            }
        }

        void SbvVideoCapture::close() {
            try {
                mWorkerThread.interrupt();
                mWorkerThread.join();
            } catch (std::exception& e) {
                throw SbvVideoCaptureException("close()", e.what());
            }
        }

        void SbvVideoCapture::runWorker(opensbv::camera::SbvVideoCapture *cap) {
            try {
                cap->StartCapture(); // open camera from Camera class with main loop
            } catch(CameraUVCException &e) {
                throw SbvVideoCaptureException("runWorker()", e.what());
            }
        }

        bool SbvVideoCapture::isOpened() {
            try {
                return IsOpened();
            } catch (std::exception& e) {
                throw SbvVideoCaptureException("isOpened()", e.what());
            }
        }

        cv::Mat SbvVideoCapture::readMat() {
            try {
                mTx.lock();
                cv::Mat frame;
                if (mRead) {
                    m_frame.copyTo(frame);
                    mRead = false;
                }
                mTx.unlock();
                return frame;
            } catch (cv::Exception& e) {
                throw SbvVideoCaptureException("readMat()", e.what());
            }
        }

        void SbvVideoCapture::OnFrameReady(const void *p, unsigned long size) {
            try {
                mTx.lock();
                if (counter == 0){
                    time(&start);
                }
                int start_s=clock();

                if (m_cap_format == CAP_YUYV) {
                    memcpy( m_frame_yuyv.data, (char *)p, sizeof(unsigned char) * (mWidth * 2 * mHeight) );
                    cvtColor(m_frame_yuyv, this->m_frame, CV_YUV2BGR_YUYV);

                }
                else if (m_cap_format == CAP_MJPG) {

                    // decode image
                    try {
                        ImageHelper::decompress_jpg_turbo((unsigned char *)p, size, IMAGE_COLOR_BGR, &m_rgbimage,mWidth, mHeight);
                    } catch(ImageHelperException &e) {
                        mTx.unlock();
                        return;
                    }

                    // apply to Mat
                    this->m_frame.data = m_rgbimage.buffer;
                }

                // fps counter begin
                time(&end);
                counter++;
                sec = difftime(end, start);
                fps = counter/sec;
                if (counter > 30) {
                    char buffer[30];
                    mFps = fps;
                }

                if (counter == (INT_MAX - 1000))
                    counter = 0;

                mRead = true;
                mTx.unlock();

            } catch(Exception &e) {
                mTx.unlock();
                throw SbvVideoCaptureException("OnFrameReady()", e.what());
            } catch(std::exception &e) {
                mTx.unlock();
                throw SbvVideoCaptureException("OnFrameReady()", e.what());
            }
        }
    }
}
