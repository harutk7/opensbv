//
// Created by harut on 7/5/18.
//

#include "opensbv/camera/SbvVideoCapture.h"
#include <opencv2/opencv.hpp>

using namespace opensbv::camera;

int main(int argc, char* argv[]) {

    SbvVideoCapture cap("/dev/video1", 1280, 720, CAP_YUYV);

    cap.open();

    while(cap.isOpened()) {
        cv::Mat frame = cap.readMat();

        if (frame.empty() || frame.data == nullptr)
            continue;

        cv::imshow("frame", frame);
        cv::waitKey(1);
    }

    cap.close();

    return 0;
}