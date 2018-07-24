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
    cap.connect();
    cap.run();

    while(true) {
        std::vector<unsigned char> data = cap.getData();
        if (data.empty())
            continue;

        cv::Mat frame = cv::imdecode(cv::Mat(data), 1);

        if (frame.empty())
            continue;

        cv::imshow("frame", frame);
        cv::waitKey(1);
    }
}