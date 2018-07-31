//
// Created by harut on 7/23/18.
//

#include "opensbv/streamer/capture.h"

namespace opensbv {
    namespace streamer {

        Capture::Capture(opensbv::streamer::CaptureType type): mOpened(false) {
            mType = type;
            mUdpPort = 2341;

            if (mType == CAPTURE_MRTP) {
                mCapture = new CaptureMRTP();
            }
        }

        Capture::Capture(opensbv::streamer::CaptureType type, unsigned short udpPort) {
            mType = type;
            mUdpPort = udpPort;

            if (mType == CAPTURE_MRTP) {
                mCapture = new CaptureMRTP();
            }
        }

        void Capture::setHost(std::string host) {
            mHost = host;
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
            m_udpServer = boost::thread(&Capture::runUdpServer, mUdpPort, mCapture, &mOpened);

            std::string cmd = getRunCmd();
            size_t request_length = cmd.length();
            boost::asio::write(*m_s, boost::asio::buffer(cmd.c_str(), request_length));

            mOpened = true;
        }

        void Capture::stop() {
            m_udpServer.interrupt();
            m_udpServer.join();
            mOpened = false;

            std::cout << "streamer disconnected " << mHost << ":" << mPort << std::endl;
        }

        void Capture::disconnect() {

        }

        std::string Capture::getRunCmd() {
            boost::property_tree::ptree root;
            std::stringstream ss;
            root.put("action", "play");
            root.put("port", mUdpPort);
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

        void Capture::runUdpServer(unsigned short port, opensbv::streamer::AbstractCapture *capture, bool *opened) {
            try {
                UdpServer s(capture, "0.0.0.0", port);

                s.run();

            } catch(boost::thread_interrupted const&e) {
                int b = 2;
            } catch(std::exception &e){
                int a = 1;
            }
            *opened = false;
        }

    }
}