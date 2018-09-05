//
// Created by harut on 9/5/18.
//

#include "opensbv/streamer/tcpClient.h"


namespace opensbv {
    namespace streamer {

        TcpClient::TcpClient(StreamBuffer *buffer, std::string address, unsigned short port):
                m_address(address),
                m_port(port),
                mChunkSplitter(SIZE_MAX)
        {
            m_buffer = buffer;
        }

        void TcpClient::run() {

            boost::asio::io_service io_service;

            tcp::socket s(io_service);
            tcp::resolver resolver(io_service);
            boost::asio::connect(s, resolver.resolve({m_address, std::to_string(m_port)}));
            uint32_t prevTimestamp = 0;

            while(true) {

                boost::this_thread::interruption_point();

                if (m_buffer->getTimestamp() == prevTimestamp)
                    continue;
                else
                    prevTimestamp = m_buffer->getTimestamp();

                mLocalBuffer = *m_buffer;

                mChunkSplitter.split(mLocalBuffer.getData(), mLocalBuffer.getLength(), mLocalBuffer.getTimestamp());

                size_t full_size = 0;

                while (mChunkSplitter.hasNext()) {

                    boost::asio::write(s, boost::asio::buffer(mChunkSplitter.getNext(), mChunkSplitter.getNext().size()));

                    mChunkSplitter.deleteNext();
                }

                usleep(STREAM_FPS);
            }
        }

        void TcpClient::stop() {

        }

        TcpClient::~TcpClient() {

        }
    }
}