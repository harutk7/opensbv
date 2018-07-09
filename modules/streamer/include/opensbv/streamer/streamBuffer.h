//
// Created by harut on 6/28/18.
//

#ifndef RTSPSTREAMER_STREAMBUFFER_H
#define RTSPSTREAMER_STREAMBUFFER_H

#include <cstddef>
#include <mutex>

namespace opensbv {
    namespace streamer {

        class StreamBuffer {
            unsigned char* mData = nullptr;
            size_t mLength = 0;
            unsigned long mTimestamp = 0;
            mutable std::mutex mtx;

        public:

            StreamBuffer();

            ~StreamBuffer();

            void assign(unsigned char* mData, size_t mLength, unsigned long mTimestamp);

            unsigned long getTimestamp();

            StreamBuffer& operator=(const StreamBuffer& other)
            {
                // check for self-assignment
                if(&other == this)
                    return *this;
                // reuse storage when possible

                other.mtx.lock();

                try {
                    delete this->mData;
                    this->mData = new unsigned char[other.mLength];
                    std::copy(other.mData + 0, other.mData + other.mLength, mData + 0);
                    mLength = other.mLength;
                    mTimestamp = other.mTimestamp;
                } catch (...) {

                }

                other.mtx.unlock();

                return *this;
            }

            unsigned char* getData();

            unsigned long getLength();
        };
    }
}

#endif //RTSPSTREAMER_STREAMBUFFER_H
