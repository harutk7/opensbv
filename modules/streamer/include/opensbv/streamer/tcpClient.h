//
// Created by harut on 9/14/18.
//

#ifndef OPENSBV_TCPCLIENT_H
#define OPENSBV_TCPCLIENT_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

namespace opensbv {
    namespace streamer {

        /// TCP Client
        class TcpClient {
            std::string m_host; ///< hostname
            int m_port; ///< port
            int sockfd, portno, n;
            struct sockaddr_in serv_addr;
            struct hostent *server;

        public:
            /**
             * Constructor
             * @param host
             * @param port
             */
            TcpClient(std::string host, int port);

            /**
             * Connect
             * @return
             */
            int Connect();

            /**
             * Send
             * @param buf
             * @param size
             * @return
             */
            size_t send(const char* buf, size_t size);
        };
    }
}

#endif //OPENSBV_TCPCLIENT_H