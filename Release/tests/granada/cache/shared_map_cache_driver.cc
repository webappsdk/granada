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
 * =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
 *
 * Tests for granada::util::string
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 **/
#include "stdafx.h"
#include <vector>
#include "granada/util/time.h"
#include "granada/cache/shared_map_cache_driver.h"

namespace granada { namespace test { namespace cache {
    
SUITE(shared_map_cache_driver)
{

	TEST(write_read)
	{
		granada::cache::SharedMapCacheDriver cache_driver;
		cache_driver.Write("hello","world");
		VERIFY_ARE_EQUAL(cache_driver.Read("hello"),"world");

		cache_driver.Write("hello","world","!!!");
		VERIFY_ARE_EQUAL(cache_driver.Read("hello","world"),"!!!");
	}


	TEST(exist)
	{
		granada::cache::SharedMapCacheDriver cache_driver;
		cache_driver.Write("hello","world");

		VERIFY_IS_TRUE(cache_driver.Exists("hello"));
		VERIFY_IS_FALSE(cache_driver.Exists("none"));

		cache_driver.Write("hello","world","!!!");
		VERIFY_IS_TRUE(cache_driver.Exists("hello"));
		VERIFY_IS_TRUE(cache_driver.Exists("hello","world"));
		VERIFY_IS_FALSE(cache_driver.Exists("none","world"));
		VERIFY_IS_FALSE(cache_driver.Exists("hello","none"));
	}

	TEST(match)
	{
		granada::cache::SharedMapCacheDriver cache_driver;
		cache_driver.Write("hello","world");
		cache_driver.Write("session:6464","token","6464");
		cache_driver.Write("session:6464","update.time","123456789");
		cache_driver.Write("session:roles:6464","ROLES_6464");
		cache_driver.Write("session:777","token","777");
		cache_driver.Write("session:777","update.time","987654321");
		cache_driver.Write("session:roles:777","ROLES_777");

		std::vector<std::string> keys;

		cache_driver.Match("",keys);
		VERIFY_IS_TRUE(keys.size()==0);

		cache_driver.Match("*",keys);
		VERIFY_IS_TRUE(keys.size()==5);

		cache_driver.Match("session:",keys);
		VERIFY_IS_TRUE(keys.size()==0);

		cache_driver.Match("session:*",keys);
		VERIFY_IS_TRUE(keys.size()==4);

		cache_driver.Match("session:*:",keys);
		VERIFY_IS_TRUE(keys.size()==0);

		cache_driver.Match("session:*:*",keys);
		VERIFY_IS_TRUE(keys.size()==2);

		cache_driver.Match("session:6464*",keys);
		VERIFY_IS_TRUE(keys.size()==1);

		cache_driver.Match("session:*6464*",keys);
		VERIFY_IS_TRUE(keys.size()==2);
	}


	TEST(destroy)
	{
		granada::cache::SharedMapCacheDriver cache_driver;
		cache_driver.Write("hello","world");

		VERIFY_ARE_EQUAL(cache_driver.Read("hello"),"world");
		VERIFY_IS_TRUE(cache_driver.Exists("hello"));
		VERIFY_IS_FALSE(cache_driver.Exists("none"));
		cache_driver.Destroy("hello");
		VERIFY_ARE_NOT_EQUAL(cache_driver.Read("hello"),"world");
		VERIFY_ARE_EQUAL(cache_driver.Read("hello"),"");
		VERIFY_IS_FALSE(cache_driver.Exists("hello"));

		cache_driver.Write("hello","world","!!!");
		VERIFY_ARE_EQUAL(cache_driver.Read("hello","world"),"!!!");
		VERIFY_IS_TRUE(cache_driver.Exists("hello"));
		VERIFY_IS_TRUE(cache_driver.Exists("hello","world"));
		VERIFY_IS_FALSE(cache_driver.Exists("none","world"));
		VERIFY_IS_FALSE(cache_driver.Exists("hello","none"));
		cache_driver.Destroy("hello","world");
		VERIFY_ARE_NOT_EQUAL(cache_driver.Read("hello","world"),"!!!");
		VERIFY_ARE_EQUAL(cache_driver.Read("hello","world"),"");
		VERIFY_IS_TRUE(cache_driver.Exists("hello"));
		VERIFY_IS_FALSE(cache_driver.Exists("hello","world"));

	}


	TEST(iterator)
	{
		granada::cache::SharedMapCacheDriver cache_driver;
		cache_driver.Write("hello","world");
		cache_driver.Write("session:6464","token","6464");
		cache_driver.Write("session:6464","update.time","123456789");
		cache_driver.Write("session:roles:6464","ROLES_6464");
		cache_driver.Write("session:777","token","777");
		cache_driver.Write("session:777","update.time","987654321");
		cache_driver.Write("session:roles:777","ROLES_777");


		std::shared_ptr<granada::cache::CacheHandlerIterator> cache_iterator = cache_driver.make_iterator("");
		VERIFY_IS_TRUE(!cache_iterator->has_next());
		for (int i = 0; i < 5; i++){
			VERIFY_IS_TRUE(cache_iterator->next()=="");
		}

		cache_iterator = cache_driver.make_iterator("*");
		int i = 0;
		while(cache_iterator->has_next()){
			i++;
			cache_iterator->next();
		}
		VERIFY_IS_TRUE(i==5);


		cache_iterator = cache_driver.make_iterator("session:");
		i = 0;
		while(cache_iterator->has_next()){
			i++;
			cache_iterator->next();
		}
		VERIFY_IS_TRUE(i==0);

		cache_iterator = cache_driver.make_iterator("session:*");
		i = 0;
		while(cache_iterator->has_next()){
			i++;
			cache_iterator->next();
		}
		VERIFY_IS_TRUE(i==4);

		cache_iterator = cache_driver.make_iterator("session:*:");
		i = 0;
		while(cache_iterator->has_next()){
			i++;
			cache_iterator->next();
		}
		VERIFY_IS_TRUE(i==0);

		cache_iterator = cache_driver.make_iterator("session:*:*");
		i = 0;
		while(cache_iterator->has_next()){
			i++;
			cache_iterator->next();
		}
		VERIFY_IS_TRUE(i==2);

		cache_iterator = cache_driver.make_iterator("session:6464*");
		i = 0;
		while(cache_iterator->has_next()){
			i++;
			cache_iterator->next();
		}
		VERIFY_IS_TRUE(i==1);

		cache_iterator = cache_driver.make_iterator("session:*6464*");
		i = 0;
		while(cache_iterator->has_next()){
			i++;
			cache_iterator->next();
		}
		VERIFY_IS_TRUE(i==2);
	}

}
    
}}} //namespaces
