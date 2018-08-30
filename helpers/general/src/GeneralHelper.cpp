//
// Created by harut on 8/11/17.
//

#include "opensbv/helpers/general/GeneralHelper.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_zone_base.hpp>

#include <boost/date_time/local_time/local_time.hpp>

#include <boost/date_time/gregorian/gregorian.hpp>
using namespace boost::gregorian;
using namespace boost::local_time;
using namespace boost::posix_time;

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

            uint32_t GeneralHelper::GetTimestamp() {
                boost::posix_time::ptime time_t_epoch(date(1970,1,1));
                boost::posix_time::ptime now = microsec_clock::local_time();
                boost::posix_time::time_duration diff = now - time_t_epoch;
                return diff.total_milliseconds();
//                return duration_cast< milliseconds >(
//                        system_clock::now().time_since_epoch()
//                ).count();
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