//
// Created by harut on 9/5/18.
//

#include "opensbv/streamer/chunkSplitter.h"


namespace opensbv {
    namespace streamer {

        ChunkSplitter::ChunkSplitter(size_t chunkSize) : mChunkSize(chunkSize) {

        }

        ChunkSplitter::~ChunkSplitter() {

        }

        bool ChunkSplitter::hasNext() {
            return !mList.empty();
        }


        std::vector<unsigned char> ChunkSplitter::getNext() {
            return mList.front();
        }

        void ChunkSplitter::deleteNext() {
            mList.remove(mList.front());
        }

        void ChunkSplitter::split(unsigned char *buf, size_t n, long long int timeStamp) {

            size_t startPos = 0;
            size_t endPos = 0;

            size_t maxCount = n / mChunkSize + 1;

            std::string prefix = "-+-" + std::to_string(n) + "-" + std::to_string(maxCount) + "-";

            int currentCount = 0;
            std::string currentPrefix;

            while(true) {
                if (n > endPos) {

                    currentCount++;
                    currentPrefix = prefix + std::to_string(currentCount) + "-" + std::to_string(timeStamp) + "-";

                    startPos = endPos;

                    if (n - endPos > mChunkSize) {
                        endPos +=mChunkSize;
                    } else {
                        endPos = n;
                    }

                    std::vector<unsigned char> currentChunk;
                    currentChunk.assign(buf + startPos,  buf + endPos);

                    std::copy(currentPrefix.c_str() + 0, currentPrefix.c_str() + currentPrefix.length(), std::back_inserter(currentChunk));

                    mList.emplace_back(currentChunk);
                } else {
                    break;
                }
            }
        }
    }
}