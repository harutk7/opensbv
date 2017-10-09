//
// Created by harut on 8/15/17.
//

#ifndef PB_MAIN_STREAMERMJPG_H
#define PB_MAIN_STREAMERMJPG_H

// Headers to send to client for http protocol
#define STD_HEADER "Connection: close\r\n" \
    "Server: MJPG-Streamer/0.2\r\n" \
    "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n" \
    "Pragma: no-cache\r\n" \
    "Expires: Mon, 7 May 2033 12:34:56 GMT\r\n"
#define BOUNDARY "boundarydonotcross"
#define BUFFER_SIZE 512 // buffer length for headers
#define MJPG_PORT (7777) // port of tcp server
#define MJPG_STREAM_WAIT_INTERVAL (1000)

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <set>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>

#include "opensbv/helpers/images/ImageHelper.h" // image helper
#include "opensbv/streamer/StreamerBase.h"

/// Streaming MJPG Listen worker struct
struct streamerMJPGListenWorkerStruct {
    int *sockFd; ///< Server socked fd
    std::set<int> *socketList; ///< List of clients
    pthread_mutex_t *mutex_client; ///< Main mutex for storing frame and sending it
    bool *m_running; ///< Streamer running or not
};

/// Streaming MJPG worker struct
struct streamerMJPGWorkerStruct {
    std::set<int> *socketList; ///< List of clients
    pthread_mutex_t *mutex_client; ///< Mutex for sending frames
    streamerDataStruct *data; ///< Data struct for streaming
    bool *isStreamReady; ///< Is stream ready or not
    bool *m_running; ///< Streamer running or not
};

/// Streamer MJPG class
/**
 * Stream Http MJPG
 */
class StreamerMJPG: public StreamerBase {
public:
    StreamerMJPG(); ///< Cosntructor
    ~StreamerMJPG(); ///< Destructor

    /// Run Streaming
    /**
     * Be sure to Stop Streaming before Running it
     * @return Streaming running or not
     */
    bool Run();

    /// Stops Streaming
    /**
     * Stops streaming
     * @return Straeming stoped or not
     */
    bool Stop();

    /// Write data to stremaing
    /**
     * write data to stream buffer
     * @param data to stream
     * @param size
     */
    void Write(unsigned char *data, ssize_t size); // write to stream

private:
    uint16_t m_port; ///< Stream server port
    pthread_mutex_t m_mutex_client = PTHREAD_MUTEX_INITIALIZER; ///< Mutex for control frames
    pthread_t m_clientListenT, m_streamerListenT; ///< Accept client and send stream threads
    cpu_set_t m_cpuSetListenWorker, m_cpuSetStreamerWorker; ///< Cpu set for affinity set
    int m_sockFd; ///< Server socket fd
    bool m_isStreamReady; ///< Is stream ready or not
    struct sockaddr_in m_serv_addr; ///< Server socket struct
    std::set<int> m_socketList; ///< Client list
    streamerMJPGListenWorkerStruct m_listenParams; ///< Listen params for thread
    streamerMJPGWorkerStruct m_streamerParams; ///< Streamer params for thread

    /// Bind TCP Socket
    bool BindTCP();

    static void *listenWorker(void* ptr); ///< Listen mjpg client thread function
    static void *streamerWorker(void* ptr); ///< mjpg streamer thread function

};


#endif //PB_MAIN_STREAMERMJPG_H