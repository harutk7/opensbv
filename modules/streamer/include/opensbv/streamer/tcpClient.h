//
// Created by harut on 9/5/18.
//

#ifndef OPENSBV_TCPCLIENT_H
#define OPENSBV_TCPCLIENT_H


#include <boost/thread.hpp>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

#include "opensbv/streamer/streamBuffer.h"
#include "opensbv/streamer/chunkSplitter.h"

#define STREAM_FPS (5000)

using boost::asio::ip::tcp;

namespace opensbv {
    namespace streamer {

        /// Tcp Client
        class TcpClient {

            ChunkSplitter mChunkSplitter; ///< chunk splitter
            StreamBuffer mLocalBuffer; ///< local buffer

            StreamBuffer* m_buffer; ///< buffer
            std::string m_address; ///< address
            unsigned short m_port; ///< port

        public:
            /**
             * Constructor
             * @param buffer
             * @param address
             * @param port
             */
            TcpClient(StreamBuffer *buffer, std::string address, unsigned short port);

            /**
             * Desturctor
             */
            ~TcpClient();

            /**
             * Run
             */
            void run();

            /**
             * Stop
             */
            void stop();
        };

    }
}


#endif //OPENSBV_TCPCLIENT_H
