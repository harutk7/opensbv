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
                static long GetTimestamp();

                /// Get current datetime
                static std::string GetDateTime();
            private:

            };
        }
    }
}

#endif //PB_MAIN_GENERALHELPER_H