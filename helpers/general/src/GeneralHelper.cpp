//
// Created by harut on 8/11/17.
//

#include "opensbv/helpers/general/GeneralHelper.h"

namespace opensbv {

    namespace helpers {
        namespace general {

            std::string GeneralHelper::format_account_number(int acct_no) {
                char buffer[20];
                std::snprintf(buffer, sizeof(buffer), "%04d", abs(acct_no));
                return buffer;
            }

            std::vector<std::string> GeneralHelper::explode(std::string const & s, char delim)
            {
                std::vector<std::string> result;
                std::istringstream iss(s);

                for (std::string token; std::getline(iss, token, delim); )
                {
                    result.push_back(std::move(token));
                }

                return result;
            }

            unsigned long GeneralHelper::GetTimestamp() {

                return duration_cast< milliseconds >(
                        system_clock::now().time_since_epoch()
                ).count();
            }

            std::string GeneralHelper::GetDateTime() {
                time_t     now = time(0);
                struct tm  tstruct;
                char       buf[80];
                tstruct = *localtime(&now);
                // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
                // for more information about date/time format
                strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

                return buf;
            }
        }
    }
}