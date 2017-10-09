//
// Created by harut on 8/4/17.
//

#include "Tracker.h"

// Constructor and call super Constructor
Tracker::Tracker(string camera) : CameraUVC((char *)camera.c_str(), FRAME_WIDTH, FRAME_HEIGHT)
        , m_mutexTracker(PTHREAD_MUTEX_INITIALIZER)
        , m_mutexTrackerNewTarget(PTHREAD_MUTEX_INITIALIZER) {

    // set capture method on CameraUVC
    SetCaptureFormat(m_cap_format);

    this->m_trackObject = false; // do not track object on first
    m_parameters = cv::TrackerKCF::Params{};

    m_parameters.detect_thresh = 0;
    m_parameters.compressed_size = 1;
    m_parameters.pca_learning_rate = 0;
    m_parameters.resize = true;
    m_parameters.split_coeff = true;
    m_parameters.wrap_kernel = true;
    m_parameters.compress_feature = true;

    m_parameters.desc_pca = cv::TrackerKCF::MODE::GRAY;
    m_parameters.desc_npca = 0;

    this->m_kcfTracker = cv::TrackerKCF::create(m_parameters);
    this->m_kcfTracker->init(this->m_frame_c, this->m_selection); // init new tracker

    // init Mat object
    this->m_frame = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC1);
    this->m_frame_yuyv = cv::Mat(FRAME_HEIGHT, FRAME_WIDTH, CV_8UC2);

    // init tracker params for thread
    m_trackerParams.mutexTracker = &m_mutexTracker;
    m_trackerParams.mutexTrackerNewTarget = &m_mutexTrackerNewTarget;
    m_trackerParams.tracker = (Tracker *)this;

    m_grayData = (unsigned char*)malloc(FRAME_HEIGHT * FRAME_WIDTH);
}

// Destructor
Tracker::~Tracker() {
    if (m_grayData != nullptr)
        free(m_grayData);
}

// Set Config instance
void Tracker::SetConfig(Config *config) {
    this->m_config = config;
}

// Set Streamer instance
void Tracker::SetStreamer(Streamer *streamer) {
    this->m_streamer = streamer;
}

// Set Serial instance
void Tracker::SetSerial(SerialRW *serial) {
    this->m_serial = serial;
}

// run tracking
void Tracker::Run() {
    try {
        // run tracker working thread
        pthread_create(&this->m_trackerWorkerT,NULL,&Tracker::trackerWorker,&m_trackerParams);

        // set cpu affinities for this threads
        CPU_ZERO(&this->m_cpuSetTrackerWorker);
        CPU_SET(6, &this->m_cpuSetTrackerWorker);
        pthread_setaffinity_np(this->m_trackerWorkerT, sizeof(cpu_set_t), &this->m_cpuSetTrackerWorker);

        if (this->m_display) {
            cv::namedWindow("video", CV_WINDOW_AUTOSIZE); // nameWindow
            cv::setMouseCallback( "video", this->onMouse, (Tracker *)this ); // mouse callback
        }

        // capture with opencv or with Camera class
        if (!this->m_capCv) {
            this->StartCapture(); // open camera from Camera class with main loop
        }
        else {
            // capture settings
            this->m_cap.open(this->m_config->GetCamera(), CV_CAP_V4L2); // open the camera

            this->m_cap.set(CV_CAP_PROP_FRAME_WIDTH,FRAME_WIDTH); // set width
            this->m_cap.set(CV_CAP_PROP_FRAME_HEIGHT,FRAME_HEIGHT); // set height
            this->m_cap.set(CV_CAP_PROP_FPS, VIDEO_FPS); // set fps
            this->m_cap.set(CV_CAP_PROP_FOURCC ,CV_FOURCC('M', 'J', 'P', 'G') ); // set capture format

            if (!this->m_cap.isOpened()) // if not opened close
            {
                printf("no cam found :(\n");
                exit(0);
            }
            this->OnCvFrameReady(); // open camera with opencv videocapture
        }
    } catch(...) {
        std::cerr << "error on running tracker" << std::endl;
    }
}

// Disable Tracker
void Tracker::DisableTracker() {
    pthread_mutex_lock(&m_mutexTrackerNewTarget); // lock

    this->m_trackObject = false; // disable tracking

    pthread_mutex_unlock(&m_mutexTrackerNewTarget); // unlock
}

// Enable Tracker
void Tracker::EnableTracker(int x, int y, int width, int height) {
    try {
        pthread_mutex_lock(&m_mutexTrackerNewTarget); // lock

        this->m_trackObject = false; // disable tracking
        this->m_selection = cv::Rect(x,y,width,height); // new selected area

        // init kcv tracker
        this->m_kcfTracker->clear(); // clear old tracker and create new one

        m_parameters = cv::TrackerKCF::Params{};

        m_parameters.detect_thresh = 0;
        m_parameters.compressed_size = 1;
        m_parameters.pca_learning_rate = 0;
        m_parameters.resize = true;
        m_parameters.split_coeff = true;
        m_parameters.wrap_kernel = true;
        m_parameters.compress_feature = true;

        m_parameters.desc_pca = cv::TrackerKCF::MODE::GRAY;
        m_parameters.desc_npca = 0;

        this->m_kcfTracker = cv::TrackerKCF::create(m_parameters);

        this->m_kcfTracker->init(this->m_frame_c, this->m_selection); // init new tracker

        this->m_trackObject = true; // enable tracking

        pthread_mutex_unlock(&m_mutexTrackerNewTarget); // unlock

    } catch (...) {
        std::cerr << "error in EnableTracker" << std::endl;
    }
}

// On mouse callback
void Tracker::onMouse(int event, int x, int y, int, void* ptr) {

    switch( event )
    {
        case cv:: EVENT_LBUTTONUP:
            ((Tracker *)ptr)->EnableTracker(x,y,30,30);
            break;
    }
}

// Tracker thread worker
void *Tracker::trackerWorker(void *ptr) {
    trackerWorkerStruct params = *(trackerWorkerStruct *)ptr; // params

    // tracking cycle
    while(true) {
        pthread_mutex_lock(params.mutexTracker); // lock
        pthread_mutex_lock(params.mutexTrackerNewTarget); // lock
        params.tracker->TrackObject(); // track the object
        pthread_mutex_unlock(params.mutexTrackerNewTarget); // unlock
        pthread_mutex_unlock(params.mutexTracker); // unlock

        usleep(TRACK_WAIT_INTERVAL*3); // sleep for some ms
    }
}

// Track Object
void Tracker::TrackObject() {
    try {
        if (this->m_trackObject) { // if tracking is enabled
            // track with KCF
            if(this->m_kcfTracker->update(this->m_frame, this->m_bbox)) {

                // draw rectangle
                cv::rectangle(this->m_frame, this->m_bbox, cv::Scalar(255, 0, 0), 2, 1);

                // write to serial
                this->WriteSerial((int)(this->m_bbox.x + (this->m_bbox.width/2)),
                                  (int)(this->m_bbox.y + (this->m_bbox.height/2)),
                                  (int)((this->m_bbox.width*this->m_bbox.height)/(FRAME_HEIGHT*FRAME_WIDTH))); //write to serial
            }
        }
    } catch(...) {
        std::cerr << "error in TrackObject" << std::endl;
    }
}

// Camera Process image override
void Tracker::OnFrameReady(const void *p, int size) {
    try {
//        int start_s=clock();

        if (m_cap_format == CAP_YUYV) {
            memcpy( m_frame_yuyv.data, (char *)p, sizeof(unsigned char) * (FRAME_WIDTH * 2 * FRAME_HEIGHT) );
            cvtColor(m_frame_yuyv, this->m_frame, CV_YUV2BGR_YUYV);
        }
        else if (m_cap_format == CAP_MJPG) {
            m_rgbimage.buffersize = 0;
            m_rgbimage.buffer = NULL;

            // decode image
            if (!ImageHelper::decompress_jpeg((unsigned char *)p, size, &m_rgbimage, IMAGE_COLOR_GRAYSCALE))
                return;

            // apply to Mat
            this->m_frame.data = m_rgbimage.buffer;
        }

        if (this->m_trackObject)
            this->m_frame.copyTo(this->m_frame_c); // copy frame

        pthread_mutex_unlock(&m_mutexTracker); // lock for tracking

        usleep(TRACK_WAIT_INTERVAL); // sleep for 5 ms

        pthread_mutex_lock(&m_mutexTracker); // unlock for tracking

        if (!this->m_frame.empty())
            this->m_streamer->Write(this->m_frame.data, FRAME_HEIGHT * FRAME_WIDTH); // stream write

//        int stop_s=clock();
//        cout << "processing time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << endl;

        if (this->m_display) {
            cv::imshow("video", this->m_frame); // show frame in nameWindow
        }

        if (m_cap_format == CAP_MJPG && m_rgbimage.buffer != nullptr)
            free(m_rgbimage.buffer);        // free the buffer

        bzero(m_grayData, FRAME_WIDTH * FRAME_HEIGHT);
        this->StopCapture();

        cv::waitKey(1);
    } catch(...) {
        std::cerr << "error in StartCapture" << std::endl;
    }
}

// Open camera with OpenCV VideoCapture
void Tracker::OnCvFrameReady() {

    while (true) {
        try {
//            int start_s=clock();

            this->m_cap >> this->m_frame; // read frame

            if (this->m_trackObject)
                this->m_frame.copyTo(this->m_frame_c); // copy frame

            if(this->trackCounter % this->trackCounterTerm) {
                pthread_mutex_unlock(&m_mutexTracker); // lock for tracking

                usleep(TRACK_WAIT_INTERVAL); // sleep for 5 ms

                pthread_mutex_lock(&m_mutexTracker); // unlock for tracking

                this->trackCounter = 0;
            }
            if (!this->m_frame.empty())
                this->m_streamer->Write(this->m_frame.data, this->m_frame.size().height * this->m_frame.size().width * 3); // stream write

            if (this->m_display)
                cv::imshow("video", this->m_frame); // show frame in nameWindow

//            int stop_s=clock();
//            cout << "encode time: " << (stop_s-start_s)/double(CLOCKS_PER_SEC)*1000 << endl;

            cv::waitKey(FRAME_INTERVAL-TRACK_WAIT_INTERVAL);
        } catch (...) {
            std::cerr << "error in OnCvFrameReady" << std::endl;
        }
    }
}

// Write to serial x, y, z
void Tracker::WriteSerial(int x, int y, int z) {
    string data;
    data += "X" + GeneralHelper::format_account_number(x) + "$"; // add x
    data += "Y" + GeneralHelper::format_account_number(y) + "*"; // add y
    data += "Z" + GeneralHelper::format_account_number(z) + "#"; // add z

    // write serial
    this->m_serial->Write(data);
}