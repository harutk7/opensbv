//
// Created by harut on 7/23/18.
//

#ifndef OPENSBV_CAPTUREMRTSP_H
#define OPENSBV_CAPTUREMRTSP_H

#include <iostream>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <chrono>
#include <cstdlib>

#include "opensbv/streamer/abstractCapture.h"
#include "opensbv/helpers/general/GeneralHelper.h"

using namespace std::chrono;

namespace opensbv {
    namespace streamer {

        class ChunkCombiner {
            std::vector<unsigned char> mData;
            size_t mLength = 0;
            int mMaxChunk = 0;
            int mCurChunk = 0;
            unsigned long mTimestamp = 0;

            bool mReady = false;

        public:
            ChunkCombiner();
            ~ChunkCombiner();

            void add(char *buf, size_t n);

            bool isReady();

            std::vector<unsigned char> getNext();

            void deleteNext();
        };

        class CaptureMRTP: public AbstractCapture {

            std::vector<unsigned char> mData;
            ChunkCombiner mCombiner;

        public:

            CaptureMRTP();
            ~CaptureMRTP();

            void fill(std::vector<unsigned char> vec);

            void onRecv(char *buf, size_t n);
            std::vector<unsigned char> getData();
        };

    }
}

#endif //OPENSBV_CAPTUREMRTSP_H