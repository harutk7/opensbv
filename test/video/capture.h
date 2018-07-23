//
// Created by harut on 8/4/17.
//

#ifndef PB_MAIN_TRACKER_H
#define PB_MAIN_TRACKER_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include "opensbv/camera/CameraUVC.h"
#include "opensbv/helpers/image/ImageHelper.h"
#include "opensbv/helpers/general/GeneralHelper.h"
#include "opensbv/streamer/streamer.h"
#include "opensbv/streamer/streamerMRTP.h"

using namespace std;
using namespace cv;
using namespace opensbv;
using namespace opensbv::helpers::image;
using namespace opensbv::streamer;
using namespace opensbv::camera;

#define FRAME_WIDTH (1920)
#define FRAME_HEIGHT (1080)
#define TRACK_WAIT_INTERVAL (5000)

/// Tracking Object class
/**
 * Capture Object class for t
 * racking object
 */
class Capture: public CameraUVC {
public:
    Capture(string camera); ///< Constructor
    ~Capture(); ///< Destructor

    /// Run Tracking and Streaming
    void Run();

private:
    cv::Mat m_frame; ///< Frames from VideoCapture and CopyTo
    cv::Mat m_frame_yuyv; ///< YUYV Mat object
    buffer_image m_rgbimage; ///< Decompress image struct
    bool m_display = true; ///< Display window or not
    enum capture_format m_cap_format = CAP_MJPG; ///< Capture format
    unsigned char *m_grayData; ///< Graysacle data

    StreamerMRTP *mMRtpStreamer = nullptr;
    Streamer mStreamer;

    // fps counter begin
    time_t start, end;
    int counter = 0;
    double sec;
    double fps;
    // fps counter end

    cv::FileStorage fs;

    cv::Mat undistort_img, camera_matrix, distortion;
    bool first = true;

    cv::Size new_size, mSize;

    cv::Mat map1, map2;


    /// Process Image function
    /**
     * This function is virtual overloaded from Camera class and is called when frame is ready
     * @param p
     * @param size
     */
    void OnFrameReady(const void *p, unsigned long size);
};


#endif //PB_MAIN_TRACKER_H