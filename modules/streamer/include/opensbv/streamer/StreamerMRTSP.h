//
// Created by harut on 8/15/17.
//

#ifndef PB_MAIN_STREAMERMRTSP_H
#define PB_MAIN_STREAMERMRTSP_H

#define MRTSP_TCP_BUFFER_SIZE 1024 // buffer length for headers
#define MRTSP_TCP_PORT (5445);
#define MRTSP_UDP_PORT (5555);

#include <vector>
#include <cstring>
#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <set>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <memory>
#include <string>

#include "opensbv/helpers/images/ImageHelper.h" // image helper
#include "opensbv/helpers/general/GeneralHelper.h" // general helper
#include "opensbv/streamer/StreamerBase.h"
#include "opensbv/streamer/PracticalSocket.h"

/// RTSP Client Structure
struct streamerMRTSPClientStruct {
    int sockFd; // client tcp fd
    int failedToSend;
    long lastActive;
    bool ready;
    string clientIp; // clientip
    unsigned short clientPort; // client udp port
    udpClientWriterStruct udpClient;
};

inline bool operator==(const streamerMRTSPClientStruct& lhs, const streamerMRTSPClientStruct& rhs)
{
    return lhs.sockFd == rhs.sockFd;
}
inline bool operator<(const streamerMRTSPClientStruct& lhs, const streamerMRTSPClientStruct& rhs)
{
    return lhs.sockFd < rhs.sockFd;
}
inline bool operator>(const streamerMRTSPClientStruct& lhs, const streamerMRTSPClientStruct& rhs)
{
    return lhs.sockFd < rhs.sockFd;
}
/// Listen for mrtsp client worker struct
struct streamerMRTSPListenWorkerStruct {
    int *sockFd; ///< Server socked fd
    pthread_mutex_t *mutex_client; ///< client mutex
    std::vector<streamerMRTSPClientStruct> *clientList; ///< client list reference
};

/// MRTSP streamer worker struct
struct streamerMRTSPWorkerStruct {
    pthread_mutex_t *mutex_client; ///< Mutex for connecting clients
    pthread_mutex_t *mutex_stream; ///< Mutex for sending frames
    streamerDataStruct *data; ///< Data struct for streaming
    bool *isStreamReady; ///< Is stream ready or not
    std::vector<streamerMRTSPClientStruct> *clientList; ///< client list reference
    bool *m_running; ///< Streaming running or not
};

/// Client mrtsp command worker struct
struct streamerMRTSPListenCommandWorkerStruct {
    int sockFd; ///< Server socked fd
    pthread_mutex_t *mutex_client; ///< Mutex for sending frames
    streamerMRTSPClientStruct currentClient; ///< current client
    std::vector<streamerMRTSPClientStruct> *clientList; ///< client list reference
};

/// MRTSP Streaming class
/**
 * Stream MRTSP Video to clients
 */
class StreamerMRTSP: public StreamerBase {
public:
    StreamerMRTSP(); ///< Constructor
    ~StreamerMRTSP(); ///< Destructor

    /// Write data to stremaing
    /**
     * write data to stream buffer
     * @param data to stream
     * @param size
     */
    void Write(unsigned char *data, ssize_t size); // write to stream

    /// Run the MRTSP Server
    bool Run(); // run mrtsp server

    /// Stop MRTSP Server
    bool Stop();

    /// Set Stream data type
    /**
     * Stream data type
     * @param dataType
     */
    void SetStreamDataType(enum stream_data_type dataType);

    static void *listenWorker(void* ptr); ///< Listen mrtsp thread function
    static void *listenCommandWorker(void* ptr); ///< MRTSP client command handler
    static void *streamerWorker(void* ptr); ///< MRTSP streamer thread function

private:
    uint16_t                                m_tcpPort; ///< Stream server port
    int                                     m_sockFd; ///< Server socket fd
    bool                                    m_isStreamReady; ///< Is stream ready or not
    enum stream_data_type                   m_streamDataType; ///< Stream Data type
    struct                                  sockaddr_in m_serv_addr; ///< Server socket struct
    streamerMRTSPListenWorkerStruct         m_listenParams; ///< Listen params for thread
    streamerMRTSPWorkerStruct               m_streamerParams; ///< Streamer params for thread
    pthread_t                               m_clientListenT = 0, m_streamerListenT = 0; ///< Accept client and send stream threads
    pthread_mutex_t                         m_mutex_client = PTHREAD_MUTEX_INITIALIZER; ///< Mutex for control clients
    pthread_mutex_t                         m_mutex_stream = PTHREAD_MUTEX_INITIALIZER; ///< Mutex for control frames
    std::vector<unsigned char>              m_outBuf; ///< Encoded image vector
    std::vector<streamerMRTSPClientStruct>  m_udpClientList; ///< UDP Client list

    /// Bind TCP Socket
    bool BindTCP();
};


#endif //PB_MAIN_STREAMERMRTSP_H