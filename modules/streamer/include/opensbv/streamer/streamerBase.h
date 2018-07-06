//
// Created by harut on 7/6/18.
//

#ifndef OPENSBV_STREAMERBASE_H
#define OPENSBV_STREAMERBASE_H

#include <iostream>
#include "opensbv/streamer/streamBuffer.h"

namespace opensbv {
    namespace streamer {


        class StreamerBase {

        protected:
            bool m_streamingRunning = false; ///< Streaming running or not
            StreamBuffer mBuffer;

        public:
            /// Write frame to stremaing
            /**
             * Converts to Jpeg and Streams
             * @param data to stream
             * @param size
             * @return
             */
            virtual size_t Write(unsigned char *data, ssize_t size) = 0; // write to stream

            /// Run Streaming
            virtual void Run() = 0;

            /// Stop Streaming
            virtual void Stop() = 0;
        };
    }
}


#endif //OPENSBV_STREAMERBASE_H
