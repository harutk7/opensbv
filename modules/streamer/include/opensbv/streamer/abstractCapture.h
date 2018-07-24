//
// Created by harut on 7/23/18.
//

#ifndef OPENSBV_ABSTRACTCAPTURE_H
#define OPENSBV_ABSTRACTCAPTURE_H

#include <cstddef>

namespace opensbv {
    namespace streamer {

        class AbstractCapture {

        public:
            virtual void onRecv(char *buf, size_t n) = 0;

            virtual std::vector<unsigned char> getData() = 0;
        };
    }
}

#endif //OPENSBV_ABSTRACTCAPTURE_H
