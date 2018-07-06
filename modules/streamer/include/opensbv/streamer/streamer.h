//
// Created by harut on 7/6/18.
//

#ifndef OPENSBV_STREAMER_H
#define OPENSBV_STREAMER_H

#include <iostream>

#include "opensbv/streamer/streamerBase.h"

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/thread.hpp>

using boost::asio::ip::tcp;

namespace opensbv {
    namespace streamer {

        /// streamer thread worker
        void streamerWorkingThread(StreamerBase *streamer);

        /// Streamer class
        class Streamer {

            boost::thread mWorkingThread;
            StreamerBase* mStreamer;

        public:

            /// constructor
            Streamer();
            ~Streamer();

            ///
            void setStreamer(StreamerBase *streamer);

            void run();

            void stop();
        };
    }
}

#endif //OPENSBV_STREAMER_H
