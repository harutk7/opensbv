//
// Created by harut on 7/24/18.
//

#ifndef OPENSBV_UDPSERVER_H
#define OPENSBV_UDPSERVER_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "opensbv/streamer/abstractCapture.h"

using boost::asio::ip::udp;

namespace opensbv {
    namespace streamer {
        class UdpServer
        {
            udp::socket socket_;
            udp::endpoint sender_endpoint_;
            enum { max_length = 65000 };
            char data_[max_length];

            AbstractCapture *mCapture = nullptr;
        public:
            UdpServer(boost::asio::io_service& io_service, short port);

            void do_receive();

            void do_send(std::size_t length);

            void setCapture(AbstractCapture *cap);

        };
    }
}
#endif //OPENSBV_UDPSERVER_H
