//
// Created by harut on 7/5/18.
//

#include "opensbv/streamer/capture.h"

#include <opencv2/opencv.hpp>

using namespace opensbv::streamer;

int main(int argc, char* argv[]) {

    Capture cap(CAPTURE_MRTP, 12345);
    cap.setHost("127.0.0.1");
    cap.setPort(8554);
    if(!cap.connect())
        return 0;
    cap.run();

    while(cap.isOpened()) {
        try {
            cv::Mat frame = cap.readMat();
            if (frame.empty())
                continue;

            cv::imshow("frame", frame);
            cv::waitKey(1);

        } catch(StreamerException &e) {

        }
    }

    return 0;
}