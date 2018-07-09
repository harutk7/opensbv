//
// Created by harut on 8/2/17.
//

#include "opensbv/streamer/streamerMRTP.h"

namespace opensbv {
    namespace streamer {

        StreamerMRTP::StreamerMRTP(unsigned short port): m_port(port) {
            m_imageBuffer.buffersize = 0;
            m_imageBuffer.buffer = nullptr;
        }

        StreamerMRTP::~StreamerMRTP() {
            if (m_imageBuffer.buffersize != 0)
                tjFree(m_imageBuffer.buffer);
            m_imageBuffer.buffersize = 0;
            m_imageBuffer.buffer = nullptr;
        }

        void StreamerMRTP::Run() {
            try
            {
                boost::asio::io_service io_service;

                server(io_service, m_port);
            }
            catch (std::exception& e)
            {
                std::cerr << "Exception: " << e.what() << "\n";
            }
        }

        void StreamerMRTP::Stop() {

        }

        size_t StreamerMRTP::Write(unsigned char *data, ssize_t size) {

            if (m_imageBuffer.buffersize != 0)
                tjFree(m_imageBuffer.buffer);
            m_imageBuffer.buffersize = 0;
            m_imageBuffer.buffer = nullptr;

            if (ImageHelper::compress_jpg_turbo(data, mImageColorType, &m_imageBuffer, mWitdh, mHeight, mQuality)) {
                mBuffer.assign(m_imageBuffer.buffer+ 0, m_imageBuffer.buffersize, GeneralHelper::GetTimestamp());
            }
        }

        void StreamerMRTP::server(boost::asio::io_service& io_service, unsigned short port)
        {
            std::cout << "sbvStreamer running on port: " << port << std::endl;
            tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
            for (;;)
            {
                tcp::socket sock(io_service);
                a.accept(sock);
                std::thread(StreamerMRTP::session, std::move(sock), CmdParser(), &mBuffer).detach();
            }
        }

        void StreamerMRTP::session(tcp::socket sock, CmdParser cmdParser, StreamBuffer *buffer)
        {
            try
            {
                unsigned short curCliPort = sock.remote_endpoint().port();
                std::string curCliAddress = sock.remote_endpoint().address().to_string();
                std::cout << "client connected " << curCliAddress << ":" << curCliPort << std::endl;
                boost::thread streamThread;
                for (;;)
                {
                    char data[1024];

                    boost::system::error_code error;
                    size_t length = sock.read_some(boost::asio::buffer(data), error);
                    if (error == boost::asio::error::eof) {
                        streamThread.interrupt();
                        streamThread.join();
                        break; // Connection closed cleanly by peer.
                    }
                    else if (error) {
                        streamThread.interrupt();
                        streamThread.join();
                        throw boost::system::system_error(error); // Some other error.
                    }

                    cmdParser.parse(data, length);

                    switch(cmdParser.getAction()) {
                        case CMD_ACTION_STREAM_PLAY:
                            streamThread = boost::thread(&StreamerMRTP::streamWorker, buffer, streamMRTPParams{cmdParser.getPort(), sock.remote_endpoint().address().to_string()});
                            break;
                    }

                    boost::asio::write(sock, boost::asio::buffer(data, length));
                }
            }
            catch (std::exception& e)
            {
            }
            std::cout << "client disconnected" << std::endl;
        }

        void StreamerMRTP::streamWorker(StreamBuffer *buffer, streamMRTPParams params) {
            try {

                UdpClient client(buffer, params.address, params.port);
                client.run();
            } catch(boost::thread_interrupted const&) {

            } catch(...){

            }
        }


        CmdParser::CmdParser() {

        }

        void CmdParser::parse(std::string data, size_t n) {

            boost::property_tree::ptree root;

            try {
                std::stringstream ss;
                data = data.substr(0, data.size() - (data.length() - n));

                ss << data.c_str();

                boost::property_tree::ptree pt;
                boost::property_tree::read_json(ss, pt);

                std::string action = pt.get<std::string>("action");

                if (action == "play") {
                    m_port = pt.get<unsigned short>("port");
                    m_action = CMD_ACTION_STREAM_PLAY;
                }
            } catch(...) {

            }
        }

        unsigned short CmdParser::getPort() {
            return m_port;
        }

        cmd_action CmdParser::getAction() {
            return m_action;
        }


        UdpClient::UdpClient(StreamBuffer *buffer, std::string address, unsigned short port): m_address(address), m_port(port) {
            m_buffer = buffer;
        }

        void UdpClient::run() {
            boost::asio::io_service io_service;

            udp::socket s(io_service, udp::endpoint(udp::v4(), 0));

            udp::resolver resolver(io_service);
            udp::endpoint endpoint = *resolver.resolve({udp::v4(), m_address, std::to_string(m_port)});

            unsigned long prevTimestamp = 0;

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

                    s.send_to(boost::asio::buffer(mChunkSplitter.getNext(), mChunkSplitter.getNext().size()), endpoint);

                    full_size += mChunkSplitter.getNext().size();

                    mChunkSplitter.deleteNext();
                }

                usleep(STREAM_FPS);
            }
        }

        void UdpClient::stop() {

        }

        UdpClient::~UdpClient() {

        }


        ChunkSplitter::ChunkSplitter() {

        }

        ChunkSplitter::~ChunkSplitter() {

        }

        bool ChunkSplitter::hasNext() {
            return !mList.empty();
        }

        std::vector<unsigned char> ChunkSplitter::getNext() {
            return mList.front();
        }

        void ChunkSplitter::deleteNext() {
            mList.remove(mList.front());
        }

        void ChunkSplitter::split(unsigned char *buf, size_t n, unsigned long timeStamp) {

            mList.clear();

            size_t startPos = 0;
            size_t endPos = 0;

            size_t maxCount = n / 60000 + 1;

            std::string prefix = "-+-" + std::to_string(n) + "-" + std::to_string(maxCount) + "-";

            int currentCount = 0;
            std::string currentPrefix;

            while(true) {
                if (n > endPos) {

                    currentCount++;
                    currentPrefix = prefix + std::to_string(currentCount) + "-" + std::to_string(timeStamp) + "-";

                    startPos = endPos;

                    if (n - endPos > mChunkSize) {
                        endPos +=mChunkSize;
                    } else {
                        endPos = n;
                    }

                    std::vector<unsigned char> currentChunk;
                    currentChunk.assign(buf + startPos,  buf + endPos);
                    std::copy(currentPrefix.c_str() + 0, currentPrefix.c_str() + currentPrefix.length(), std::back_inserter(currentChunk));

                    mList.emplace_back(currentChunk);

                } else {
                    break;
                }
            }
        }

    }
}