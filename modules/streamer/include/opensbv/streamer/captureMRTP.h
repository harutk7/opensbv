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
#include <mutex>

#include "opensbv/streamer/abstractCapture.h"
#include "opensbv/helpers/general/GeneralHelper.h"

using namespace std::chrono;

namespace opensbv {
    namespace streamer {

        /// Chunk unit class
        class ChunkUnit {
        public:
            long long int m_timestamp;
            int m_maxchunk = 0;
            int m_curchunk = 0;
            std::vector<unsigned char> m_data;

            ChunkUnit(int maxchunk, long long int timestamp) {
                m_maxchunk = maxchunk;
                m_timestamp = timestamp;
            }

            void fill(char *data, unsigned long length) {
                std::copy(data + 0, data + length, std::back_inserter(m_data));
            }
        };


        /// Client chunk combiner
        class ChunkCombiner {
            std::vector<unsigned char> mData; ///< vector of data
            size_t mLength = 0; ///< Length of buffer
            int mMaxChunk = 0; ///< max chunks
            int mCurChunk = 0; ///< current chunk
            long long int mTimestamp = 0; ///< current timestamp

            bool mReady = false; ///< is full package combined or not

            std::vector<int> m_remList; ///< remove list;
            std::vector<ChunkUnit> m_units;

        public:
            /**
             * constructor
             */
            ChunkCombiner();
            /**
             * destructor
             */
            ~ChunkCombiner();

            /**
             * add chunk
             * @param buf
             * @param n
             */
            void add(char *buf, size_t n);

            /**
             * us full package combined and read or not
             * @return
             */
            bool isReady();

            /**
             * get timestamp of frame
             * @return
             */
            long long int getTimestmap();

            /**
             * get next package
             * @return
             */
            std::vector<unsigned char>& getNext();

            /*
             * delete next package
             */
            void deleteNext();
        };

        /// Capture from MRTP streaming protocol
        class CaptureMRTP: public AbstractCapture {

            unsigned char *mData; ///< data of current package
            size_t mLength; ///< length of package
            std::mutex mTx; ///< mutex for reading writing data
            long long int mTimestamp; ///< timestamp

        public:

            /**
             * constructor
             */
            CaptureMRTP();
            /**
             * destructor
             */
            ~CaptureMRTP();

            /**
             * fill the data
             * @param vec regerance of combined vector package
             */
            void fill(std::vector<unsigned char> &vec);

            /*
             * data receive from udp server
             */
            void onRecv(char *buf, size_t n, long long int timestamp);

            /**
             * get current timestamp of frame
             * @return
             */
            long long int getTimestamp();

            /**
             * get data
             * @return
             */
            std::vector<unsigned char> getData();
        };

    }
}

#endif //OPENSBV_CAPTUREMRTSP_H