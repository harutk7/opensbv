//
// Created by harut on 8/15/17.
//

#include "opensbv/streamer/StreamerMRTSP.h"

StreamerMRTSP::StreamerMRTSP():StreamerBase() {
    m_streamDataType = STREAM_DATA_BYTES;

    this->m_streamerListenT = 0;
    this->m_clientListenT = 0;
}

StreamerMRTSP::~StreamerMRTSP() {

}

bool StreamerMRTSP::BindTCP() {
    try {
        m_sockFd = socket(AF_INET, SOCK_STREAM, IPPROTO_IP); // create socket
        int enable = 1;
        setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)); // make it reusable after exit
        int nodelay_flag = 1;
        setsockopt(m_sockFd, IPPROTO_TCP, TCP_NODELAY, (void*) &nodelay_flag, sizeof(int));
        int lsock = 0;
        int flags = fcntl(lsock, F_GETFL, 0);
        fcntl(lsock, F_SETFL, flags | O_NONBLOCK);
        m_serv_addr.sin_family = AF_INET;
        m_serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
        m_serv_addr.sin_port = htons(this->m_tcpPort);
        if (bind(m_sockFd, (struct sockaddr *) &m_serv_addr,sizeof(m_serv_addr)) < 0) {
            std::cout << "error running stream server... trying again." << std::endl;
            return false;
        }

        listen(m_sockFd,2); // listen to socket

        std::cout << "mrtsp streaming on: " << this->m_tcpPort << std::endl;
    } catch (...) {
        std::cerr << "error on tcp... trying again." << std::endl;
        return false;
    }

    return true;
}

bool StreamerMRTSP::Run() {
    this->m_tcpPort = m_streamerPort;

    m_isStreamReady = false; // stream is not ready here yet
    m_streamRunning = true;

    // bind tcp until it is ready
    while(!this->BindTCP()) {
        sleep(2); // sleep for 2 seconds after next try
    }

    // run thread that listens clients
    m_listenParams.sockFd = &this->m_sockFd;
    m_listenParams.mutex_client = &this->m_mutex_client;
    m_listenParams.clientList = &this->m_udpClientList;
    pthread_create(&this->m_clientListenT,NULL,&StreamerMRTSP::listenWorker,&m_listenParams);

    m_streamerParams.data = &m_streamerData;
    m_streamerParams.isStreamReady = &this->m_isStreamReady;
    m_streamerParams.mutex_client = &this->m_mutex_client;
    m_streamerParams.mutex_stream = &this->m_mutex_stream;
    m_streamerParams.clientList = &this->m_udpClientList;
    m_streamerParams.m_running = &this->m_streamRunning;
    pthread_create(&this->m_streamerListenT,NULL,&StreamerMRTSP::streamerWorker,&m_streamerParams);

    return true;
}

void *StreamerMRTSP::listenWorker(void *ptr) {
    // parameters structure
    streamerMRTSPListenWorkerStruct params = *(streamerMRTSPListenWorkerStruct *)ptr;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    pthread_t listenCommandT[65535];
    int newSockFd; // new client socket fd
    streamerMRTSPListenCommandWorkerStruct clientCommandParams;
    clientCommandParams.worker_join_mutex = PTHREAD_MUTEX_INITIALIZER;
    clilen = sizeof(cli_addr);

    // accept connection from client
    while((newSockFd = accept(*(params.sockFd), (struct sockaddr *) &cli_addr, &clilen))) {
        try {

            int yes = 1;
            setsockopt(newSockFd, SOL_SOCKET, SO_KEEPALIVE, &yes, sizeof(int));

            int idle = 1;
            setsockopt(newSockFd, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(int));

            int interval = 1;
            setsockopt(newSockFd, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(int));

            int maxpkt = 10;
            setsockopt(newSockFd, IPPROTO_TCP, TCP_KEEPCNT, &maxpkt, sizeof(int));

            pthread_mutex_lock(&clientCommandParams.worker_join_mutex);

            // apply socket fd params
            streamerMRTSPClientStruct currentClient;
            currentClient.sockFd = newSockFd;
            currentClient.failedToSend = 0;
            currentClient.lastActive = GeneralHelper::GetTimestamp();
            currentClient.ready = false;
            currentClient.clientIp = std::string(inet_ntoa(cli_addr.sin_addr));
            currentClient.clientPort = 0;

            clientCommandParams.sockFd = *(params.sockFd); // server fd
            clientCommandParams.clientList = params.clientList;
            clientCommandParams.currentClient = currentClient;
            clientCommandParams.mutex_client = params.mutex_client;

            // run thread of current client command
            pthread_create(&listenCommandT[newSockFd],NULL,&StreamerMRTSP::listenCommandWorker,&clientCommandParams);

            usleep(1000); // wait for 1 ms
        } catch (...) {
            std::cerr << "error in listenWorker loop" << endl;
        }
    }
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, nullptr);
}

void *StreamerMRTSP::listenCommandWorker(void *ptr) {
    streamerMRTSPListenCommandWorkerStruct params = *(streamerMRTSPListenCommandWorkerStruct *)ptr; // params

    pthread_mutex_unlock(&((streamerMRTSPListenCommandWorkerStruct *)ptr)->worker_join_mutex);
    char receiveBuffer[MRTSP_TCP_BUFFER_SIZE] = {0}; // receive buffer
    unsigned short udpPort = MRTSP_UDP_PORT;
    char buffer[1024] = {0}; // buffer for headers
    std::string receivedData; // receive data string
    std::vector<std::string> receivedDataVector;
    bool skip;

    fd_set read_sd;
    FD_ZERO(&read_sd);
    FD_SET(params.currentClient.sockFd, &read_sd);

    while (true) {
        fd_set rsd = read_sd;

        int sel = select(params.currentClient.sockFd + 1, &rsd, 0, 0, 0);

        if (sel > 0) {
            skip = false;
            // client has performed some activity (sent data or disconnected?)

            ssize_t bytes = recv(params.currentClient.sockFd,receiveBuffer, MRTSP_TCP_BUFFER_SIZE, 0);

            if (bytes > 0) {
                try {
                    // got data from the client.
                    receivedData.assign(receiveBuffer); // assign to string

                    // split buffer
                    receivedDataVector = GeneralHelper::explode(receivedData, ':');
                    if (!receivedDataVector.empty()) {
                        if (receivedDataVector[0] == "SETUP" && receivedDataVector.size() >= 2) {

                            pthread_mutex_lock(params.mutex_client);

//                            for (int j = 0; j < params.clientList->size(); j++) {
//                                if ((*(params.clientList))[j].clientIp == params.currentClient.clientIp && (*(params.clientList))[j].clientPort == (unsigned short)stoi(receivedDataVector[1]) && (*(params.clientList))[j].clientPort > 0) {
//                                    skip = true;
//                                }
//                            }

                            if (!skip) {
                                params.currentClient.udpClient = StreamerBase::GetUDPClient((char *)params.currentClient.clientIp.c_str(), (unsigned short)stoi(receivedDataVector[1]), false);
                                params.currentClient.ready = true;
                                params.currentClient.clientPort = (unsigned short)stoi(receivedDataVector[1]);
                                params.currentClient.lastActive = GeneralHelper::GetTimestamp();
                                params.clientList->push_back(params.currentClient);
                            }

                            pthread_mutex_unlock(params.mutex_client);
                        }
                    }
                } catch (...) {
                    std::cerr << "ERROR: StreamerMRTSP::listenCommandWorker() 1" << std::endl;
                }
            }
            else if (bytes == 0) {
                pthread_mutex_lock(params.mutex_client);
                try {
                    for( auto iter = params.clientList->begin(); iter != params.clientList->end(); ++iter )
                    {
                        if( iter->sockFd == params.currentClient.sockFd )
                        {
                            params.clientList->erase( iter );
                            break;
                        }
                    }
                } catch (...) {
                    std::cerr << "ERROR: StreamerMRTSP::listenCommandWorker() 2" << std::endl;
                }
                pthread_mutex_unlock(params.mutex_client);
                break;
            }
            else {
                // error receiving data from client. You may want to break from
                // while-loop here as well.
            }
        }
        else if (sel < 0) {
            // grave error occurred.
            break;
        }
    }

    close(params.currentClient.sockFd);
}

bool StreamerMRTSP::Stop() {
    try {
        this->m_streamRunning = false;
        if(this->m_clientListenT != 0)
        {
            pthread_cancel(this->m_clientListenT);
            close(this->m_sockFd);
        }
        if(this->m_streamerListenT != 0)
        {
            pthread_join(this->m_streamerListenT, nullptr);
        }
        std::cout << "mrtsp streaming stopped" << std::endl;

    } catch(...) {
        std::cerr << "error in StreamerMRTSP::Stop()" << std::endl;
    }
}

void StreamerMRTSP::SetStreamDataType(enum stream_data_type dataType) {
    m_streamDataType = dataType;
}

size_t StreamerMRTSP::Write(unsigned char *data, ssize_t size) {
    try {
        pthread_mutex_lock(&m_mutex_stream); // lock
        if (!m_isStreamReady)
            m_isStreamReady = true;

        if (m_streamDataType == STREAM_DATA_IMAGE) {
            if (!ImageHelper::compress_jpg(data, m_imageColorType, &m_imageBuffer, GetImageWidth(), GetImageHeight(), m_jpegQuality))
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

    return m_imageBuffer.buffersize;
}

void *StreamerMRTSP::streamerWorker(void *ptr) {
    // parameters struct
    streamerMRTSPWorkerStruct params = *(streamerMRTSPWorkerStruct *)ptr;
    long oldTimestamp = 0; // timestamp to compare with
    int total_pack; // num of packs to send
    std::vector<unsigned char> encoded;
    unsigned long oldSize = 0;
    unsigned char *imageBuf;
    unsigned char *currentBuf;

    int i = 0;
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
                if (true) {
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

                        pthread_mutex_lock(params.mutex_client); // unlock
                        for (int j = 0; j < params.clientList->size(); j++) {
                            if ((*(params.clientList))[j].ready) {
                                if (i == total_pack - 1) {
                                    if (sendto((*(params.clientList))[j].udpClient.sockFd, currentBuf, encoded.size() - (i* UDP_PACK_SIZE), MSG_NOSIGNAL,
                                               (struct sockaddr *)&(*(params.clientList))[j].udpClient.their_addr, sizeof ((*(params.clientList))[j].udpClient.their_addr)) < 0) {
                                    }
//                            params.sock->sendTo( currentBuf, encoded.size() - (i* UDP_PACK_SIZE), params.serverAddress, params.serverPort);
                                } else {
                                    if (sendto((*(params.clientList))[j].udpClient.sockFd, currentBuf, UDP_PACK_SIZE, MSG_NOSIGNAL,
                                               (struct sockaddr *)&(*(params.clientList))[j].udpClient.their_addr, sizeof ((*(params.clientList))[j].udpClient.their_addr)) < 0) {
                                    };
//                            params.sock->sendTo( currentBuf, UDP_PACK_SIZE, params.serverAddress, params.serverPort);
                                }
                            }
                        }
                        pthread_mutex_unlock(params.mutex_client); // unlock

                        free(currentBuf);
                    }
                    free(imageBuf);
                }
            }

            pthread_mutex_unlock(params.mutex_stream); // unlock

            usleep(UDP_STREAM_WAIT_INTERVAL); // sleep for 1 ms
        } catch(...) {
            std::cerr << "exception on streamWorker loop" << std::endl;
            continue;
        }
    }

    try {
        // close clients
        for (int j = 0; j < params.clientList->size(); j++) {
            close((*(params.clientList))[j].sockFd);
        }
    } catch (...) {
        std::cerr << "ERROR: erase client from streamworker()" << std::endl;
    }
}