//
// Created by harut on 7/2/18.
//

#include "opensbv/streamer/streamBuffer.h"


namespace opensbv {
    namespace streamer {

        StreamBuffer::StreamBuffer() {

        }

        StreamBuffer::~StreamBuffer() {
            delete mData;
        }

        void StreamBuffer::assign(unsigned char *Data, size_t Length, uint32_t Timestamp) {

            pthread_mutex_lock(&mutex);

            if (mLength != 0) {
                delete mData;
            }

            mData = new unsigned char[Length];
            std::copy(Data + 0, Data + Length, mData + 0);
            mLength = Length;
            mTimestamp = Timestamp;

            pthread_mutex_unlock(&mutex);
        }

        uint32_t StreamBuffer::getTimestamp() {
            return mTimestamp;
        }

        unsigned char* StreamBuffer::getData() {
            return mData;
        }

        uint32_t StreamBuffer::getLength() {
            return mLength;
        }
    }
}