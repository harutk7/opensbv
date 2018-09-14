//
// Created by harut on 7/23/18.
//

#include "opensbv/streamer/capture.h"

namespace opensbv {
    namespace streamer {

        Capture::Capture(opensbv::streamer::CaptureType type): mOpened(false) {
            mType = type;
            mStreamPort = 2341;

            if (mType == CAPTURE_MRTP) {
                mCapture = new CaptureMRTP();
            }
        }

        Capture::Capture(opensbv::streamer::CaptureType type, unsigned short port) {
            mType = type;
            mStreamPort = port;

            if (mType == CAPTURE_MRTP) {
                mCapture = new CaptureMRTP();
            }
        }

        Capture::Capture(opensbv::streamer::Capture &bj) {

        }

        long long int Capture::getTimestamp() {
            if (mCapture != nullptr) {
                mCapture->getTimestamp();
            }
        }

        void Capture::setHost(std::string host) {
            try {
               std::vector<std::string> vec = opensbv::helpers::general::GeneralHelper::explode(host, ':');

               if (vec.size() == 2) {
                   mHost = vec[0];
                   setPort((ushort)std::stoi(vec[1]));
               } else {
                   mHost = host;
               }
            } catch(std::exception &e) {
                mHost = host;
            }
        }

        void Capture::setPort(ushort port) {
            mPort = port;
        }

        Capture::~Capture() {
            stop();
            io_service.stop();
            m_s->close();
            delete m_s;
            delete mCapture;
        }

        bool Capture::isOpened() {
            return mOpened;
        }

        bool Capture::connect() {
            bool status = true;
            try {

                m_s = new tcp::socket(io_service);
                tcp::resolver resolver(io_service);
//                boost::asio::connect(*m_s, resolver.resolve({m_address, std::to_string(m_port)}));
                tcp::resolver::iterator iterator = resolver.resolve({mHost, std::to_string(mPort)});
                boost::system::error_code ec;
                m_s->connect(*iterator, ec);
                if (ec) {
                    status = false;
                } else {
                    std::cout << "connected to sbStreamer server " << mHost << ":" << mPort << std::endl;
                }

                std::cout << "streamer connected " << mHost << ":" << mPort << std::endl;
            } catch(boost::exception &e) {
                status = false;
                StreamerException("Capture::connect()", "boost::exception").log();
            } catch (boost::system::system_error const& e) {
                status = false;
                StreamerException("Capture::connect()", e.what()).log();
            } catch (...) {
                status = false;
                StreamerException("Capture::connect()", "unknown exception").log();
            }
            return status;
        }

        void Capture::run() {
            // Switch from tcp to udp and vv
            m_server = boost::thread(&Capture::runServer, mStreamPort, mCapture, &mOpened);

            std::string cmd = getRunCmd();
            size_t request_length = cmd.length();
            boost::asio::write(*m_s, boost::asio::buffer(cmd.c_str(), request_length));

            mOpened = true;
        }

        void Capture::stop() {
            m_server.interrupt();
            m_server.join();
            mOpened = false;

            std::cout << "streamer disconnected " << mHost << ":" << mPort << std::endl;
        }

        void Capture::disconnect() {

        }

        std::string Capture::getRunCmd() {
            boost::property_tree::ptree root;
            std::stringstream ss;
            root.put("action", "play");
            root.put("port", mStreamPort);
            boost::property_tree::write_json(ss, root);
            return ss.str();
        }

        std::vector<unsigned char> Capture::read() {

            if (mCapture != nullptr)
                return mCapture->getData();
            else
                return std::vector<unsigned char>();
        }

        cv::Mat Capture::readMat() {

            if (mCapture != nullptr) {

                std::vector<unsigned char> data = mCapture->getData();

                if (data.empty())
                    return cv::Mat();

                return cv::imdecode(cv::Mat(data), 1);
            }
            else
                return cv::Mat();
        }

        void Capture::runServer(unsigned short port, opensbv::streamer::AbstractCapture *capture, bool *opened) {
            try {
                TcpServer server(port);

                int run = server.run();
                if (run < 0)
                    exit(1);

                size_t bufferSize = 4096;
                size_t size;

                char buffer[bufferSize];
                char imageBuffer[4461819];
                bzero(buffer,bufferSize);

                long long int timestamp = 0;
                size_t fullSize = 0;

                while(true) {
                    size = server.recv(buffer, bufferSize);

                    if (size < 0) {
                        return;
                    }
                    if (size == 0)
                        continue;

                    if (size <= 15) {
                        if (fullSize != 0)
                            capture->onRecv(imageBuffer, fullSize, timestamp);
                        timestamp = atoll(buffer);
                        fullSize = 0;
                    } else {
                        std::copy(buffer + 0, buffer + size, imageBuffer + fullSize);
                        fullSize += size;
                    }

                    bzero(buffer,bufferSize);
                }

            } catch(boost::thread_interrupted const&e) {
                int b = 2;
            } catch(std::exception &e){
                int a = 1;
            }
            *opened = false;
        }

    }
}