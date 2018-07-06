//
// Created by harut on 6/8/17.
//

#ifndef PB_MAIN_SERIALRW_H
#define PB_MAIN_SERIALRW_H

#include <iostream>
#include <string>
#include <serial/serial.h>

using namespace std;
using namespace serial;

namespace opensbv {
    namespace serial {

        /// Serial Read Write class
        /**
         * Read and Write to serial port
         */
        class SerialRW {
        public:
            SerialRW(char *read, char *write, uint32_t baundRate); ///< Constructor
            ~SerialRW(); ///< Destructor

            /// Write data to serial
            ssize_t Write(string data);

            /// Read from serial
            string Read(size_t size);

            /// Set bound rate
            void SetBoundRate(uint32_t rate);

        private:
            char* read; ///< Read fd
            char * write; ///< Write fd
            Serial *m_serial; ///< Serial lib pointer
            uint32_t m_baundRate; ///< Baudrate

        };

    }
}

#endif //PB_MAIN_SERIALRW_H
