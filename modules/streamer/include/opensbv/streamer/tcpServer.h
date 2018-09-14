//
// Created by harut on 9/14/18.
//

#ifndef OPENSBV_TCPSERVER_H
#define OPENSBV_TCPSERVER_H

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


namespace opensbv {
    namespace streamer {

        /// TCP Client
        class TcpServer {

            int m_port; ///< port
            int sockfd, newsockfd, portno;
            socklen_t clilen;
            struct sockaddr_in serv_addr, cli_addr;

        public:
            /**
             * Constructor
             * @param port
             */
            TcpServer(int port);

            /**
             * Run
             * @return
             */
            int run();

            /**
             * Receive
             * @param buffer
             * @param size
             * @return
             */
            size_t recv(char *buffer, size_t size);
        };
    }
}

#endif //OPENSBV_TCPSERVER_H
