//
// Created by harut on 7/5/18.
//

#include "capture.h"

int main(int argc, char* argv[]) {

    Capture cap("/dev/video1");
    cap.Run();
}