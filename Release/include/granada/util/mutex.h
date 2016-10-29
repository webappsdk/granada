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
  */

#pragma once
#include <memory>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "granada/util/memory.h"

namespace granada {
  namespace util {
    namespace mutex{
      class call_once{
        public:
		  call_once(){};

          void call(std::function<void(void)> fn){
            std::call_once(of_, [&]{
              {
				if (mtx_ == nullptr){
					mtx_ = granada::util::memory::make_unique<std::mutex>();
				}
                std::lock_guard<std::mutex> lg(*mtx_);
                fn();
                fn_called_ = true;
              }
			  if (cv_ == nullptr){
				cv_ = granada::util::memory::make_unique<std::condition_variable>();
			  }
              cv_->notify_all();
            });

            // wait until properties are loaded.
            std::unique_lock<std::mutex> ul(*mtx_);
            cv_->wait(ul, [this]{ return fn_called_; });
          };

        private:

		 /**
		  * Once flag.
		  */
		  std::once_flag of_;


          /**
           * Mutex.
           */
          std::unique_ptr<std::mutex> mtx_;


          /**
           * True if function has finished its execution, false if not.
           */
          bool fn_called_ = false;


          /**
           * Used to block all threads until function is
           * executed.
           */
		  std::unique_ptr<std::condition_variable> cv_;
      };
    }
  }
}
