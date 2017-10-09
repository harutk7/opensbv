//
// Created by harut on 8/15/17.
//

#ifndef PB_MAIN_STREAMERBASE_H
#define PB_MAIN_STREAMERBASE_H

#define JPEG_ENCODE_QUALITY (80)

#include <chrono>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <vector>
#include <signal.h>

#include "opensbv/helpers/images/ImageHelper.h" // image helper

#define UDP_STREAM_WAIT_INTERVAL (1000)
#define UDP_PACK_SIZE (62000)
#define UDP_PORT (5445)
#define UDP_PKG_HEADER ("--bgn--")
#define UDP_PKG_FOOTER ("--end--")

typedef unsigned short ushort;

enum stream_data_type {
    STREAM_DATA_IMAGE,
    STREAM_DATA_AUDIO,
    STREAM_DATA_BYTES
};

/// data buffer struct
struct buffer_data {
    unsigned char *buffer; ///< image buffer
    unsigned long buffersize; ///< image buffer size

};
/// UDP Client streaming struct
struct udpClientWriterStruct {
    int sockFd; ///< UDP client socket fd
    struct sockaddr_in their_addr; ///< Destination struct
};

/// Streaming Data struct
struct streamerDataStruct {
    std::vector<unsigned char> *buffer; ///< Frame to be send
    long *timestamp; ///< TimeStamp
};

/// Streamer Base Struct
/**
 * Base class for Streaming which has functions that use both streaming types
 */
class StreamerBase {
public:
    StreamerBase();
    virtual ~StreamerBase(); ///< Desturctor

    /// Write Mat to stream
    /**
     * This function must be overrided by inherited class
     * @param data
     * @param size
     */
    virtual void Write(unsigned char *data, ssize_t size);

    /// Stop Streaming
    virtual bool Stop();

    /// Stop Running
    virtual bool Run();

    /**
     * Get Height of frame
     * @return
     */
    unsigned short GetImageHeight();

    /**
     * Get Width of frame
     * @return
     */
    unsigned short GetImageWidth();

    /// Set width params
    /**
     * Set Width
     * @param width
     */
    void SetImageWidth(ushort width);

    /// Set width height
    /**
     * Set Height
     * @param height
     */
    void SetImageHeight(ushort height);

    /// set image quality
    /**
     *
     * @param quality
     */
    void SetImageQuality(ushort quality);

    /// get image colortype
    /**
     *
     * @param colorType
     */
    void SetImageColorType(imageColorType colorType);

    /// Set port
    /**
     *
     * @param port
     */
    void SetPort(ushort port);

    /**
     * Get UDP Client for Streaming
     * @param hostname
     * @param port
     * @param isBroadCast
     * @return
     */
    static udpClientWriterStruct GetUDPClient(char hostname[15], unsigned short port, bool isBroadCast);


protected:
    streamerDataStruct m_streamerData; ///< Streaming data;
    buffer_image m_imageBuffer; ///< Encoded Decoded image buffer struct
    buffer_data m_dataBuffer; ///< Data buffer struct
    ushort m_jpegQuality; ///< Jpeg Quality
    ushort m_imageWidth; ///< Frame width
    ushort m_imageHeight; ///< Frame height
    ushort m_streamerPort; ///< Streaming port
    imageColorType m_imageColorType; /// Image color type
    bool m_streamRunning = false; ///< Streaming running or not

private:

};


#endif //PB_MAIN_STREAMERBASE_H
