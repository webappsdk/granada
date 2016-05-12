/**
  * Copyright (c) <2016> Web App SDK granada <afernandez@cookinapps.io>
  *
  * This source code is licensed under the MIT license.
  *
  * Permission is hereby granted, free of charge, to any person obtaining a copy
  * of this software and associated documentation files (the "Software"), to deal
  * in the Software without restriction, including without limitation the rights
  * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  * copies of the Software, and to permit persons to whom the Software is
  * furnished to do so, subject to the following conditions:
  *
  * The above copyright notice and this permission notice shall be included in
  * all copies or substantial portions of the Software.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  * SOFTWARE.
  *
  * Utils for time manipulation and measure.
  */

#pragma once

#include <string>
#include <sstream>

namespace granada{
  namespace util{
    namespace time{

      /**
       * Parse a string containing time into time_t
       * @param  time_str Stringified time
       * @return          Parsed time.
       */
      static std::time_t parse(const std::string& time_str){
        long int _time = 0;
        try{
          _time = std::stol(time_str);
        }catch(const std::logic_error& e){
          _time = 0;
        }
        return _time;
      }


      /**
       * Stringify time_t.
       * @param  _time Time to stringify.
       * @return       Stringified time.
       */
      static std::string stringify(const std::time_t& _time) {
        std::stringstream ss;
        ss << _time;
        return ss.str();
      }


      /**
       * Returns true if the difference in seconds between now and the given time is higher than
       * the given timeout seconds plus an extra seconds.
       * @param  _time         Time to compare with now
       * @param  timeout       Timeout, minimum difference in seconds now and given time should have to not
       *                       being considered timedout. If -1 is considered infinite+.
       * @param  extra_seconds Extra seconds to sum to timeout.
       * @return               True if timedout false if not.
       */
      static bool is_timedout(const std::time_t& _time, const long int& timeout, const long int& extra_seconds){
        if (timeout > -1){
          std::time_t now = std::time(nullptr);
          long int seconds = std::difftime(now,_time);
          if (seconds > timeout + extra_seconds){
            return true;
          }
        }
        return false;
      }


      /**
       * Returns true if the difference in seconds between now and the given time is higher than
       * the given timeout.
       * @param  _time         Time to compare with now
       * @param  timeout       Timeout, minimum difference in seconds now and given time should have to not
       *                       being considered timedout. If -1 is considered infinite+.
       * @return               True if timedout false if not.
       */
      static inline bool is_timedout(const std::time_t& _time, const long int& timeout){
        granada::util::time::is_timedout(_time,timeout,0);
      }
    }
  }
}
