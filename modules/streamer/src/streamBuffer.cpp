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

        void StreamBuffer::assign(unsigned char *Data, size_t Length, unsigned long Timestamp) {

            mtx.lock();

            mData = Data;
            mLength = Length;
            mTimestamp = Timestamp;

            mtx.unlock();
        }

        unsigned long StreamBuffer::getTimestamp() {
            return mTimestamp;
        }

        unsigned char* StreamBuffer::getDate() {
            return mData;
        }

        unsigned long StreamBuffer::getLength() {
            return mLength;
        }
    }
}