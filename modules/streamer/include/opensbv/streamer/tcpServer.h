//
// Created by harut on 9/5/18.
//

#ifndef OPENSBV_TCPSERVER_H
#define OPENSBV_TCPSERVER_H

#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "opensbv/streamer/abstractCapture.h"

using boost::asio::ip::tcp;

namespace opensbv {
    namespace streamer {

        ///< Tcp Server
        class TcpServer
        {

            std::string m_host; ///< bind host
            short m_port; ///< bind port
            AbstractCapture *mCapture = nullptr; ///<  capture instance
        public:
            /**
             * Constructor
             * @param capture
             * @param host
             * @param port
             */
            TcpServer(AbstractCapture *capture, std::string host, short port);

            /**
             * Destructor
             */
            ~TcpServer();

            /**
             * Run
             */
            void run();

            /**
             * Client connection holder
             * @param sock
             * @param capture
             */
            static void session(tcp::socket sock, AbstractCapture *capture);

            /**
             * Server Holder
             * @param io_service
             * @param port
             */
            void server(boost::asio::io_service& io_service, unsigned short port);
        };

    }
}

#endif //OPENSBV_TCPSERVER_H