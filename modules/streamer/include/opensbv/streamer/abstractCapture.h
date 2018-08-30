//
// Created by harut on 7/23/18.
//

#ifndef OPENSBV_ABSTRACTCAPTURE_H
#define OPENSBV_ABSTRACTCAPTURE_H

#include <cstddef>

namespace opensbv {
    namespace streamer {

        /// Abstract capture type for specific captures
        class AbstractCapture {

        public:
            /**
             * on recv function
             * @param buf
             * @param n
             */
            virtual void onRecv(char *buf, size_t n) = 0;

            /**
             * get data of frame
             * @return
             */
            virtual std::vector<unsigned char> getData() = 0;

            /**
             * get frame timestamp if available or receive timestamp
             * @return
             */
            virtual uint32_t getTimestamp() = 0;
        };
    }
}

#endif //OPENSBV_ABSTRACTCAPTURE_H
