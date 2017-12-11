//
// Created by harut on 8/15/17.
//

#include "opensbv/streamer/StreamerBase.h"

StreamerBase::StreamerBase() {
    m_streamerData.buffer = new std::vector<unsigned char>();
    m_streamerData.timestamp = new long();

    m_imageBuffer.buffer = NULL;
    m_imageBuffer.buffersize = 0;

    m_dataBuffer.buffer = NULL;
    m_dataBuffer.buffersize = 0;

    m_jpegQuality = JPEG_ENCODE_QUALITY;
    m_imageColorType = IMAGE_COLOR_BGR;
}

StreamerBase::~StreamerBase() {

}

bool StreamerBase::Stop() {

}

bool StreamerBase::Run() {

}

size_t StreamerBase::Write(unsigned char *data, ssize_t size) {

}

udpClientWriterStruct StreamerBase::GetUDPClient(char hostname[15], unsigned short port, bool isBroadCast) {
    int sockfd;
    struct sockaddr_in their_addr; // connector's address information
    struct hostent *he;
    int numbytes;
    int broadcast = 1;
    //char broadcast = '1'; // if that doesn't work, try this

    if ((he=gethostbyname(hostname)) == NULL) {  // get the host info
        perror("gethostbyname");
        exit(1);
    }

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }

    if (isBroadCast) {
        // this call is what allows broadcast packets to be sent:
        if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcast,
                       sizeof broadcast) == -1) {
            perror("setsockopt (SO_BROADCAST)");
            exit(1);
        }
    }

    their_addr.sin_family = AF_INET;     // host byte order
    their_addr.sin_port = htons(port); // short, network byte order
    their_addr.sin_addr = *((struct in_addr *)he->h_addr);
    memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);

    return udpClientWriterStruct{sockfd, their_addr};
}

ushort StreamerBase::GetImageWidth() {
    return this->m_imageWidth;
}

ushort StreamerBase::GetImageHeight() {
    return this->m_imageHeight;
}

void StreamerBase::SetImageWidth(ushort width) {
    this->m_imageWidth = width;
}

void StreamerBase::SetImageHeight(ushort height) {
    this->m_imageHeight = height;
}

void StreamerBase::SetImageQuality(ushort quality) {
    m_jpegQuality = quality;
}

void StreamerBase::SetPort(ushort port) {
    this->m_streamerPort = port;
}

void StreamerBase::SetImageColorType(imageColorType colorType) {
    this->m_imageColorType = colorType;
}