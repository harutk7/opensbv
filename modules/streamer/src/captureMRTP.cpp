//
// Created by harut on 7/23/18.
//

#include "opensbv/streamer/captureMRTP.h"

namespace opensbv {
    namespace streamer {

        // MAIN CLASS
        CaptureMRTP::CaptureMRTP(): mData() {

        }

        CaptureMRTP::~CaptureMRTP() {}

        void CaptureMRTP::onRecv(char *buf, size_t n) {

            mCombiner.add(buf, n);

            if (mCombiner.isReady())
                fill(mCombiner.getNext());
        }

        std::vector<unsigned char> CaptureMRTP::getData() {
            return mData;
        }

        void CaptureMRTP::fill(std::vector<unsigned char> vec) {
            mData = vec;
        }

        // CHUNK COMBINER CLASS
        ChunkCombiner::ChunkCombiner() {

        }

        ChunkCombiner::~ChunkCombiner() {

        }

        bool ChunkCombiner::isReady() {
            return mReady;
        }

        std::vector<unsigned char> ChunkCombiner::getNext() {

            unsigned long a = (unsigned long)duration_cast< milliseconds >(
                    system_clock::now().time_since_epoch()
            ).count();


//            std::cout << a - mTimestamp << std::endl;
            return mData;
        }

        void ChunkCombiner::deleteNext() {

        }

        void ChunkCombiner::add(char *buf, size_t n) {

            std::string bufStr(buf + (n - 40), buf + n);

            std::size_t found = bufStr.find("-+-");
            if (found!=std::string::npos) {
                std::string str2 = bufStr.substr (found + 3,bufStr.length() - 1);

                auto infos = opensbv::helpers::general::GeneralHelper::explode(str2, '-');

                if (infos.size() == 4) {
                    size_t length = std::stoi(infos[0]);
                    int maxChunk = std::stoi(infos[1]);
                    int curChunk = std::stoi(infos[2]);
                    unsigned long timestamp = std::stoul(infos[3]);

                    if (timestamp != mTimestamp) {
                        mData.clear();
                        mLength = 0;
                        mMaxChunk = maxChunk;
                        mCurChunk = curChunk;
                        mTimestamp = timestamp;
                    } else {
                        mCurChunk = curChunk;
                    }

                    mLength += (n - 40 + found);
                    std::copy(buf + 0, buf + (n - 40 + found), std::back_inserter(mData));
                    mReady = mLength == length;
                }
            }

        }

    }
}