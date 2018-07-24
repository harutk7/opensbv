//
// Created by harut on 7/24/18.
//

#include "opensbv/streamer/udpServer.h"


namespace opensbv {
    namespace streamer {
        UdpServer::UdpServer(boost::asio::io_service &io_service, short port)
                : socket_(io_service, udp::endpoint(udp::v4(), port)) {
            std::cout << "udp server running on port: " << port << std::endl;
        }

        void UdpServer::do_receive() {

            while(true) {
                boost::this_thread::interruption_point();
                size_t bytes_recvd = socket_.receive_from(
                        boost::asio::buffer(data_, max_length), sender_endpoint_);

                if (bytes_recvd > 0) {
                    if (mCapture != nullptr)
                        mCapture->onRecv(data_, bytes_recvd);
                }
            }
//            socket_.async_receive_from(
//                    boost::asio::buffer(data_, max_length), sender_endpoint_,
//                    [this](boost::system::error_code ec, std::size_t bytes_recvd) {
//                        boost::this_thread::interruption_point();
//                        if (!ec && bytes_recvd > 0) {
//                            if (mCapture != nullptr)
////                            do_send(bytes_recvd);
//                        }
//
//                        do_receive();
//                    });
        }

        void UdpServer::setCapture(AbstractCapture *cap) {
            mCapture = cap;
        }

        void UdpServer::do_send(std::size_t length) {
            socket_.async_send_to(
                    boost::asio::buffer(data_, length), sender_endpoint_,
                    [this](boost::system::error_code /*ec*/, std::size_t /*bytes_sent*/) {
                        do_receive();
                    });
        }
    }
}