//
// Created by harut on 7/23/18.
//

#include "opensbv/streamer/capture.h"

namespace opensbv {
    namespace streamer {

        Capture::Capture(opensbv::streamer::CaptureType type) {
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
            delete m_s;
            delete mCapture;
        }

        void Capture::connect() {
            try {
                boost::asio::io_service io_service;

                m_s = new tcp::socket(io_service);
                tcp::resolver resolver(io_service);
//                boost::asio::connect(*m_s, resolver.resolve({m_address, std::to_string(m_port)}));
                tcp::resolver::iterator iterator = resolver.resolve({mHost, std::to_string(mPort)});
                m_s->connect(*iterator);
                std::cout << "connected to sbStreamer server " << mHost << ":" << mPort << std::endl;
            } catch(boost::exception &e) {
                throw StreamerException("Capture::connect()", "boost::exception");
            } catch (boost::system::system_error const& e) {
                throw StreamerException("Capture::connect()", e.what());
            } catch (...) {
                throw StreamerException("Capture::connect()", "unknown exception");
            }
        }

        void Capture::run() {
            m_udpServer = boost::thread(&Capture::runUdpServer, mUdpPort, mCapture);

            std::string cmd = getRunCmd();
            size_t request_length = cmd.length();
            boost::asio::write(*m_s, boost::asio::buffer(cmd.c_str(), request_length));
        }

        void Capture::stop() {
            m_udpServer.interrupt();
            m_udpServer.join();
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

        std::vector<unsigned char> Capture::getData() {

            if (mCapture != nullptr)
                return mCapture->getData();
            else
                return std::vector<unsigned char>();
        }

        void Capture::runUdpServer(unsigned short port, opensbv::streamer::AbstractCapture *capture) {
            try {
                boost::asio::io_service io_service;

                UdpServer s(io_service, port);

                s.setCapture(capture);

                s.do_receive();

            } catch(boost::thread_interrupted const&e) {
                throw StreamerException("Capture::runUdpServer()", "thread interrupted");
            } catch(...){

            }
        }

    }
}