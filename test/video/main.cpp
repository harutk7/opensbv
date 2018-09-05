//
// Created by harut on 7/5/18.
//

#include "opensbv/camera/SbvVideoCapture.h"
#include "opensbv/streamer/streamer.h"
#include "opensbv/streamer/streamerMRTP.h"
#include <opencv2/opencv.hpp>

#define FRAME_WIDTH (1280)
#define FRAME_HEIGHT (720)
#define FRAME_QUALITY (90)
#define STREAMER_PORT (8554)
#define WAIT_INTERVAL (100000)

using namespace opensbv::camera;
using namespace opensbv::streamer;

int main(int argc, char* argv[]) {

    if (argc < 2)
        return 1;

    SbvVideoCapture cap(argv[1], FRAME_WIDTH, FRAME_HEIGHT, CAP_YUYV);

    cap.open();

    StreamerMRTP mMRtpStreamer(STREAMER_PORT);
    mMRtpStreamer.setQuality(FRAME_QUALITY);
    mMRtpStreamer.setWidth(FRAME_WIDTH);
    mMRtpStreamer.setHeight(FRAME_HEIGHT);
    mMRtpStreamer.setColorType(IMAGE_COLOR_BGR);
    Streamer mStreamer;
    mStreamer.setStreamer(&mMRtpStreamer);

    mStreamer.run();
//    cv::FileStorage opencv_file("frame_bgr.xml", cv::FileStorage::WRITE);

    while(cap.isOpened()) {
        cv::Mat frame = cap.readMat();

        if (frame.empty() || frame.data == nullptr)
            continue;

//        opencv_file << "my_matrix" << frame;
//        opencv_file.release();

        try {
            mMRtpStreamer.Write(frame.data, frame.total() * frame.elemSize());
        } catch(StreamerMRTPException &e) {
            e.log();
        }

        usleep(WAIT_INTERVAL);
    }

    cap.close();
    mStreamer.stop();

    return 0;
}