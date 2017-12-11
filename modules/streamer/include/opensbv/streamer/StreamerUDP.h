//
// Created by harut on 8/22/17.
//

#ifndef PB_MAIN_STREAMERUDP_H
#define PB_MAIN_STREAMERUDP_H

#include <vector>
#include <cstring>

#include "opensbv/helpers/images/ImageHelper.h" // image helper
#include "opensbv/streamer/StreamerBase.h"

/// Streaming UDP worker struct
struct streamerUDPWorkerStruct {
    pthread_mutex_t *mutex_stream; ///< Mutex for sending frames
    bool *isStreamReady; ///< Is stream ready or not
    streamerDataStruct *data; ///< Data struct for streaming
    char serverAddress[15]; ///< Server address to send to
    unsigned short serverPort; ///< Server port to send to
    bool *m_running; ///< Streaming running or not
};

/// Streamer UDP class
/**
 * Stream image to UDP Server
 */
class StreamerUDP: public StreamerBase {
public:
    StreamerUDP(); ///< Constructor
    ~StreamerUDP(); ///< Destructor

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

    /// Set Stream data type
    /**
     * Stream data type
     * @param dataType
     */
    void SetStreamDataType(enum stream_data_type dataType);

    /// Set Udp Stream Params
    /**
     *
     * @param ip
     * @param isBroadcast
     */
    void SetUdpStreamParams(char ip[20], bool isBroadcast);

    /// Write data to stremaing
    /**
     * write data to stream buffer
     * @param data to stream
     * @param size
     * @return
     */
    size_t Write(unsigned char *data, ssize_t size); // write to stream

private:
    bool m_isStreamReady; ///< Is stream ready or not
    pthread_mutex_t m_mutex_stream = PTHREAD_MUTEX_INITIALIZER; ///< Mutex for control frames
    streamerUDPWorkerStruct m_streamerParams; ///< Streamer params for thread
    enum stream_data_type                   m_streamDataType; ///< Stream Data type
    cpu_set_t m_cpuSetStreamerWorker; ///< Cpu set for affinity set
    pthread_t m_streamerListenT; ///< Accept client and send stream threads
    char m_serverAddress[15]; ///< Server Address to send to

    static void *streamerWorker(void* ptr); ///< mjpg streamer thread function
};


#endif //PB_MAIN_STREAMERUDP_H