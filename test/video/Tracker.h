//
// Created by harut on 8/4/17.
//

#ifndef PB_MAIN_TRACKER_H
#define PB_MAIN_TRACKER_H

#include <iostream>
#include "Config.h"
#include "opensbv/streamer/Streamer.h"
#include "Opencv.h"
#include "opensbv/camera/CameraUVC.h"
#include "opensbv/serial/Serialrw.h"
#include "opensbv/helpers/images/ImageHelper.h"
#include "opensbv/helpers/general/GeneralHelper.h"

class Tracker;

/// Tracker worker function for Thread
struct trackerWorkerStruct {
    Tracker *tracker; ///< Tracker class instance
    pthread_mutex_t *mutexTracker; ///< Mutex for control tracking
    pthread_mutex_t *mutexTrackerNewTarget; ///< Mutex for control tracking
};

/// Tracking Object class
/**
 * Tracker Object class for tracking object
 */
class Tracker: public CameraUVC {
public:
    Tracker(string camera); ///< Constructor
    ~Tracker(); ///< Destructor

    /// Set Config instance
    void SetConfig(Config *config);

    /// set Streamer instance
    void SetStreamer(Streamer *streamer);

    /// Set Serial instance
    void SetSerial(SerialRW *serial);

    /// Disable tracking
    void DisableTracker();

    /// Enable tracking
    /**
     * This function is for enabling tracking by providing reactangle for tracking
     * @param x
     * @param y
     * @param width
     * @param height
     */
    void EnableTracker(int x, int y, int width, int height);

    /// Track the object
    /**
     * This function is for private use, it is called everytime the frame is received from thread
     */
    void TrackObject();

    /// This function is for development test
    /**
     * To make it work you should set m_display to true
     * @param event
     * @param x
     * @param y
     * @param ptr
     */
    static void onMouse( int event, int x, int y, int, void* ptr);

    /// Trackering object main Worker for Thread
    static void *trackerWorker(void* ptr);

    /// Process Image function
    /**
     * This function is virtual overloaded from Camera class and is called when frame is ready
     * @param p
     * @param size
     */
    void OnFrameReady(const void *p, int size);

    /// Run Tracking and Streaming
    void Run();

private:
    Config *m_config; ///< Config class
    Streamer *m_streamer; ///< Streamer class
    SerialRW *m_serial; ///< Serial class
    cv::VideoCapture m_cap; ///< VideoCapture instance
    cv::Mat m_frame, m_frame_c; ///< Frames from VideoCapture and CopyTo
    cv::Mat m_frame_yuyv; ///< YUYV Mat object
    bool m_trackObject; ///< Track object or not
    cv::Rect2d m_bbox; ///< Define an initial bounding box
    cv::Rect m_selection; ///< Selected place where to track
    cv::Ptr<cv::Tracker> m_kcfTracker; ///< Tracker kcf object
    cv::Ptr<cv::Tracker> m_medianFlowTracker; ///< Tracker median flow object
    cv::Ptr<cv::Tracker> m_boostingTracker; ///< Tracker boosting object
    pthread_mutex_t m_mutexTracker; ///< Mutex for control tracking
    pthread_mutex_t m_mutexTrackerNewTarget; ///< Mutex for control tracking
    pthread_t m_trackerWorkerT; ///< Tracker worker thread
    cpu_set_t m_cpuSetTrackerWorker; ///< Cpu set for affinity set
    trackerWorkerStruct m_trackerParams; ///< Tracker worker thread params;
    buffer_image m_rgbimage; ///< Decompress image struct
    cv::TrackerKCF::Params m_parameters; ///< KCF parameters
    bool m_display = true; ///< Display window or not
    bool m_capCv = false; ///< Capture with opencv or not
    int trackCounter = 0; ///< Counter which frames to track
    int trackCounterTerm = 2; ///< Which frames to detect
    enum capture_format m_cap_format = CAP_MJPG; ///< Capture format
    unsigned char *m_grayData; ///< Graysacle data

    /// Open Camera with OpenCV
    /**
     * To enable it you should set m_capCV to true
     */
    void OnCvFrameReady();

    /// Write data to serial
    /**
     * This function gets data, creates string and callsed SerialRW Write function
     * @param x
     * @param y
     * @param z
     */
    void WriteSerial(int x, int y, int z);
};


#endif //PB_MAIN_TRACKER_H