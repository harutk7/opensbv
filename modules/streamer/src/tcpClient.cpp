//
// Created by harut on 9/14/18.
//

#include "opensbv/streamer/tcpClient.h"

namespace opensbv {
    namespace streamer {

        TcpClient::TcpClient(std::string host, int port): m_port(port), m_host(host) {}

        int TcpClient::Connect() {

            portno = m_port;
            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0)
                return -1;
            server = gethostbyname(m_host.c_str());
            if (server == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr,
                  (char *)&serv_addr.sin_addr.s_addr,
                  server->h_length);
            serv_addr.sin_port = htons(portno);
            if (connect(sockfd,(struct sockaddr *) &serv_addr ,sizeof(serv_addr)) < 0)
                return -1;

            return 0;
        }

        size_t TcpClient::send(const unsigned char *buf, size_t size) {
            return write(sockfd,buf+0,size);
        }
    }
}
