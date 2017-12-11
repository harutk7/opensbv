//
// Created by harut on 8/2/17.
//

#ifndef PB_MAIN_STREAMER_H
#define PB_MAIN_STREAMER_H

// Headers to send to client for http protocol
#define STD_HEADER "Connection: close\r\n" \
    "Server: MJPG-Streamer/0.2\r\n" \
    "Cache-Control: no-store, no-cache, must-revalidate, pre-check=0, post-check=0, max-age=0\r\n" \
    "Pragma: no-cache\r\n" \
    "Expires: Mon, 7 May 2033 12:34:56 GMT\r\n"
#define BOUNDARY "boundarydonotcross"
#define BUFFER_SIZE 512 // buffer length for headers

#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <set>
#include <vector>
#include <memory>

#include "opensbv/helpers/images/ImageHelper.h" // image helper
#include "opensbv/streamer/StreamerRTSP.h" // RTSP Streaming
#include "opensbv/streamer/StreamerMJPG.h" // MJPG Streaming
#include "opensbv/streamer/StreamerUDP.h" // UDP Streaming
#include "opensbv/streamer/StreamerMRTSP.h" // MRTSP Streaming

/// Type of Streming
enum streaming_type {
    STREAM_MJPG,
    STREAM_RTSP,
    STREAM_UDP,
    STREAM_MRTSP,
};

/// Listen struct for worker Thread
struct streamerListenWorkerStruct {
    int *sockFd; ///< Server socked fd
    std::set<int> *socketList; ///< List of clients
    pthread_mutex_t *mutex_client; ///< Main mutex for storing frame and sending it
};

/// Listen struct for streamer Thread
struct streamerWorkerStruct {
    std::set<int> *socketList; ///< List of clients
    pthread_mutex_t *mutex_client; ///< Mutex for sending frames
    std::vector<unsigned char> *outBuf; ///< Frame to be send
    bool *isStreamReady; ///< Is stream ready or not
};

/// Streamer class
/**
 * Streaming frame to network class
 */
class Streamer {
public:
    /**
     * Streamer constructor
     * @param width
     * @param height
     */
    Streamer();
    ~Streamer(); ///< Destructor

    /// Write frame to stremaing
    /**
     * Converts to Jpeg and Streams
     * @param data to stream
     * @param size
     * @return
     */
    size_t Write(unsigned char *data, ssize_t size); // write to stream

    /// Set Streaming Type
    /**
     * Set streaming type and Stop streaming
     * @param stream_type
     */
    void SetStreamType(enum streaming_type stream_type);

    /// Set Stream data type
    /**
     * Stream data type
     * @param dataType
     */
    void SetStreamDataType(enum stream_data_type dataType);

    /// Set Image params
    /**
     * Set image params
     * @param width
     * @param height
     * @param jpegQuality
     */
    void SetImageParams(ushort width, ushort height, ushort jpegQuality);

    /// Set stream port
    /**
     *
     * @param port
     */
    void SetStreamPort(ushort port);

    /// Set udp Stream params
    /**
     *
     * @param ip
     * @param isBroadcast
     * @return
     */
    void SetUdpStreamParams(char ip[20], bool isBroadcast);

    /// set image steramer
    /**
     *
     * @param colorType
     */
    void SetImageColorType(imageColorType colorType);

    /// Stop Streaming
    void Stop();

    /// Run Streaming
    void Run();

private:
    bool m_streamingRunning = false; ///< Streaming running or not
    enum streaming_type m_stream_type = STREAM_MJPG; ///< Streaming Type
    StreamerMJPG m_streamerMJPG; ///< MJPG Streamer class
    StreamerRTSP m_streamerRTSP; ///< RTSP Streamer class
    StreamerUDP m_streamerUDP; ///< UDP Streamer class
    StreamerMRTSP m_streamerMRTSP; ///< MRTSP Streamer class

};

#endif //PB_MAIN_STREAMER_H