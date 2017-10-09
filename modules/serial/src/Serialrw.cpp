//
// Created by harut on 6/8/17.
//

#include "opensbv/serial/Serialrw.h"

SerialRW::SerialRW(string read, string write, uint32_t baundRate) {
    m_baundRate = baundRate; // set m_baundRate

    this->read = read; // read
    this->write = write; // write
    try {
        // init serial class
        m_serial = new Serial(string(this->write),this->m_baundRate,Timeout::simpleTimeout(1000));
    } catch (...) {
        m_serial = nullptr;
        cerr << "something wrong while init serial" << endl;
    }
}

SerialRW::~SerialRW() {
    delete m_serial; // delete serial lib pointer
}

ssize_t SerialRW::Write(string data) {
    return m_serial->write(data); // write
}

string SerialRW::Read(size_t size) {
    return m_serial->read(size);
}

void SerialRW::SetBoundRate(uint32_t rate) {
    this->m_baundRate = rate;
}