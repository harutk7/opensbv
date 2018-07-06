//
// Created by harut on 7/6/18.
//

#include "opensbv/streamer/streamer.h"

namespace opensbv {
    namespace streamer {

        Streamer::Streamer() {

        }

        Streamer::~Streamer() {

        }

        void Streamer::run() {
            mWorkingThread = boost::thread(&streamerWorkingThread, mStreamer);
        }

        void Streamer::stop() {
            mWorkingThread.interrupt();
            mWorkingThread.join();
        }

        void Streamer::setStreamer(StreamerBase *streamer) {
                mStreamer = streamer;
        }

        void streamerWorkingThread(StreamerBase *streamer) {
            try {
                streamer->Run();
            } catch(boost::thread_interrupted const&) {

            } catch(...){

            }
        }
    }
}

