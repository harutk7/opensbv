//
// Created by harut on 7/24/18.
//

#ifndef OPENSBV_UDPSERVER_H
#define OPENSBV_UDPSERVER_H

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "opensbv/streamer/abstractCapture.h"

using boost::asio::ip::udp;

namespace opensbv {
    namespace streamer {
        class client
        {
        public:
            client(const udp::endpoint& listen_endpoint)
                    : socket_(io_service_, listen_endpoint),
                      deadline_(io_service_)
            {
                // No deadline is required until the first socket operation is started. We
                // set the deadline to positive infinity so that the actor takes no action
                // until a specific deadline is set.
                deadline_.expires_at(boost::posix_time::pos_infin);

                // Start the persistent actor that checks for deadline expiry.
                check_deadline();
            }

            std::size_t receive(const boost::asio::mutable_buffer& buffer,
                                boost::posix_time::time_duration timeout, boost::system::error_code& ec)
            {
                // Set a deadline for the asynchronous operation.
                deadline_.expires_from_now(timeout);

                // Set up the variables that receive the result of the asynchronous
                // operation. The error code is set to would_block to signal that the
                // operation is incomplete. Asio guarantees that its asynchronous
                // operations will never fail with would_block, so any other value in
                // ec indicates completion.
                ec = boost::asio::error::would_block;
                std::size_t length = 0;

                // Start the asynchronous operation itself. The handle_receive function
                // used as a callback will update the ec and length variables.
                socket_.async_receive(boost::asio::buffer(buffer),
                                      boost::bind(&client::handle_receive, _1, _2, &ec, &length));

                // Block until the asynchronous operation has completed.
                do io_service_.run_one(); while (ec == boost::asio::error::would_block);

                return length;
            }

        private:
            void check_deadline()
            {
                // Check whether the deadline has passed. We compare the deadline against
                // the current time since a new asynchronous operation may have moved the
                // deadline before this actor had a chance to run.
                if (deadline_.expires_at() <= boost::asio::deadline_timer::traits_type::now())
                {
                    // The deadline has passed. The outstanding asynchronous operation needs
                    // to be cancelled so that the blocked receive() function will return.
                    //
                    // Please note that cancel() has portability issues on some versions of
                    // Microsoft Windows, and it may be necessary to use close() instead.
                    // Consult the documentation for cancel() for further information.
                    socket_.cancel();

                    // There is no longer an active deadline. The expiry is set to positive
                    // infinity so that the actor takes no action until a new deadline is set.
                    deadline_.expires_at(boost::posix_time::pos_infin);
                }

                // Put the actor back to sleep.
                deadline_.async_wait(boost::bind(&client::check_deadline, this));
            }

            static void handle_receive(
                    const boost::system::error_code& ec, std::size_t length,
                    boost::system::error_code* out_ec, std::size_t* out_length)
            {
                *out_ec = ec;
                *out_length = length;
            }

        private:
            boost::asio::io_service io_service_;
            udp::socket socket_;
            boost::asio::deadline_timer deadline_;
        };



        class UdpServer
        {
            enum { max_length = 65000 };

            AbstractCapture *mCapture = nullptr;
            udp::endpoint mListen_endpoint;
            client mClient;
        public:
            UdpServer(AbstractCapture *capture, std::string host, short port);
            ~UdpServer();

            void run();
        };
    }
}
#endif //OPENSBV_UDPSERVER_H
