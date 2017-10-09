//
// Created by harut on 6/9/17.
//

#include "Config.h"

// Constructor
Config::Config(string config) {

    // read config file
    string line,text;
    ifstream in(config.c_str());

    while(getline(in, line)) {
        istringstream iss(line);

        if (!(iss >> this->m_streamer_port >> this->m_commands_port >> this->m_camera >> this->m_stream_quality >> this->m_serial_read >> this->m_serial_write)) {
            cerr << "error in config file" << endl;
            exit(1);
        }

        break;
    }
}

// Destructor
Config::~Config() {

}

// Get camera index
string Config::GetCamera(){
    return this->m_camera;
}

// Get Streaming port
uint16_t Config::GetStreamerPort() {
    return this->m_streamer_port;
}

unsigned short Config::GetStreamQuality() {
    return (unsigned short)stoi(this->m_stream_quality);
}

// Get commands server port
uint16_t Config::GetCommandsPort() {
    return this->m_commands_port;
}

// Get Serial Read
string Config::GetSerialRead() {
    return this->m_serial_read;
}

// Get Serial Write
string Config::GetSerialWrite() {
    return this->m_serial_write;
}

