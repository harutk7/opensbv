//
// Created by harut on 6/28/18.
//

#ifndef RTSPSTREAMER_STREAMBUFFER_H
#define RTSPSTREAMER_STREAMBUFFER_H

#include <cstddef>
#include <mutex>
#include <pthread.h>

namespace opensbv {
    namespace streamer {

        class StreamBuffer {
            unsigned char* mData = nullptr;
            size_t mLength = 0;
            uint32_t mTimestamp = 0;

            mutable pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

        public:

            StreamBuffer();

            ~StreamBuffer();

            void assign(unsigned char* mData, size_t mLength, uint32_t mTimestamp);

            uint32_t getTimestamp();

            StreamBuffer& operator=(const StreamBuffer& other)
            {
                // check for self-assignment
                if(&other == this)
                    return *this;
                // reuse storage when possible

                pthread_mutex_lock(&other.mutex);

                try {
                    if (mLength != 0)
                        delete this->mData;
                    this->mData = new unsigned char[other.mLength];

                    std::copy(other.mData + 0, other.mData + other.mLength, mData + 0);
                    mLength = other.mLength;
                    mTimestamp = other.mTimestamp;
                } catch (...) {

                }

                pthread_mutex_unlock(&other.mutex);

                return *this;
            }

            unsigned char* getData();

            uint32_t getLength();
        };
    }
}

#endif //RTSPSTREAMER_STREAMBUFFER_H
