//
// Created by harut on 8/15/17.
//

#include <opensbv/helpers/general/GeneralHelper.h>
#include "opensbv/streamer/StreamerMJPG.h"

// Constructor
StreamerMJPG::StreamerMJPG():StreamerBase() {
    this->m_clientListenT = 0;
    this->m_streamerListenT = 0;
}

// Destructor
StreamerMJPG::~StreamerMJPG() {

}

// Run
bool StreamerMJPG::Run() {
    this->m_port = m_streamerPort;
    m_isStreamReady = false; // stream is not ready here yet
    m_streamRunning = true;

    // bind tcp until it is ready
    while(!this->BindTCP()) {
        sleep(2); // sleep for 2 seconds after next try
    }

    // run thread that listens clients
    m_listenParams.sockFd = &this->m_sockFd;
    m_listenParams.socketList = &this->m_socketList;
    m_listenParams.mutex_client = &this->m_mutex_client;
    pthread_create(&this->m_clientListenT,NULL,&StreamerMJPG::listenWorker,&m_listenParams);

    // run thread that send images
    m_streamerParams.socketList = &this->m_socketList;
    m_streamerParams.mutex_client = &this->m_mutex_client;
    m_streamerParams.data = &m_streamerData;
    m_streamerParams.isStreamReady = &this->m_isStreamReady;
    m_streamerParams.m_running = &m_streamRunning;
    pthread_create(&this->m_streamerListenT,NULL,&StreamerMJPG::streamerWorker,&m_streamerParams);

    // set cpu affinities for this threads
//    CPU_ZERO(&this->m_cpuSetListenWorker);
//    CPU_SET(0, &this->m_cpuSetListenWorker);
//    pthread_setaffinity_np(this->m_clientListenT, sizeof(cpu_set_t), &this->m_cpuSetListenWorker);

//    CPU_ZERO(&this->m_cpuSetStreamerWorker);
//    CPU_SET(1, &this->m_cpuSetStreamerWorker);
//    pthread_setaffinity_np(this->m_streamerListenT, sizeof(cpu_set_t), &this->m_cpuSetStreamerWorker);
}

// Bind TCP Server
bool StreamerMJPG::BindTCP() {
    try {
        m_sockFd = socket(AF_INET, SOCK_STREAM, 0); // create socket
        int enable = 1;
        setsockopt(m_sockFd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)); // make it reusable after exit
        m_serv_addr.sin_family = AF_INET;
        m_serv_addr.sin_addr.s_addr = htons(INADDR_ANY);
        m_serv_addr.sin_port = htons(this->m_port);
        if (bind(m_sockFd, (struct sockaddr *) &m_serv_addr,sizeof(m_serv_addr)) < 0) {
            std::cout << "error running stream server... trying again." << std::endl;
            return false;
        }

        listen(m_sockFd,2); // listen to socket

        std::cout << "mjpg streaming on: " << this->m_port << std::endl;
    } catch (...) {
        std::cerr << "error on tcp... trying again." << std::endl;
        return false;
    }

    return true;
}

// Write with Mat and encode
size_t StreamerMJPG::Write(unsigned char *data, ssize_t size) {

    pthread_mutex_lock(&m_mutex_client); // lock

    if (!m_isStreamReady)
        m_isStreamReady = true;

    // compress with libjpeg
    if (!ImageHelper::compress_jpg(data, m_imageColorType, &m_imageBuffer, GetImageWidth(), GetImageHeight(), m_jpegQuality))
        m_isStreamReady = false;
    else
        m_streamerData.buffer->assign(m_imageBuffer.buffer, m_imageBuffer.buffer + m_imageBuffer.buffersize);

    // assign buffer
    *(m_streamerData.timestamp) = GeneralHelper::GetTimestamp(); // set timestamp

    // free buffer
    if (m_imageBuffer.buffer != nullptr)
        free(m_imageBuffer.buffer);

    pthread_mutex_unlock(&m_mutex_client); // unlock

    return m_imageBuffer.buffersize;
}

// Listen for clients to connect
void *StreamerMJPG::listenWorker(void *ptr) {

    // parameters structure
    streamerMJPGListenWorkerStruct params = *(streamerMJPGListenWorkerStruct *)ptr;
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    int newSockFd; // new client socket fd
    char buffer[BUFFER_SIZE] = {0}; // buffer for sending header

    sprintf(buffer, "HTTP/1.0 200 OK\r\n" \
            "Access-Control-Allow-Origin: *\r\n" \
            STD_HEADER \
            "Content-Type: multipart/x-mixed-replace;boundary=" BOUNDARY "\r\n" \
            "\r\n" \
            "--" BOUNDARY "\r\n");

    clilen = sizeof(cli_addr);

    // accept connection from client
    while((newSockFd = accept(*(params.sockFd), (struct sockaddr *) &cli_addr, &clilen))) {

        pthread_mutex_lock(params.mutex_client); // lock

        if (send(newSockFd,buffer,strlen(buffer), MSG_NOSIGNAL) < 0) { // send headers
            continue;
        }

        usleep(10000); // wait 10 ms

        params.socketList->insert(newSockFd); //insert client socket fd in list

        pthread_mutex_unlock(params.mutex_client); // unlock
    }
}

// Send stream to clients
void *StreamerMJPG::streamerWorker(void *ptr) {

    // parameters struct
    streamerMJPGWorkerStruct params = *(streamerMJPGWorkerStruct *)ptr;
    char buffer[BUFFER_SIZE] = {0}; // buffer for headers
    int disconnectClient = 0; // counter for detecting client disconnect
    std::set<int> disconnectSockList; // disconnected client list
    std::set<int>::iterator it; // iterator for socket set
    long oldTimestamp = 0; // timestamp to compare with
    unsigned long oldSize = 0;

    // send with sleep for corresponding fps
    while (*(params.m_running)) {
        try {
            pthread_mutex_lock(params.mutex_client); // lock
            if (*(params.isStreamReady) && (oldTimestamp != *(params.data->timestamp) || oldTimestamp != 0)) {
                // update timestamp
                oldTimestamp = *(params.data->timestamp);

                // check if not same frame
                if (oldSize != params.data->buffer->size()) {
                    oldSize = params.data->buffer->size();

                    // send data to clients
                    for (it = params.socketList->begin(); it != params.socketList->end(); ++it)
                    {
                        int sockFd = *it; // Note the "*" here
                        disconnectClient = 0; // reset counter

                        // send middle header
                        sprintf(buffer, "Content-Type: image/jpeg\r\n" \
                "Content-Length: %d\r\n" \
                "X-Timestamp: %d.%06d\r\n" \
                "\r\n", (int)params.data->buffer->size(), (int)1, (int)2);
                        if (send(sockFd,buffer,strlen(buffer), MSG_NOSIGNAL) < 0) {
                            disconnectClient++; // + 1 point to disconnect
                        }

                        // send image
                        if (send(sockFd,params.data->buffer->data(),params.data->buffer->size(),MSG_NOSIGNAL) < 0) {
                            disconnectClient++; // + 1 point to disconnect
                        };

                        // send middle header
                        sprintf(buffer, "\r\n--" BOUNDARY "\r\n");
                        if (send(sockFd,buffer,strlen(buffer),MSG_NOSIGNAL) < 0) {
                            disconnectClient++; // + 1 point to disconnect
                        };

                        // check disconnect client
                        if (disconnectClient == 3) {
                            disconnectSockList.insert(sockFd); // add to disconnect list
                        }
                    }
                }

                // remove disconnected element
                if (!disconnectSockList.empty()) {

                    // remove disconnected client socket from socket list
                    for (it = disconnectSockList.begin(); it != disconnectSockList.end(); ++it) {
                        int sockFd = *it;
                        close(sockFd); // close socket
                        params.socketList->erase(sockFd);
                    }

                    disconnectSockList.clear(); // clear list
                }
            }
            pthread_mutex_unlock(params.mutex_client); // unlock

            usleep(MJPG_STREAM_WAIT_INTERVAL); // sleep for 4 miliseconds
        } catch(...) {
            std::cerr << "exception on streamer thread" << std::endl;
            continue;
        }
    }
}

bool StreamerMJPG::Stop() {
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

        std::cout << "mjpg streaming stopped" << std::endl;
    } catch(...) {
        std::cerr << "error in StreamerMJPG::Stop()" << std::endl;
    }
}