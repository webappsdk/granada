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
  * Handles cache of a website in different ways, from no-cache to in memory websites.
  */

#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include "rapidjson/document.h"
#include "boost/filesystem.hpp"
#include <boost/functional/hash.hpp>
#include "granada/util/application.h"


namespace granada{

  namespace cache{

    /**
     * Web resource
     * Example:
     * 		content_type     => text/javascript; charset=utf-8
     * 		content_encoding => gzip
     * 		content          => console.log("content of a javascript resource.");
     */
    struct Resource{
      std::string content_type;
      std::string content_encoding;
      std::string last_modified;
      std::string ETag;
      std::vector<unsigned char> content;
    };


    /**
     * Handles the cache of website or web application.
     */
    class WebResourceCacheHandler
    {
      public:


        /**
         * Constructor
         */
        WebResourceCacheHandler();


        /**
         * Returns Resource type of file containing the "Content type", the "Content encoding"
         * and the content of the resource.
         * This content could be cached or not, if it is not cached it will retrieve the
         * information from a file in the hard drive.
         *
         * @param   file_path Relative path of the file. In the case of the http_request, it will be the relative uri path.
         * @return  string    Content type of the file.
         */
        granada::cache::Resource GetFile(std::string& file_path);


        /**
         * Returns content encoding: can be gzip or empty string.
         * @return string Content encoding: gzip | empty string.
         */
        std::string GetContentEncoding();


        /**
         * Insert a record in the files_ unordered map.
         * @param resource_path Path of the resource, it has to be unique.
         * @param resource      Resource.
         */
        void CacheRecord(const std::string& resource_path, const granada::cache::Resource& resource);

      private:


        /**
         * Cache according to the given properties.
         */
        void Start();


        /**
         * Load and parse cache configuration properties
         * from server configuration file into class members.
         */
        void LoadConfig();


        /**
         * Makes a copy of the root directory and gzip all the
         * files with the extension indicated to gzip in the
         * gzip_extensions property of the server configuration file.
         * Eample: if html is part of the gzip_extensions property in the server
         * configuration file then all files with html extension
         * will be gziped.
         */
        void GzipCopy();


        /**
         * Load files in memory.
         * @param  relative_path        Path of the file without the root path (relative uri path).
         * @param maximum_cache_memory  Limit of bytes to load.
         * @return  True if some files have been cached, and false if no file has been cached.
         */
        bool RecursiveLoad(const std::string &relative_path, int& maximum_cache_memory);


        /**
         * Returns the content type based on a given extension, checking the
         * data given in the server config file.
         * A content type has the form of "text/html; charset=utf-8"
         * If no content type is found it returns the default content type
         * indicated in the server config file.
         * @param  extension      Extension of the file
         * @return content type
         */
        std::string GetExtensionContentType(const std::string& extension);


        /**
         * Returns the content encoding based on a given extension, checking the
         * data given in the server config file.
         * A content encoding has the form of "gzip".
         * @param  extension          Extension of the file.
         * @return content encoding   gzip | ""
         */
        std::string GetExtensionContentEncoding(const std::string& extension);


        /**
         * Format a date to this format: Tue, 15 Nov 1994 12:45:26 GMT
         * @param  modification_date Date we want to format.
         * @return formated date.
         */
        std::string FormatLastModified(const std::time_t date);


        /**
         * Generate ETag based on a string
         * @param  resource_path Path of the resource, something like /about/index.html
         * @param  last_modified Modification date of the resource. Format: Tue, 15 Nov 1994 12:45:26 GMT
         * @return ETag Etag to identify the version of a resource. Example: 17163160063112100381
         */
        std::string GenerateETag(const std::string& resource_path, const std::string& last_modified);


        /**
         * Root path where all the web files are stored.
         * it can be something like /etc/granada/www
         * It is taken from the server configuration file, if in the
         * configuration file the indicated path does not start with a slash
         * then root path will be the path where the granada server
         * executable is + the path indicated in the configuration file,
         * if no path indicated granada server executable path + www will be taken.
         */
        std::string root_path_;


        /**
         * Contains the file extensions as keys and content types.
         * Eamples:
         * 		png => image/png
         * 		html => text/html; charset=utf-8
         */
        std::unordered_map<std::string, std::string> content_types_;


        /**
         * Contains the files relative paths and their properties.
         * The properties are the "Content Type", the "Content Encoding"
         * and the content of the file.
         * Examples:
         * 		files_
         * 			/about/
         * 				|_ content_type     => text/html; charset=utf-8
         * 				|_ content_encoding => gzip
         * 				|_ content          => <html>()...)</html>
         *      /about
         *      	|_ content_type     => text/html; charset=utf-8
         *      	|_ content_encoding => gzip
         * 				|_ content          => <html>()...)</html>
         *      /about/index.html
         *      	|_ content_type     => text/html; charset=utf-8
         *      	|_ content_encoding => gzip
         * 				|_ content          => <html>()...)</html>
         *      /resources/js/cookinapps.min.js
         *      	|_ content_type     => text/javascript; charset=utf-8
         *      	|_ content_encoding => gzip
         * 				|_ content          => console.log("content of a javascript resource.");
         *      /resources/img/image.png
         *      	|_ content_type     => text/javascript; charset=utf-8
         *      	|_ content_encoding => ""
         *      	|_ content          => PNG ...
         */
        std::unordered_map<std::string, granada::cache::Resource> files_;


        /**
         * JSON Array containing the files to be returned
         * if path is not complete.
         * Example: if user asks for the content of /about/
         * as /about/ is not a file the paths needs to be completed
         * So we try with the first element in default_files_ :
         * default_files=["index.html","index.htm"]
         * the path then will be /about/index.html, if /about/index.html
         * is not found then we will try with index.htm.
         * default_files is a property indicated in the server configuration
         * file.
         */
        rapidjson::Document default_files_;


        /**
         * JSON Object containing the files path with the content to show in case there is an error.
         */
        rapidjson::Document error_paths_;


        /**
         * JSON Array containing the extensions of the files that have to be gziped.
         */
        rapidjson::Document gzip_extensions_;


        /**
         * True if files will be gziped. False if not.
         */
        bool gzip_content_ = false;

    };
  }
}
