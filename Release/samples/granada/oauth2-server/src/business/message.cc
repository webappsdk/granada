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


#include "message.h"

namespace granada{

  Message::Message(std::shared_ptr<granada::cache::CacheHandler>& cache){
    cache_ = cache;
    n_generator_ = std::unique_ptr<utility::nonce_generator>(new utility::nonce_generator(16));
  }

  void Message::Create(const std::string username, const std::string& message){
    std::string message_id = n_generator_->generate();
    if (cache_->Exists("message:" + username + ":" + message_id)){
      Create(username,message);
    }else{
      cache_->Write("message:" + username + ":" + message_id, "key", message_id);
      cache_->Write("message:" + username + ":" + message_id, "text", message);
    }
  }

  bool Message::Edit(const std::string username, const std::string& message_id, const std::string& message){
    if (cache_->Exists("message:" + username + ":" + message_id)){
      cache_->Write("message:" + username + ":" + message_id, "text", message);
      return true;
    }
    return false;
  }

  void Message::Delete(const std::string username, const std::string& message_id){
    cache_->Destroy("message:" + username + ":" + message_id);
  }

  std::string Message::List(const std::string username){
    std::string message_list = "";
    std::vector<std::string> keys;
    cache_->Match("message:" + username + ":*", keys);
    for (auto it = keys.begin(); it != keys.end(); ++it){
      if (it != keys.begin()){
        message_list += ",";
      }
      message_list = message_list + "{\"key\":\"" + cache_->Read(*it, "key") + "\",\"text\":\"" + cache_->Read(*it, "text") + "\"}";
    }
    message_list = "[" + message_list + "]";
    return message_list;
  }
}
