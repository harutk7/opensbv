//
// Created by harut on 6/8/17.
//

#include "opensbv/serial/Serialrw.h"

namespace opensbv {
    namespace serial {

        SerialRW::SerialRW(char *read, char *write, long long int baundRate) {
            m_baundRate = baundRate; // set m_baundRate

            this->read = read; // read
            this->write = write; // write
            try {
                // init serial class
                m_serial = new Serial(string(this->read),this->m_baundRate,Timeout::simpleTimeout(1000));
                m_serial->setTimeout(serial::Timeout::max(), 1000, 0, 1000, 0);
            } catch (...) {
                m_serial = nullptr;
                cerr << "something wrong while init serial" << endl;
            }
        }

        SerialRW::~SerialRW() {
            delete m_serial; // delete serial lib pointer
        }

        ssize_t SerialRW::Write(string data) {
            if (m_serial != nullptr)
                return m_serial->write(data); // write

            cerr << "serial is not inited" << std::endl;
            return 0;
        }

        string SerialRW::Read(size_t size) {
            if (m_serial != nullptr)
                return m_serial->read(size);

            cerr << "serial is not inited" << std::endl;
            return "";
        }

        void SerialRW::SetBoundRate(long long int rate) {
            this->m_baundRate = rate;
        }
    }
}
