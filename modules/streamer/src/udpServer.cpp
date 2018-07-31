//
// Created by harut on 7/24/18.
//

#include "opensbv/streamer/udpServer.h"


namespace opensbv {
    namespace streamer {
        UdpServer::UdpServer(AbstractCapture* capture, std::string host, short port) : mListen_endpoint(
                boost::asio::ip::address::from_string(host),
                port), mClient(mListen_endpoint)
                 {
             mCapture = capture;
        }

        UdpServer::~UdpServer() {
        }

        void UdpServer::run() {
            for (;;)
            {
                boost::this_thread::interruption_point();
                char data[max_length];
                boost::system::error_code ec;
                std::size_t n = mClient.receive(boost::asio::buffer(data),
                                          boost::posix_time::seconds(1), ec);
                if (ec)
                {
                    throw std::exception();
                }
                else
                {
                    mCapture->onRecv(data, n);
                }
            }
        }
    }
}