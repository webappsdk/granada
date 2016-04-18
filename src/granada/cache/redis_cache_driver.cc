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
  * Manages the cache with a redis database.
  */

#include "granada/cache/redis_cache_driver.h"

namespace granada{
  namespace cache{

    RedisCacheDriver::RedisCacheDriver(){

      boost::asio::ip::address address = boost::asio::ip::address::from_string("127.0.0.1");
      const unsigned short port = 6379;

      boost::asio::io_service ioService;
      redis_ = std::shared_ptr<RedisSyncClient>(new RedisSyncClient(ioService));

      std::string errmsg;

      if( !redis_->connect(address, port, errmsg) )
      {
          // log debug
          std::cout << "Can t connect to redis: " << errmsg << std::endl;
      }
    }

    std::string RedisCacheDriver::Read(const std::string& key){
      mtx.lock();
      RedisValue result = redis_->command("GET", key);
      if( result.isOk() )
      {
        mtx.unlock();
        return result.toString();
      }
      mtx.unlock();
      return std::string();
    }

    std::string RedisCacheDriver::Read(const std::string& hash,const std::string& key){
      mtx.lock();
      RedisValue result = redis_->command("HGET", hash, key);
      if( result.isOk() )
      {
        mtx.unlock();
        return result.toString();
      }
      mtx.unlock();
      return std::string();
    }

    void RedisCacheDriver::Write(const std::string& key,const std::string& value){
      mtx.lock();
      redis_->command("SET", key, value);
      mtx.unlock();
    }

    void RedisCacheDriver::Write(const std::string& hash,const std::string& key,const std::string& value){
      mtx.lock();
      redis_->command("HSET", hash, key, value);
      mtx.unlock();
    }

    void RedisCacheDriver::Destroy(const std::string& key){
      mtx.lock();
      redis_->command("DEL", key);
      mtx.unlock();
    }

    void RedisCacheDriver::Destroy(const std::string& hash,const std::string& key){
      mtx.lock();
      redis_->command("HDEL", hash, key);
      mtx.unlock();
    }

  }
}
