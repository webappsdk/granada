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
#include "granada/util/time.h"
#include "granada/util/string.h"
#include <unordered_map>
#include <map>
#include <vector>
#include <deque>
#include "cpprest/json.h"


namespace granada { namespace test { namespace util {
    
SUITE(string)
{

	TEST(ltrim)
	{
	    std::string str = " hello";
	    granada::util::string::ltrim(str);
	    VERIFY_ARE_EQUAL(str, "hello");

	    str = "                          hello             ";
	    granada::util::string::ltrim(str);
	    VERIFY_ARE_EQUAL(str,"hello             ");

	    str = "		hello	";
	    granada::util::string::ltrim(str);
	    VERIFY_ARE_EQUAL(str,"hello	");

	    str = " h e l l o ";
	    granada::util::string::ltrim(str);
	    VERIFY_ARE_EQUAL(str,"h e l l o ");
	}


	TEST(rtrim)
	{
	    std::string str = "hello ";
	    granada::util::string::rtrim(str);
	    VERIFY_ARE_EQUAL(str, "hello");

	    str = "                          hello             ";
	    granada::util::string::rtrim(str);
	    VERIFY_ARE_EQUAL(str,"                          hello");

	    str = "		hello	";
	    granada::util::string::rtrim(str);
	    VERIFY_ARE_EQUAL(str,"		hello");

	    str = " h e l l o ";
	    granada::util::string::rtrim(str);
	    VERIFY_ARE_EQUAL(str," h e l l o");
	}


	TEST(trim)
	{

	    std::string str = "hello ";
	    granada::util::string::trim(str);
	    VERIFY_ARE_EQUAL(str, "hello");

	    str = "                          hello             ";
	    granada::util::string::trim(str);
	    VERIFY_ARE_EQUAL(str,"hello");

	    str = "		hello	";
	    granada::util::string::trim(str);
	    VERIFY_ARE_EQUAL(str,"hello");

	    str = " h e l l o ";
	    granada::util::string::trim(str);
	    VERIFY_ARE_EQUAL(str,"h e l l o");
	}


	TEST(replace)
	{
		std::deque<std::pair<std::string,std::string>> values;
		values.push_back(std::make_pair("hello","world"));
		values.push_back(std::make_pair("o","world"));


	    std::string str = "hello ";
	    granada::util::string::replace(str,values,"l"," ");
	    VERIFY_ARE_EQUAL(str, "helworld");

	    str = "hello ";
	    granada::util::string::replace(str,values," ","");
	    VERIFY_ARE_EQUAL(str,"hello ");

	    str = "		hello	";
	    granada::util::string::replace(str,values,"\t","\t");
	    VERIFY_ARE_EQUAL(str,"\tworld");

	    str = "{{hello}} my name is {{hello}}, {{hello}} ";
	    granada::util::string::replace(str,values,"{{","}}");
	    VERIFY_ARE_EQUAL(str,"world my name is world, world ");

	    str = "{{hello}} my name is {{hello}}, {{hello}} ";
	    granada::util::string::replace(str,values);
	    VERIFY_ARE_EQUAL(str,"world my name is world, world ");
	    
	}


	TEST(replace_better_container){

		// compare which container is the fastest for replace function.

		const std::string& open = "{{";
		const std::string& close = "}}";
		const int& loops = 0;

		// unordered map
		int start_time = granada::util::time::get_milliseconds();
		for (int i = 0; i < loops; i++){
			std::unordered_map<std::string,std::string> values;
			values.insert(std::make_pair("action","x6kMAVVSBxvMpj6Z"));
			values.insert(std::make_pair("response_type","b5s7lt9k7gby7zMz"));
			values.insert(std::make_pair("scope","GFF9a2FBPUm3Fc7Y"));
			values.insert(std::make_pair("client_id","r3M2zHjKMY1MkfdH"));
			values.insert(std::make_pair("redirect_uri","GYVP5369T4ltOUsW"));
			values.insert(std::make_pair("state","NbMSlP8sbpJt0IIi"));

			std::string text = "<form id=\"authorization-form\" method=\"POST\" action=\"{{action}}\" enctype=\"application/x-www-form-urlencoded\"><input type=\"text\" name=\"username\" placeholder=\"username\"></input><br /><input type=\"password\" name=\"password\" placeholder=\"password\"></input><br /><input type=\"hidden\" name=\"response_type\" value=\"{{response_type}}\"></input><input type=\"hidden\" name=\"scope\" value=\"{{scope}}\"></input><input type=\"hidden\" name=\"client_id\" value=\"{{client_id}}\"></input><input type=\"hidden\" name=\"redirect_uri\" value=\"{{redirect_uri}}\"></input><input type=\"hidden\" name=\"state\" value=\"{{state}}\"></input><button>Authorize application</button></form>";

			std::string tag;
	        std::string value;
	        for (auto it = values.begin(); it != values.end(); ++it){
				tag.assign(open + it->first + close);
				value = it->second;
				size_t pos = 0;
				while ((pos = text.find(tag, pos)) != std::string::npos) {
				   text.replace(pos, tag.length(), value);
				   pos += value.length();
				}
        	}
		}
		int span = granada::util::time::get_milliseconds_span(start_time);

		std::cout << "replace_better_container [unordered_map] : " << span << "ms\n";


		// map
		start_time = granada::util::time::get_milliseconds();
		for (int i = 0; i < loops; i++){
			std::map<std::string,std::string> values;
			values.insert(std::make_pair("action","x6kMAVVSBxvMpj6Z"));
			values.insert(std::make_pair("response_type","b5s7lt9k7gby7zMz"));
			values.insert(std::make_pair("scope","GFF9a2FBPUm3Fc7Y"));
			values.insert(std::make_pair("client_id","r3M2zHjKMY1MkfdH"));
			values.insert(std::make_pair("redirect_uri","GYVP5369T4ltOUsW"));
			values.insert(std::make_pair("state","NbMSlP8sbpJt0IIi"));

			std::string text = "<form id=\"authorization-form\" method=\"POST\" action=\"{{action}}\" enctype=\"application/x-www-form-urlencoded\"><input type=\"text\" name=\"username\" placeholder=\"username\"></input><br /><input type=\"password\" name=\"password\" placeholder=\"password\"></input><br /><input type=\"hidden\" name=\"response_type\" value=\"{{response_type}}\"></input><input type=\"hidden\" name=\"scope\" value=\"{{scope}}\"></input><input type=\"hidden\" name=\"client_id\" value=\"{{client_id}}\"></input><input type=\"hidden\" name=\"redirect_uri\" value=\"{{redirect_uri}}\"></input><input type=\"hidden\" name=\"state\" value=\"{{state}}\"></input><button>Authorize application</button></form>";

			std::string tag;
	        std::string value;
	        for (auto it = values.begin(); it != values.end(); ++it){
				tag.assign(open + it->first + close);
				value = it->second;
				size_t pos = 0;
				while ((pos = text.find(tag, pos)) != std::string::npos) {
				   text.replace(pos, tag.length(), value);
				   pos += value.length();
				}
        	}
		}
		span = granada::util::time::get_milliseconds_span(start_time);

		std::cout << "replace_better_container [map] : " << span << "ms\n";


		// vector
		start_time = granada::util::time::get_milliseconds();
		for (int i = 0; i < loops; i++){
			std::vector<std::pair<std::string,std::string>> values;
			values.push_back(std::make_pair("action","x6kMAVVSBxvMpj6Z"));
			values.push_back(std::make_pair("response_type","b5s7lt9k7gby7zMz"));
			values.push_back(std::make_pair("scope","GFF9a2FBPUm3Fc7Y"));
			values.push_back(std::make_pair("client_id","r3M2zHjKMY1MkfdH"));
			values.push_back(std::make_pair("redirect_uri","GYVP5369T4ltOUsW"));
			values.push_back(std::make_pair("state","NbMSlP8sbpJt0IIi"));

			std::string text = "<form id=\"authorization-form\" method=\"POST\" action=\"{{action}}\" enctype=\"application/x-www-form-urlencoded\"><input type=\"text\" name=\"username\" placeholder=\"username\"></input><br /><input type=\"password\" name=\"password\" placeholder=\"password\"></input><br /><input type=\"hidden\" name=\"response_type\" value=\"{{response_type}}\"></input><input type=\"hidden\" name=\"scope\" value=\"{{scope}}\"></input><input type=\"hidden\" name=\"client_id\" value=\"{{client_id}}\"></input><input type=\"hidden\" name=\"redirect_uri\" value=\"{{redirect_uri}}\"></input><input type=\"hidden\" name=\"state\" value=\"{{state}}\"></input><button>Authorize application</button></form>";

			std::string tag;
	        std::string value;
	        for (auto it = values.begin(); it != values.end(); ++it){
				tag.assign(open + it->first + close);
				value = it->second;
				size_t pos = 0;
				while ((pos = text.find(tag, pos)) != std::string::npos) {
				   text.replace(pos, tag.length(), value);
				   pos += value.length();
				}
        	}
		}
		span = granada::util::time::get_milliseconds_span(start_time);

		std::cout << "replace_better_container [vector] : " << span << "ms\n";


		// deque push_front
		start_time = granada::util::time::get_milliseconds();
		for (int i = 0; i < loops; i++){
			std::deque<std::pair<std::string,std::string>> values;
			values.push_front(std::make_pair("action","x6kMAVVSBxvMpj6Z"));
			values.push_front(std::make_pair("response_type","b5s7lt9k7gby7zMz"));
			values.push_front(std::make_pair("scope","GFF9a2FBPUm3Fc7Y"));
			values.push_front(std::make_pair("client_id","r3M2zHjKMY1MkfdH"));
			values.push_front(std::make_pair("redirect_uri","GYVP5369T4ltOUsW"));
			values.push_front(std::make_pair("state","NbMSlP8sbpJt0IIi"));

			std::string text = "<form id=\"authorization-form\" method=\"POST\" action=\"{{action}}\" enctype=\"application/x-www-form-urlencoded\"><input type=\"text\" name=\"username\" placeholder=\"username\"></input><br /><input type=\"password\" name=\"password\" placeholder=\"password\"></input><br /><input type=\"hidden\" name=\"response_type\" value=\"{{response_type}}\"></input><input type=\"hidden\" name=\"scope\" value=\"{{scope}}\"></input><input type=\"hidden\" name=\"client_id\" value=\"{{client_id}}\"></input><input type=\"hidden\" name=\"redirect_uri\" value=\"{{redirect_uri}}\"></input><input type=\"hidden\" name=\"state\" value=\"{{state}}\"></input><button>Authorize application</button></form>";

			std::string tag;
	        std::string value;
	        for (auto it = values.begin(); it != values.end(); ++it){
				tag.assign(open + it->first + close);
				value = it->second;
				size_t pos = 0;
				while ((pos = text.find(tag, pos)) != std::string::npos) {
				   text.replace(pos, tag.length(), value);
				   pos += value.length();
				}
        	}
		}
		span = granada::util::time::get_milliseconds_span(start_time);

		std::cout << "replace_better_container [deque push_front] : " << span << "ms\n";


		// deque push_back
		start_time = granada::util::time::get_milliseconds();
		for (int i = 0; i < loops; i++){
			std::deque<std::pair<std::string,std::string>> values;
			values.push_back(std::make_pair("action","x6kMAVVSBxvMpj6Z"));
			values.push_back(std::make_pair("response_type","b5s7lt9k7gby7zMz"));
			values.push_back(std::make_pair("scope","GFF9a2FBPUm3Fc7Y"));
			values.push_back(std::make_pair("client_id","r3M2zHjKMY1MkfdH"));
			values.push_back(std::make_pair("redirect_uri","GYVP5369T4ltOUsW"));
			values.push_back(std::make_pair("state","NbMSlP8sbpJt0IIi"));

			std::string text = "<form id=\"authorization-form\" method=\"POST\" action=\"{{action}}\" enctype=\"application/x-www-form-urlencoded\"><input type=\"text\" name=\"username\" placeholder=\"username\"></input><br /><input type=\"password\" name=\"password\" placeholder=\"password\"></input><br /><input type=\"hidden\" name=\"response_type\" value=\"{{response_type}}\"></input><input type=\"hidden\" name=\"scope\" value=\"{{scope}}\"></input><input type=\"hidden\" name=\"client_id\" value=\"{{client_id}}\"></input><input type=\"hidden\" name=\"redirect_uri\" value=\"{{redirect_uri}}\"></input><input type=\"hidden\" name=\"state\" value=\"{{state}}\"></input><button>Authorize application</button></form>";

			std::string tag;
	        std::string value;
	        for (auto it = values.begin(); it != values.end(); ++it){
				tag.assign(open + it->first + close);
				value = it->second;
				size_t pos = 0;
				while ((pos = text.find(tag, pos)) != std::string::npos) {
				   text.replace(pos, tag.length(), value);
				   pos += value.length();
				}
        	}
		}
		span = granada::util::time::get_milliseconds_span(start_time);

		std::cout << "replace_better_container [deque push_back] : " << span << "ms\n";

	}


	TEST(to_json)
	{
		web::json::value obj = web::json::value::object();
		obj["hello"] = web::json::value::string("world");
		web::json::value arr = web::json::value::array(3);
		arr[0] = web::json::value::number(1);
		arr[1] = web::json::value::number(2);
		arr[2] = web::json::value::number(3);

		std::string str = "{\"hello\":\"world\",\"arr\":[1,2,3]}";
		web::json::value json = granada::util::string::to_json(str);

		VERIFY_IS_TRUE(json.has_field("hello"));
		VERIFY_IS_TRUE(json.at("hello").is_string());
		VERIFY_IS_TRUE(json.at("hello").as_string()=="world");
		VERIFY_IS_TRUE(json.has_field("arr"));
		VERIFY_IS_TRUE(json.at("arr").is_array());
		VERIFY_ARE_EQUAL(json.at("arr"),arr);


		str = "[1,2,3]";
		VERIFY_ARE_EQUAL(granada::util::string::to_json(str),arr);


		obj = web::json::value::object();
		str = "sdjkfkld";
		VERIFY_ARE_EQUAL(granada::util::string::to_json(str),obj);

		str = "{hello:\"world\"}";
		VERIFY_ARE_EQUAL(granada::util::string::to_json(str),obj);

		str = "";
		VERIFY_ARE_EQUAL(granada::util::string::to_json(str),obj);

		str = "{}";
		VERIFY_ARE_EQUAL(granada::util::string::to_json(str),obj);

		str = "[lkhjlkj]";
		VERIFY_ARE_EQUAL(granada::util::string::to_json(str),obj);
	}


	TEST(stringified_json)
	{

		std::string str = "{\"hello\":\"world\"}";
		VERIFY_ARE_EQUAL(granada::util::string::stringified_json(str),str);

		str = "[1,2,3]";
		VERIFY_ARE_EQUAL(granada::util::string::stringified_json(str),str);

		str = "sdjkfkld";
		VERIFY_ARE_EQUAL(granada::util::string::stringified_json(str),"{}");

		str = "{hello:\"world\"}";
		VERIFY_ARE_EQUAL(granada::util::string::stringified_json(str),"{}");

		str = "";
		VERIFY_ARE_EQUAL(granada::util::string::stringified_json(str),"{}");

		str = "{}";
		VERIFY_ARE_EQUAL(granada::util::string::stringified_json(str),"{}");

		str = "[lkhjlkj]";
		VERIFY_ARE_EQUAL(granada::util::string::stringified_json(str),"{}");
	}

}
    
}}} //namespaces
