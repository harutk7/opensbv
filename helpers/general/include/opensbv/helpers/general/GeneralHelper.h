//
// Created by harut on 8/11/17.
//

#ifndef PB_MAIN_GENERALHELPER_H
#define PB_MAIN_GENERALHELPER_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream> // for std::istringstream
#include <chrono>
#include <exception>
#include <iomanip>
#include <ctime>
#include <cstdint>

using namespace std::chrono;

namespace opensbv {
    namespace helpers {
        namespace general {

            /// General Helper class
            /**
              Here are function to manipulate with various types of data
            */
            class GeneralHelper {

            public:

                /// Format account number
                /**
                  Format string and add zeros from start
                */
                static std::string format_account_number(int acct_no);

                /// explode function for string
                static std::vector<std::string> explode(std::string const & s, char delim);

                /// Get current timestamp
                static long long int GetTimestamp();

                /// Get current datetime
                static std::string GetDateTime();


            };

        class GeneralException: public std::exception {

            protected:
                std::string mMsg, mRef, mFname, mFullMessage;

            public:

                explicit GeneralException(const char *ref, const char *fName, const char *message, bool out = true) {

                    mMsg = std::string(message);
                    mRef = std::string(ref);
                    mFname = std::string(fName);

                    mFullMessage = mRef + "(" + mFname + "): " + mMsg;
                }

                const char* what() {
                    return mFullMessage.c_str();
                }

                void log() {
                    time_t now = time(0);
                    tm *ltm = localtime(&now);
                    std::cerr << mFullMessage << ": " << ltm->tm_mday << "/" << 1 + ltm->tm_mon << "/" <<
                              1970 + ltm->tm_year << " " << 1 + ltm->tm_hour << ":" << 1 + ltm->tm_min << ":" <<
                              1 + ltm->tm_sec << std::endl;
                }
            };
        }
    }
}

#endif //PB_MAIN_GENERALHELPER_H