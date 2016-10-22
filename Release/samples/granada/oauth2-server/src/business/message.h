/**
  * Copyright (c) <2016> granada <afernandez@cookinapps.io>
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
  * Message
  * Class for creating, editing and deleting users' messages.
  *
  */


#pragma once

#include <string>
#include <mutex>
#include <memory>
#include <sstream>
#include "granada/util/string.h"
#include "granada/util/time.h"
#include "granada/cache/cache_handler.h"
#include "cpprest/asyncrt_utils.h"

namespace granada{
  class Message{
    public:

      /**
       * Constructor
       */
      Message(std::shared_ptr<granada::cache::CacheHandler>& cache);

      /**
       * Creates a new message associated with a user with
       * given username.
       * @param username
       * @param message  Text of the message.
       */
      void Create(const std::string username, const std::string& message);


      /**
       * Edit a message with the given message id (or key)
       * associated with a user with the given username.
       * @param  username
       * @param  message_id Message unique id or key.
       * @param  message    New text of the message.
       * @return            True if message exists and is edited, false if it does not.
       */
      bool Edit(const std::string username, const std::string& message_id, const std::string& message);


      /**
       * Deletes message with the given message id (or key) and
       * related to a user with the given username.
       * @param username
       * @param message_id
       */
      void Delete(const std::string username, const std::string& message_id);


      /**
       * Returns a list of user's message in form of a stringified json array.
       * @param  username
       * @return          Stringified json array containing the user's message.
       */
      std::string List(const std::string username);

    private:

      /**
       * Cache driver for inserting, editing and deleting message data.
       */
      std::shared_ptr<granada::cache::CacheHandler> cache_;


      /**
       * Nonce string generator, for generating unique strings tokens.
       * From cpprest/asyncrt_utils.h
       * Generate a nonce string containing random alphanumeric characters (A-Za-z0-9).
       */
      std::unique_ptr<utility::nonce_generator> n_generator_;
  };
}
