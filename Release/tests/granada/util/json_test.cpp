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
 * Tests for granada::util::json
 *
 * =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 **/
#include "stdafx.h"
#include "granada/util/time.h"
#include "granada/util/json.h"
#include "cpprest/json.h"


namespace granada { namespace test { namespace util {
    
SUITE(string)
{

	TEST(as_string)
	{
	    web::json::value json = web::json::value::object();
	    json["hello"] = web::json::value::string("world");
	    json["obj"] = web::json::value::object();
	    json["arr"] = web::json::value::array(0);
	    web::json::value null_json;
	    json["null"] = null_json;

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(json.at("hello")),"world");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(json.at("hello"),"hello"),"");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(json,"hello"),"world");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(json,"other"),"");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(json,"obj"),"");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(json,"arr"),"");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(null_json,"arr"),"");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(null_json),"");

	    VERIFY_ARE_EQUAL(granada::util::json::as_string(json,"null"),"");
	}


	TEST(as_object)
	{
	    web::json::value json = web::json::value::object();
	    json["hello"] = web::json::value::string("world");
	    json["obj"] = web::json::value::object();
	    json["obj"]["obj2"] = web::json::value::object();
	    json["obj"]["obj3"] = web::json::value::string("world_obj3");
	    json["arr"] = web::json::value::array(0);
	    web::json::value null_json;
	    json["null"] = null_json;

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json.at("hello"),"hello"),web::json::value::object());

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json,"hello"),web::json::value::object());

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json,"other"),web::json::value::object());

	    web::json::value json2 = web::json::value::object();
	    json2["obj2"] = web::json::value::object();
	    json2["obj3"] = web::json::value::string("world_obj3");

	    VERIFY_ARE_NOT_EQUAL(granada::util::json::as_object(json,"obj"),web::json::value::object());

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json,"obj"),json2);

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json,"obj2"),web::json::value::object());

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json,"obj3"),web::json::value::object());

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json,"arr"),web::json::value::object());

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(null_json,"arr"),web::json::value::object());

	    VERIFY_ARE_EQUAL(granada::util::json::as_object(json,"null"),web::json::value::object());

	}



	TEST(as_array)
	{
	    web::json::value json = web::json::value::object();
	    json["hello"] = web::json::value::string("world");
	    json["obj"] = web::json::value::object();
	    json["obj"]["obj2"] = web::json::value::object();
	    json["obj"]["obj3"] = web::json::value::string("world_obj3");
	    json["arr"] = web::json::value::array(3);
	    json["arr"][0] = web::json::value::string("str1");
	    json["arr"][1] = web::json::value::string("str2");
	    json["arr"][2] = web::json::value::string("str3");
	    web::json::value null_json;
	    json["null"] = null_json;

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json.at("hello"),"hello"),web::json::value::array());

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"hello"),web::json::value::array());

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"other"),web::json::value::array());

	    web::json::value arr2 = web::json::value::array(3);
	    arr2[0] = web::json::value::string("str1");
	    arr2[1] = web::json::value::string("str2");
	    arr2[2] = web::json::value::string("str3");

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"obj"),web::json::value::array());

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"obj"),web::json::value::array());

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"obj2"),web::json::value::array());

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"obj3"),web::json::value::array());

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"arr"),arr2);

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(null_json,"arr"),web::json::value::array());

	    VERIFY_ARE_EQUAL(granada::util::json::as_array(json,"null"),web::json::value::array());

	}

	TEST(first)
	{
	    web::json::value json = web::json::value::object();
	    json["hello"] = web::json::value::string("world");
	    json["obj"] = web::json::value::object();
	    json["obj"]["obj2"] = web::json::value::object();
	    json["obj"]["obj3"] = web::json::value::string("world_obj3");
	    json["arr"] = web::json::value::array(3);
	    json["arr"][0] = web::json::value::string("str1");
	    json["arr"][1] = web::json::value::string("str2");
	    json["arr"][2] = web::json::value::string("str3");
	    web::json::value null_json;
	    json["null"] = null_json;

	    VERIFY_IS_TRUE(granada::util::json::first(json.at("hello")).is_null());

	    VERIFY_ARE_EQUAL(granada::util::json::first(json.at("arr")),web::json::value::string("str1"));

	    VERIFY_IS_TRUE(granada::util::json::first(null_json).is_null());

	}

}
    
}}} //namespaces
