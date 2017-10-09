//
// Created by harut on 8/4/17.
//

#ifndef PB_MAIN_CONFIG_H
#define PB_MAIN_CONFIG_H

#define VIDEO_FPS (30)
//#define FRAME_INTERVAL (1000/VIDEO_FPS)
#define FRAME_INTERVAL (1)
#define FRAME_WIDTH (640)
#define FRAME_HEIGHT (480)
#define ENCODE_QUALITY 50
#define TRACK_WAIT_INTERVAL (VIDEO_FPS/6) // sync interval for tracking

#include <iostream>
#include <vector>
#include <sstream>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>

using namespace std;

/// Configuration class
/**
 * Main Class for configuration of program
 */
class Config {
public:
    Config(string config); ///< Constructor
    ~Config(); ///< Destructor

    /// Get Camera string for capture
    string GetCamera();

    /// Get Streaming port
    uint16_t GetStreamerPort();

    /// Get Command server port
    uint16_t GetCommandsPort();

    /// Get Stream quality
    unsigned short GetStreamQuality();

    /// Get Serial Read file
    string GetSerialRead();

    /// Get Serial Write file
    string GetSerialWrite();

private:
    string m_camera; ///< Camera index
    uint16_t m_streamer_port; ///< Streaming server port
    uint16_t m_commands_port; ///< Commands server port
    string m_serial_read, m_serial_write, m_stream_quality; ///< Serial read write
};

#endif //PB_MAIN_CONFIG_H