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
  * Manages the cache storing key-value pairs or sets of key-value pairs using redis
  * data structure server (http://redis.io/).
  * It uses redisclient by Alex Nekipelov https://github.com/nekipelov/redisclient
  * This code is multi-thread safe.
  *
  */
#pragma once
#include "granada/util/application.h"
#include "cache_handler.h"
#include <string>
#include <mutex>
#include "redisclient/redissyncclient.h"


namespace granada{
  namespace cache{
    class RedisCacheDriver : public CacheHandler
    {
      public:

        /**
         * Controler
         */
        RedisCacheDriver();


        /**
         * Override. Returns the value associated with the given key.
         * @param  key Key associated with the value.
         * @return     Value.
         */
        const std::string Read(const std::string& key);


        /**
         * Returns the value stored in a set and associated with the given key.
         * @param  hash Name of the set where the key-value pairs are stored.
         * @param  key  Key associated with the value.
         * @return      Value.
         */
        const std::string Read(const std::string& hash, const std::string& key);


        /**
         * Override. Inserts a key-value pair, rewrites it if it already exists.
         * @param key   Key to identify the value.
         * @param value Value
         */
        void Write(const std::string& key,const std::string& value);


        /**
         * Inserts or rewrite a key-value pair in a set with the given name.
         * If the set does not exist, it creates it.
         * @param hash  Name of the set.
         * @param key   Key to identify the value inside the set.
         * @param value Value
         */
        void Write(const std::string& hash,const std::string& key,const std::string& value);


        /**
         * Destroys a key-value pair or a set of values.
         * @param key Key of the value or name of the set to destroy.
         */
        void Destroy(const std::string& key);


        /**
         * Destroy a key-value pair stored in a set.
         * @param hash Name of the set where the key-value pair is stored.
         * @param key  Key associated with the value.
         */
        void Destroy(const std::string& hash,const std::string& key);


      protected:

        /**
         * Redis client.
         */
        std::shared_ptr<RedisSyncClient> redis_;


        /**
         * Mutex. For multi-thread safety.
         */
        std::mutex mtx;


        /**
         * Address used in case "redis_cache_driver_address" property
         * is not provided.
         */
        const std::string DEFAULT_REDIS_ADDRESS = "127.0.0.1";


        /**
         * Loaded in LoadProperties() function, will take the value
         * of the "redis_cache_driver_address" property. If the property
         * is not provided DEFAULT_REDIS_ADDRESS will be taken instead.
         */
        std::string redis_address_;


        /**
         * Port used in case "redis_cache_driver_port" property
         * is not provided.
         */
        const unsigned short DEFAULT_REDIS_PORT = 6379;


        /**
         * Loaded in LoadProperties() function, will take the value
         * of the "redis_cache_driver_port" property. If the property
         * is not provided DEFAULT_REDIS_PORT will be taken instead.
         */
        unsigned short redis_port_;


        /**
         * Load properties for configuring the redis server connection.
         */
        void LoadProperties();

    };
  }
}
