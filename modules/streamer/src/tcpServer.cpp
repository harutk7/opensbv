//
// Created by harut on 9/5/18.
//

#include "opensbv/streamer/tcpServer.h"

namespace opensbv {
    namespace streamer {

        TcpServer::TcpServer(AbstractCapture* capture, std::string host, short port): m_port(port), m_host(host), mCapture(capture)
        {
        }

        TcpServer::~TcpServer() {
        }

        void TcpServer::run() {

            boost::asio::io_service io_service;

            server(io_service, m_port);

        }

        void TcpServer::session(tcp::socket sock, AbstractCapture *capture)
        {
            try
            {
                for (;;)
                {
                    char data[700000];

                    boost::system::error_code error;
                    size_t length = sock.read_some(boost::asio::buffer(data), error);

                    capture->onRecv(data, length);

                    if (error == boost::asio::error::eof)
                        break; // Connection closed cleanly by peer.
                    else if (error)
                        throw boost::system::system_error(error); // Some other error.
                }
            }
            catch (std::exception& e)
            {
                std::cerr << "Exception in thread: " << e.what() << "\n";
            }
        }

        void TcpServer::server(boost::asio::io_service& io_service, unsigned short port)
        {
            tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
            for (;;)
            {
                tcp::socket sock(io_service);
                a.accept(sock);
                std::thread(TcpServer::session, std::move(sock), mCapture).detach();
            }
        }
    }
}