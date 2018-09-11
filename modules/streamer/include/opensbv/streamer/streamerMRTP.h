//
// Created by harut on 8/2/17.
//

#ifndef PB_MAIN_STREAMER_H
#define PB_MAIN_STREAMER_H

#include <vector>
#include <list>
#include <cstddef>
#include <thread>

#include "opensbv/streamer/streamerBase.h"
#include "opensbv/helpers/image/ImageHelper.h"
#include "opensbv/helpers/general/GeneralHelper.h"

#include <boost/asio.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/thread.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "opensbv/streamer/chunkSplitter.h"


#define STREAM_FPS (5000)

using boost::asio::ip::udp;

using boost::asio::ip::tcp;

using namespace opensbv::helpers::image;
using namespace opensbv::helpers::general;

namespace opensbv {
    namespace streamer {

        class StreamerMRTPException: public opensbv::helpers::general::GeneralException {
        public:
            explicit StreamerMRTPException(const char *fName, const char* msg): GeneralException("StreamerMRTP", fName, msg) {

            }
        };


        /// mrtspStreamParams
        struct streamMRTPParams {
            unsigned short port;
            std::string address;
        };


        /// cmd actions
        enum cmd_action {
            CMD_ACTION_STREAM_PLAY
        };

        /// cmd parser
        class CmdParser {
            cmd_action m_action;

            unsigned short m_port;

        public:
            CmdParser();

            void parse(std::string data, size_t n);

            unsigned short getPort();

            cmd_action getAction();
        };

        class UdpClient {

            ChunkSplitter mChunkSplitter;
            StreamBuffer mLocalBuffer;

            StreamBuffer* m_buffer;
            std::string m_address;
            unsigned short m_port;

        public:
            UdpClient(StreamBuffer *buffer, std::string address, unsigned short port);

            ~UdpClient();

            void run();
            void stop();

        };


        /// StreamerMRTP class
        /**
         * StreamerMRTP frame to network class
         */
        class StreamerMRTP: public StreamerBase {

            boost::thread mStreamThread; ///< streaming thread

            unsigned short m_port;

            buffer_image m_imageBuffer;

            void server(boost::asio::io_service &io_service, unsigned short port);

        public:
            /**
             * Streamer constructor
             */
            StreamerMRTP(unsigned short port = 8554);
            ~StreamerMRTP(); ///< Destructor

            /// Write frame to stremaing
            /**
             * Converts to Jpeg and Streams
             * @param data to stream
             * @param size
             * @return
             */
            size_t Write(unsigned char *data, ssize_t size); // write to stream

            /**
             * Wrute mjpg
             * @param data
             * @param size
             */
            void WriteJPG(unsigned char *data, ssize_t size);

            /// Run Streaming
            void Run();

            /// Stop Streaming
            void Stop();

            static void session(tcp::socket sock, CmdParser cmdParser, StreamBuffer *buffer);

            static void streamWorker(StreamBuffer *buffer, streamMRTPParams params);

        };
    }
}

#endif //PB_MAIN_STREAMER_H