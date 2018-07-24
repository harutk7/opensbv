//
// Created by harut on 7/6/18.
//

#ifndef OPENSBV_STREAMER_CAPTURE_H
#define OPENSBV_STREAMER_CAPTURE_H

#include <string>
#include "opensbv/streamer/captureMRTP.h"


#include <cstdlib>
#include <cstring>
#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "opensbv/streamer/udpServer.h"
#include "opensbv/helpers/general/GeneralHelper.h"

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

const int tcp_recv_max_lenght = 1024;

typedef unsigned short ushort;

namespace opensbv {
    namespace streamer {

        /// streamer exception

        class StreamerException: public opensbv::helpers::general::GeneralException {
            public:
                explicit StreamerException(const char *fName, const char* msg): GeneralException("ImageHelper", fName, msg) {

                }
        };

        /// Capture stream type
        enum CaptureType {
            CAPTURE_MRTP
        };

        /// Capture Class
        class Capture {

            std::string mHost; ///< hostname
            ushort mPort, mUdpPort; ///< port
            tcp::socket *m_s;

            boost::thread m_udpServer;

            CaptureType mType; ///< type

            AbstractCapture *mCapture = nullptr; ///< Capture for getting data

            /**
             * get run command
             * @return
             */
            std::string getRunCmd();

        public:

            /**
             * constructor
             * @param type
             */
            explicit Capture(CaptureType type);

            /**
             * constructor
             */
            explicit Capture(CaptureType type, unsigned short udpPort);

            /**
             * destructor
             */
            ~Capture();

            /**
             * set hostname
             * @param host
             */
            void setHost(std::string host);

            /**
             * set port
             * @param port
             */
            void setPort(ushort port);

            /**
             * connect
             */
            void connect();

            /**
             * disconnect
             */
            void disconnect();

            /**
             * run
             */
            void run();

            /**
             * stop
             */
            void stop();

            /**
             * get data
             * @return
             */
            std::vector<unsigned char> getData();

            /**
             * run udp server worker function
             * @param port
             * @param capture
             */
            static void runUdpServer(unsigned short port, AbstractCapture* capture);

        };
    }
}

#endif //OPENSBV_STREAMER_CAPTURE_H