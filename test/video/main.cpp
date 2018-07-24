//
// Created by harut on 7/5/18.
//

#include "opensbv/camera/SbvVideoCapture.h"

using namespace opensbv::camera;

int main(int argc, char* argv[]) {

    SbvVideoCapture cap("/dev/video1", 1280, 720, CAP_YUYV);
    SbvVideoCapture a = cap;

    cap.open();

    while(cap.isOpened()) {
        cv::Mat frame = cap.readMat();

        if (frame.empty())
            continue;

        cv::imshow("frame", frame);
        cv::waitKey(1);
    }

    cap.close();

    return 0;
}