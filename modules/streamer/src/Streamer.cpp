//
// Created by harut on 8/2/17.
//

#include "opensbv/streamer/Streamer.h"

Streamer::Streamer():
        m_streamerMJPG(),
        m_streamerRTSP(),
        m_streamerUDP(),
        m_streamerMRTSP() {
}

Streamer::~Streamer() {

}

size_t Streamer::Write(unsigned char *data, ssize_t size = 0) {
    if (m_stream_type == STREAM_MJPG){
        return m_streamerMJPG.Write(data,size); // call mjpeg write
    } else if (m_stream_type == STREAM_RTSP) {
        return m_streamerRTSP.Write(data,size);
    } else if (m_stream_type == STREAM_UDP) {
        return m_streamerUDP.Write(data,size);
    } else if (m_stream_type == STREAM_MRTSP) {
        return m_streamerMRTSP.Write(data,size);
    }
}

void Streamer::Stop() {

    // Stop Streaming

    if (m_stream_type == STREAM_MJPG){
        m_streamerMJPG.Stop();
    } else if (m_stream_type == STREAM_RTSP) {
        m_streamerRTSP.Stop();
    } else if (m_stream_type == STREAM_UDP) {
        m_streamerUDP.Stop();
    } else if (m_stream_type == STREAM_MRTSP) {
        m_streamerMRTSP.Stop();
    }

    this->m_streamingRunning = false;
}

void Streamer::Run() {
    if (!this->m_streamingRunning) {
        if (m_stream_type == STREAM_MJPG) {
            m_streamerMJPG.Run(); // Stop Streaming
        } else if (m_stream_type == STREAM_RTSP) {
            m_streamerRTSP.Run(); // Stop Streaming
        } else if (m_stream_type == STREAM_UDP) {
            m_streamerUDP.Run(); // Stop Streaming
        } else if (m_stream_type == STREAM_MRTSP) {
            m_streamerMRTSP.Run(); // Stop Streaming
        }
    }

    m_streamingRunning = true;
}

void Streamer::SetStreamType(enum streaming_type stream_type) {
    this->m_stream_type = stream_type; // change type
}

void Streamer::SetStreamDataType(enum stream_data_type dataType) {
        m_streamerMRTSP.SetStreamDataType(dataType);
        m_streamerRTSP.SetStreamDataType(dataType);
        m_streamerUDP.SetStreamDataType(dataType);
}

void Streamer::SetImageParams(ushort width, ushort height, ushort jpegQuality) {
        m_streamerMJPG.SetImageWidth(width);
        m_streamerMJPG.SetImageHeight(height);
        m_streamerMJPG.SetImageQuality(jpegQuality);

        m_streamerMRTSP.SetImageWidth(width);
        m_streamerMRTSP.SetImageHeight(height);
        m_streamerMRTSP.SetImageQuality(jpegQuality);

        m_streamerRTSP.SetImageWidth(width);
        m_streamerRTSP.SetImageHeight(height);
        m_streamerRTSP.SetImageQuality(jpegQuality);

        m_streamerUDP.SetImageWidth(width);
        m_streamerUDP.SetImageHeight(height);
        m_streamerUDP.SetImageQuality(jpegQuality);
}

void Streamer::SetStreamPort(ushort port) {
    m_streamerMJPG.SetPort(port);
    m_streamerMRTSP.SetPort(port);
    m_streamerRTSP.SetPort(port);
    m_streamerUDP.SetPort(port);
}

void Streamer::SetUdpStreamParams(char ip[20], bool isBroadcast) {
    m_streamerUDP.SetUdpStreamParams(ip, isBroadcast);
}

void Streamer::SetImageColorType(imageColorType colorType) {
    m_streamerMJPG.SetImageColorType(colorType);
    m_streamerMRTSP.SetImageColorType(colorType);
    m_streamerRTSP.SetImageColorType(colorType);
    m_streamerUDP.SetImageColorType(colorType);
}
