//
// Created by harut on 7/5/18.
//

#include "opensbv/streamer/capture.h"
#include "opensbv/helpers/general/GeneralHelper.h"

#include <opencv2/opencv.hpp>

using namespace opensbv::streamer;
using namespace opensbv::helpers::general;

int main(int argc, char* argv[]) {

    if (argc < 3)
        return 1;

    std::string host = argv[1];
    ushort port = (ushort)std::stoi(argv[2]);

    Capture cap(CAPTURE_MRTP, port);
    cap.setHost(host);
    cap.setPort(8554);
    if(!cap.connect())
        return 0;
    cap.run();

    std::string filename;
    filename = host + "_" + GeneralHelper::GetDateTime() + ".avi";

    cv::VideoWriter video(filename,CV_FOURCC('M','J','P','G'),10, cv::Size(1280,720));

    int count = 1;
    while(cap.isOpened()) {
        try {
            cv::Mat frame = cap.readMat();
            if (frame.empty())
                continue;

//            video.write(frame);

            count++;
            if (count == 3)
                break;
            cv::imshow("frame", frame);
            cv::waitKey(1);

        } catch(StreamerException &e) {

        }
    }

    cap.stop();

    return 0;
}