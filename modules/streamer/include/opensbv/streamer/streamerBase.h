//
// Created by harut on 7/6/18.
//

#ifndef OPENSBV_STREAMERBASE_H
#define OPENSBV_STREAMERBASE_H

#include <iostream>
#include "opensbv/streamer/streamBuffer.h"
#include "opensbv/helpers/image/ImageHelper.h"
#include "opensbv/helpers/general/GeneralHelper.h"

using namespace opensbv::helpers::image;
using namespace opensbv::helpers::image;

namespace opensbv {
    namespace streamer {

        class StreamerBase {

        protected:
            unsigned short mQuality;

            unsigned short mWitdh;
            unsigned short mHeight;

            imageColorType mImageColorType;

            bool m_streamingRunning = false; ///< Streaming running or not
            StreamBuffer mBuffer;

        public:

            /**
             * constructor
             */
            StreamerBase();

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

            void setQuality(unsigned short quality);

            void setWidth(unsigned short width);
            void setHeight(unsigned short height);

            void setColorType(imageColorType type);
        };
    }
}


#endif //OPENSBV_STREAMERBASE_H
