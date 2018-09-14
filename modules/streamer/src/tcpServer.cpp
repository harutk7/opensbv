//
// Created by harut on 9/14/18.
//

#include "opensbv/streamer/tcpServer.h"

namespace opensbv {
    namespace streamer {

        TcpServer::TcpServer(int port) : m_port(port) {}

        int TcpServer::run() {

            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0)
                return -1;
            bzero((char *) &serv_addr, sizeof(serv_addr));
            portno = m_port;
            serv_addr.sin_family = AF_INET;
            serv_addr.sin_addr.s_addr = INADDR_ANY;
            serv_addr.sin_port = htons(portno);
            int enable = 1;
            if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
                return 1;
            if (bind(sockfd, (struct sockaddr *) &serv_addr,
                     sizeof(serv_addr)) < 0)
                return -1;
            listen(sockfd,5);
            clilen = sizeof(cli_addr);
            newsockfd = accept(sockfd,
                               (struct sockaddr *) &cli_addr,
                               &clilen);
            if (newsockfd < 0)
                return -1;

            return 0;
        }

        size_t TcpServer::recv(char *buffer, size_t size) {
            return read(newsockfd,buffer,size);
        }
    }
}