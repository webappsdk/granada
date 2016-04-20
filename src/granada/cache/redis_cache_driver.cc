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

      LoadProperties();

      boost::asio::ip::address address = boost::asio::ip::address::from_string(redis_address_);
      const unsigned short port = redis_port_;

      boost::asio::io_service ioService;
      redis_ = std::shared_ptr<RedisSyncClient>(new RedisSyncClient(ioService));

      std::string errmsg;

      if( !redis_->connect(address, port, errmsg) )
      {
          std::cout << "Can t connect to redis: " << errmsg << std::endl;
      }
    }


    void RedisCacheDriver::LoadProperties(){
      redis_address_.assign(granada::util::application::GetProperty("redis_cache_driver_address"));
      if (redis_address_.empty()){
        redis_address_.assign(DEFAULT_REDIS_ADDRESS);
      }

      std::string redis_port_str = granada::util::application::GetProperty("redis_cache_driver_port");
      if (redis_port_str.empty()){
        redis_port_ = DEFAULT_REDIS_PORT;
      }else{
        try{
          redis_port_ = (unsigned short) std::strtoul(redis_port_str.c_str(), NULL, 0);
        }catch(const std::exception& e){
          redis_port_ = DEFAULT_REDIS_PORT;
        }
      }
    }


    const std::string RedisCacheDriver::Read(const std::string& key){
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


    const std::string RedisCacheDriver::Read(const std::string& hash,const std::string& key){
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
