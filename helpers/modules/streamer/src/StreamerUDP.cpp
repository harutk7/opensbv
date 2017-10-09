//
// Created by harut on 8/22/17.
//

#include <opensbv/helpers/general/GeneralHelper.h>
#include "opensbv/streamer/StreamerUDP.h"

StreamerUDP::StreamerUDP():StreamerBase() {
    sprintf(m_serverAddress, "192.168.168.4");
    m_streamDataType = STREAM_DATA_BYTES;
    this->m_streamerListenT = 0;
}

StreamerUDP::~StreamerUDP() {

}

bool StreamerUDP::Stop() {
    try {
        this->m_streamRunning = false;
        if(this->m_streamerListenT != 0)
        {
            pthread_join(this->m_streamerListenT, nullptr);
        }
        std::cout << "udp streaming stopped" << std::endl;

    } catch(...) {
        std::cerr << "error in StreamerUDP::Stop()" << std::endl;
    }
}

bool StreamerUDP::Run() {

    // run thread that send images
    m_streamerParams.mutex_stream = &this->m_mutex_stream;
    m_streamerParams.data = &m_streamerData;
    m_streamerParams.isStreamReady = &this->m_isStreamReady;
    std::copy(std::begin(this->m_serverAddress), std::end(this->m_serverAddress), std::begin(m_streamerParams.serverAddress));
    m_streamerParams.serverPort = m_streamerPort;
    m_streamerParams.m_running = &this->m_streamRunning;
    pthread_create(&this->m_streamerListenT,NULL,&StreamerUDP::streamerWorker,&m_streamerParams);

//    CPU_ZERO(&this->m_cpuSetStreamerWorker);
//    CPU_SET(1, &this->m_cpuSetStreamerWorker);
//    pthread_setaffinity_np(this->m_streamerListenT, sizeof(cpu_set_t), &this->m_cpuSetStreamerWorker);

    std::cout << "udp streaming run to: " << m_serverAddress << ":" << m_streamerPort << std::endl;
}

void StreamerUDP::SetStreamDataType(enum stream_data_type dataType) {
    m_streamDataType = dataType;
}

void StreamerUDP::Write(unsigned char *data, ssize_t size) {
    try {
        pthread_mutex_lock(&m_mutex_stream); // lock
        if (!m_isStreamReady)
            m_isStreamReady = true;

        if (m_streamDataType == STREAM_DATA_IMAGE) {
            if (!ImageHelper::compress_jpg(data, m_imageColorType,&m_imageBuffer, GetImageWidth(), GetImageHeight(), m_jpegQuality))
                m_isStreamReady = false;
            else
                m_streamerData.buffer->assign(m_imageBuffer.buffer, m_imageBuffer.buffer + m_imageBuffer.buffersize);
        } else if (m_streamDataType == STREAM_DATA_BYTES) {
            m_streamerData.buffer->assign(data,data + size);
        }
        *(m_streamerData.timestamp) = GeneralHelper::GetTimestamp(); // set timestamp

        // free buffer
        if (m_imageBuffer.buffer != nullptr)
            free(m_imageBuffer.buffer);
        if (m_dataBuffer.buffer != nullptr)
            free(m_dataBuffer.buffer);

        pthread_mutex_unlock(&m_mutex_stream); // unlock
    } catch (...) {
        std::cerr << "error in Write" << std:: endl;
    }
}

void *StreamerUDP::streamerWorker(void *ptr) {

    // parameters struct
    streamerUDPWorkerStruct params = *(streamerUDPWorkerStruct *)ptr;
    long oldTimestamp = 0; // timestamp to compare with
    int total_pack; // num of packs to send
    std::vector<unsigned char> encoded;
    unsigned long oldSize = 0;
    unsigned char *imageBuf;
    unsigned char *currentBuf;

    // udp client
    udpClientWriterStruct udpClient = StreamerBase::GetUDPClient(params.serverAddress, params.serverPort, true);

    // send with sleep for corresponding fps
    while (*(params.m_running)) {
        try {
            encoded.clear();

            pthread_mutex_lock(params.mutex_stream); // lock
            // check if is stream ready for start
            if (*(params.isStreamReady) && (oldTimestamp != *(params.data->timestamp) || oldTimestamp != 0)) {
                // update timestamp
                oldTimestamp = *(params.data->timestamp);

                // insert header
                encoded.insert(encoded.begin(),UDP_PKG_HEADER,UDP_PKG_HEADER+strlen(UDP_PKG_HEADER));
                // insert main image
                std::copy(params.data->buffer->begin(), params.data->buffer->end(), std::back_inserter(encoded));
                // insert footer
                encoded.insert(encoded.end(),UDP_PKG_FOOTER,UDP_PKG_FOOTER+strlen(UDP_PKG_FOOTER));

                // packs to send
                total_pack = (int)(1 + (encoded.size()) / UDP_PACK_SIZE);

                // check if not same frame
                if (oldSize != encoded.size()) {
                    oldSize = encoded.size();

                    imageBuf = (unsigned char *) malloc (encoded.size() * sizeof(unsigned char));
                    std::memcpy( imageBuf, encoded.data(), encoded.size() );

                    for (int i = 0; i < total_pack; i++) {

                        //--------------------
                        if (i == total_pack - 1) {
                            currentBuf = (unsigned char *) malloc ((encoded.size() - (i * UDP_PACK_SIZE))*sizeof(unsigned char));
                            std::memcpy( currentBuf, imageBuf + (i * UDP_PACK_SIZE), encoded.size() - (i* UDP_PACK_SIZE));

                        } else {
                            currentBuf = (unsigned char *) malloc (UDP_PACK_SIZE);
                            std::memcpy( currentBuf, imageBuf + (i * UDP_PACK_SIZE), UDP_PACK_SIZE);
                        }
//                        std::copy(buf + (i * UDP_PACK_SIZE), buf + (i * (2*UDP_PACK_SIZE)), currentBuf);
                    /*    string name;
                        int foundHeader = -1, foundFooter = -1;

                        char * pch;
                        pch = strstr ((char *)currentBuf,(char *)UDP_PKG_HEADER);
                        if (pch)
                            foundHeader = 1;

//                        if (i == total_pack - 1) {
//                            int a = 1;
//                        }
                        char * pch1;
                        pch1 = strstr ((char *)currentBuf,(char *)UDP_PKG_FOOTER);
                        if (pch1)
                            foundFooter = 1;

                        name = "buffer_" + std::to_string(*(params.data->timestamp)) + "_" + std::to_string(i+1) + "_" +  std::to_string(encoded.size()) + "_(" + std::to_string(foundHeader) + ")" + "(" + std::to_string(foundFooter) + ").txt";

                        std::ofstream outfile ((char *)name.c_str(),std::ofstream::binary);
                        if (i == total_pack - 1) {
                            outfile.write ((char *)currentBuf,encoded.size() - (i* UDP_PACK_SIZE));
                        } else {
                            outfile.write ((char *)currentBuf,UDP_PACK_SIZE);
                        }
                        outfile.close(); */

                        //---------------------
                        if (i == total_pack - 1) {
                            sendto(udpClient.sockFd, currentBuf, encoded.size() - (i* UDP_PACK_SIZE), 0,
                                                 (struct sockaddr *)&udpClient.their_addr, sizeof udpClient.their_addr);
//                            sock.sendTo( currentBuf, encoded.size() - (i* UDP_PACK_SIZE), params.serverAddress, params.serverPort);
                        } else {
                            sendto(udpClient.sockFd, currentBuf, UDP_PACK_SIZE, 0,
                                   (struct sockaddr *)&udpClient.their_addr, sizeof udpClient.their_addr);
//                            sock.sendTo( currentBuf, UDP_PACK_SIZE, params.serverAddress, params.serverPort);
                        }

                        free(currentBuf);
                    }
                    free(imageBuf);
                }
            }

            pthread_mutex_unlock(params.mutex_stream); // unlock

            usleep(UDP_STREAM_WAIT_INTERVAL); // sleep for 1 ms
        } catch(...) {
            std::cerr << "exception on streamer thread" << std::endl;
            continue;
        }
    }

    close(udpClient.sockFd);
}

void StreamerUDP::SetUdpStreamParams(char *ip, bool isBroadcast) {
    sprintf(m_serverAddress, ip);
}