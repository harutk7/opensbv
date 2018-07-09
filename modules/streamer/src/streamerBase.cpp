//
// Created by harut on 7/6/18.
//

#include "opensbv/streamer/streamerBase.h"


namespace opensbv {
    namespace streamer {

        StreamerBase::StreamerBase() {
            mQuality = 100;
        }

        void StreamerBase::setQuality(unsigned short quality) {
            mQuality = quality;
        }

        void StreamerBase::setWidth(unsigned short width) {
            mWitdh = width;
        }

        void StreamerBase::setHeight(unsigned short height) {
            mHeight = height;
        }

        void StreamerBase::setColorType(imageColorType type) {
            mImageColorType = type;
        }
    }
}
