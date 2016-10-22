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
  * Used to cache html, js etc. files.
  *
  */

#include "granada/cache/web_resource_cache.h"

namespace granada{

  namespace cache{

    WebResourceCache::WebResourceCache(){
      Start();
    }

    granada::cache::Resource WebResourceCache::GetFile(std::string& file_path){
      if ( !files_.empty() ){
        auto it = files_.find(file_path);
        if (it == files_.end()){
          return granada::cache::Resource();
        }
        granada::cache::Resource resource = it->second;
        return resource;
      }

      // file is not cached so we get the content from the file stored in the hard drive.
      std::string extension = granada::util::file::GetExtension(file_path);
      if (extension.empty()){
        // given path does not have an extension, then it is a directory, we will search for the default file.
        // search for the default file.
        if ( file_path.substr(file_path.length() - 1).compare("/") != 0 ){
          file_path += "/";
        }

        // check if any of the default files exist in the directory.
        if (default_files_.is_array()){
          std::string default_file_path;

          for(auto it = default_files_.as_array().cbegin(); it != default_files_.as_array().cend(); ++it){
			default_file_path = utility::conversions::to_utf8string(it->as_string());
            extension = granada::util::file::GetExtension(default_file_path);

            if (GetExtensionContentEncoding(extension) == "gzip"){
              default_file_path = root_path_ + "/" + file_path + utility::conversions::to_utf8string(it->as_string()) + ".gz";
            }else{
				default_file_path = root_path_ + "/" + file_path + utility::conversions::to_utf8string(it->as_string());
            }

            // if file exists, then take it as the one to get content from.
            if (boost::filesystem::exists(default_file_path)){
              file_path.assign(default_file_path);
              break;
            }else{
              default_file_path.assign("");
            }
          }

          if (error_paths_.has_field(U("404"))){
            try{
              const web::json::value &error_404_file_path_json = error_paths_.at(U("404"));
			  std::string error_404_file_path = utility::conversions::to_utf8string(error_404_file_path_json.as_string());
              if (file_path == error_404_file_path || file_path == error_404_file_path + "/"){
                return granada::cache::Resource();
              }
            }catch(const web::json::json_exception e){
              return granada::cache::Resource();
            }
          }else{
            if (default_file_path.empty()){
              file_path.assign("");
            }
          }
        }
      }else{
        if (GetExtensionContentEncoding(extension) == "gzip"){
          file_path = root_path_ + "/" + file_path + ".gz";
        }else{
          file_path = root_path_ + "/" + file_path;
        }
      }

      if (file_path.empty() || !boost::filesystem::exists(file_path)){
        // return 404 file content if it exists..
        if (error_paths_.has_field(U("404"))){
          try{
            const web::json::value &error_404_file_path_json = error_paths_.at(U("404"));
			std::string error_404_file_path = utility::conversions::to_utf8string(error_404_file_path_json.as_string());
            return GetFile(error_404_file_path);
          }catch(const web::json::json_exception e){}
        }
      }else{
        // read the file and assign content to content string variable
        boost::filesystem::path path(file_path);
        std::ifstream ifs(path.string());
        std::vector<unsigned char> content( (std::istreambuf_iterator<char>(ifs) ),
                           (std::istreambuf_iterator<char>()) );

        std::string content_type = GetExtensionContentType(extension);

        std::string content_encoding = "";
        if (GetExtensionContentEncoding(extension) == "gzip"){
          content_encoding = "gzip";
        }

        granada::cache::Resource resource;
        resource.content_type = content_type;
        resource.content_encoding = content_encoding;
        resource.content = content;

        return resource;
      }

      return granada::cache::Resource();
    }


    std::string WebResourceCache::GetContentEncoding(){
      if(gzip_content_){
        return "gzip";
      }else{
        return std::string();
      }
    }

    void WebResourceCache::CacheRecord(const std::string& resource_path, const granada::cache::Resource& resource){
      files_.insert(std::make_pair(resource_path,resource));
    }


    void WebResourceCache::Start(){

      // get and parse cache configuration properties
      // from server configuration file.
      LoadConfig();

      ////
      // gzip content encoding
      // get property that will tell if gzip content or not (on:gzip;off:do not zip).
      // if gzip true then create a Gziped copy of the files.
      std::string gzip_content_str = granada::util::application::GetProperty("gzip_content");
      if(!gzip_content_str.empty() && gzip_content_str=="on"){
        gzip_content_ = true;
        GzipCopy();
      }

      // get percentage of the files we want to cache (0:none;100:all).
      std::string cache_content = granada::util::application::GetProperty("cache_content");
      if (!cache_content.empty() && cache_content == "on"){
        // get the maximum cache memory property that is
        // the maximum amount of MB that we will load in the cache.
        std::string maximum_cache_memory_str = granada::util::application::GetProperty("maximum_cache_memory");
        int maximum_cache_memory = 0;
        if (!maximum_cache_memory_str.empty()){
          try{
            maximum_cache_memory = std::stoi(maximum_cache_memory_str);
            // convert to bytes
            maximum_cache_memory = maximum_cache_memory * 1024 * 1024;
          }catch(const std::exception e){}
        }

        // load all files in memory.
        RecursiveLoad("/",maximum_cache_memory);
      }
    }


    void WebResourceCache::LoadConfig(){
      ////
      // content types
      // get the pairs of content types and file extensions. Example image/png <=> png .
      std::string content_types_str = granada::util::application::GetProperty("content_types");
      if (!content_types_str.empty()){
        try{
          // parse the json with the content types and files extensions into the unordered_map content_types_ .
		  web::json::value obj = web::json::value::parse(utility::conversions::to_string_t(content_types_str));
          std::string content_type;
          std::string extension;
          // loop through the keys of the json. The keys are the content types.
          for(auto it = obj.as_object().cbegin(); it != obj.as_object().cend(); ++it){
			  const std::string& content_type = utility::conversions::to_utf8string(it->first);
              const web::json::value& extensions_json = it->second;

              // loop through the extensions related to the extracted content type.
              for(auto it2 = extensions_json.as_array().cbegin(); it2 != extensions_json.as_array().cend(); ++it2){
				  extension = utility::conversions::to_utf8string(it2->as_string());
                // insert pair of content type and extension in the unordered_map.
                content_types_.insert(std::make_pair(extension,content_type));
              }

          }
        }catch(const web::json::json_exception e){}
      }

      ////
      // default files
      // get the default files to get content from if the client request
      // only includes the directory path.
      std::string default_files_str = granada::util::application::GetProperty("default_files");
      if (!default_files_str.empty()){
        try{
          default_files_ = web::json::value::parse(utility::conversions::to_string_t(default_files_str));
        }catch(const web::json::json_exception e){
          default_files_ = web::json::value::array();
        }
      }

      ////
      // error files
      // get the path of the files to get content from when there is an error.
      std::string error_paths_str = granada::util::application::GetProperty("error_paths");
      if (!error_paths_str.empty()){
        try{
          error_paths_ = web::json::value::parse(utility::conversions::to_string_t(error_paths_str));
        }catch(const web::json::json_exception e){
          error_paths_ = web::json::value::object(false);
        }
      }

      ////
      // gzip extensions
      // get the extensions of the files to gzip from gzip_extensions property
      // in the server configuration file.
      std::string gzip_extensions_str = granada::util::application::GetProperty("gzip_extensions");
      if (!gzip_extensions_str.empty()){
        try{
		  gzip_extensions_ = web::json::value::parse(utility::conversions::to_string_t(gzip_extensions_str));
        }catch(const web::json::json_exception e){
          gzip_extensions_ = web::json::value::array();
        }
      }

      ////
      // root path
      // get relative path where the files of the web are stored.
      std::string root_path_property = granada::util::application::GetProperty("root_path");
      if (root_path_property.empty()){
        root_path_property = "www";
      }

      // if the root_path_property starts with a slash
      // we supose it is not a relative path
      if (root_path_property[0] == '/'){
        root_path_ = root_path_property;
      }else{
        root_path_ = granada::util::application::get_selfpath() + "/" + root_path_property;
      }

      // check if this path exists, if it does not
      // fire exception
      if ( !boost::filesystem::exists(root_path_) ){
        // root path does not exist exception
      }
    }


    void WebResourceCache::GzipCopy(){
      // make a copy of the files into another directory and gzip it.
      std::string not_gziped_filepath(root_path_);
      root_path_ += "_gzip";
      std::string command;

      // gzip files in another directory.
      #ifdef WINDOWS
        if ( boost::filesystem::exists( root_path_ ) ){
          command = "deltree " + root_path_;
          system(command.c_str());
        }
        command = "xcopy " + not_gziped_filepath + " " + root_path_;
        system(command.c_str());

        // gzip only the files with extension indicated in the gzip_extensions property.
        std::string extensions;

        for(auto it = gzip_extensions_.as_array().cbegin(); it != gzip_extensions_.as_array().cend(); ++it){
          if (!extensions.empty()){
            extensions += "|";
          }
          extensions += "*." + it->as_string();
        }

        // not tested
        command = "For /R " + root_path_ + " %%G IN (" + extensions + ") do gzip \"%%G\"";
        system(command.c_str());
      #else
        // remove gzip directory if it already exists.
        if ( boost::filesystem::exists( root_path_ ) ){
          std::string command = "rm -r " + root_path_;
          system(command.c_str());
        }
        // copy all the directory into a gzip directory.
        command = "cp -r " + not_gziped_filepath + " " + root_path_;
        system(command.c_str());

        // gzip only the files with extension indicated in the gzip_extensions property.
        std::string extensions;
        for(auto it = gzip_extensions_.as_array().cbegin(); it != gzip_extensions_.as_array().cend(); ++it){
          if (!extensions.empty()){
            extensions += "|";
          }
		  extensions += "\\." + utility::conversions::to_utf8string(it->as_string()) + "$";
        }

        // gzip
        if (!extensions.empty()){
          command = "for i in `find " + root_path_ + " | grep -E \"" + extensions +"\"`; do gzip -9 \"$i\" ; done";
          system(command.c_str());
        }
      #endif
    }

    bool WebResourceCache::RecursiveLoad(const std::string &relative_path,int& maximum_cache_memory){

      std::string application_and_relative_path = root_path_ + relative_path;

      if ( boost::filesystem::exists(application_and_relative_path) ){

        boost::filesystem::directory_iterator end_it;
        boost::filesystem::path path;
        std::string filename;
        std::string extension;
        std::string content_encoding;

        for ( boost::filesystem::directory_iterator it(application_and_relative_path); it != end_it; ++it ){
          content_encoding = "";
          path = it->path();
          filename = path.filename().string();
          if ( boost::filesystem::is_directory(it->status()) ){
            // file is a directory, content cached must be from a file so call recursive load again
            // to cache the files from the directory.
            RecursiveLoad(relative_path + filename + "/", maximum_cache_memory);
          }else{
            // cache the file.
            // but only if its inside the cache memory usage limits
            int file_size = boost::filesystem::file_size(path);
            maximum_cache_memory -= file_size;

            if (maximum_cache_memory > 0){
              if (gzip_content_){
                // remove the .gz extension from the file name if file is compressed.
                // The resultant name will be the identifier of the file
                // client is going to request example.html not example.html.gz.
                int filename_length = filename.length();
                if (filename_length > 3){
                  std::string extension = filename.substr(filename_length-3,filename_length);
                  if (extension == ".gz"){
                    std::string filename_copy(filename);
                    filename_copy = filename_copy.substr(0,filename_copy.length()-3);
                    filename.assign(filename_copy);
                    content_encoding = "gzip";
                  }
                }
              }

              extension = granada::util::file::GetExtension(filename);

              // create a resource
              granada::cache::Resource resource;

              resource.content_type = GetExtensionContentType(extension);

              resource.content_encoding = content_encoding;

              // get the content from the file in form of a vector of unsigned chars.
              std::ifstream ifs(path.string());
              std::vector<unsigned char> content((std::istreambuf_iterator<char>(ifs)),(std::istreambuf_iterator<char>()));
              resource.content = content;

              std::time_t modification_date = boost::filesystem::last_write_time(path);
              resource.last_modified = FormatLastModified(modification_date);

              resource.ETag = GenerateETag(relative_path + filename, resource.last_modified);

              // check if file is a default kind of file, if so
              // we store two additional possible client requests for this file:
              // these are path/to/file/, path/to/file.
              for(auto it = default_files_.as_array().cbegin(); it != default_files_.as_array().cend(); ++it){
				  if (filename == utility::conversions::to_utf8string(it->as_string())){
                  // store path/to/file/

                  CacheRecord(relative_path,resource);

                  // store path/to/file
                  std::string reduced_relative_path(relative_path);
                  reduced_relative_path.erase(reduced_relative_path.end()-1);

                  CacheRecord(reduced_relative_path,resource);
                  break;
                }
              }

              // store path/to/file/default.file
              CacheRecord(relative_path + filename,resource);

            }else{
              break;
            }
          }
        }
      }

      if (files_.empty()){
        return false;
      }

      return true;
    }


    std::string WebResourceCache::GetExtensionContentType(const std::string& extension){
      auto it = content_types_.find(extension);
      if (it != content_types_.end()){
        return it->second;
      }else{
        return granada::util::application::GetProperty("default_content_type");
      }
    }


    std::string WebResourceCache::GetExtensionContentEncoding(const std::string& extension){
      if (gzip_content_){
        for(auto it = gzip_extensions_.as_array().cbegin(); it != gzip_extensions_.as_array().cend(); ++it){
			if (utility::conversions::to_utf8string(it->as_string()) == extension){
            return "gzip";
          }
        }
      }
      return std::string();
    }


    std::string WebResourceCache::FormatLastModified(const std::time_t date){
      #ifdef _WIN32
        std::tm ptm;
        localtime_s(&ptm,&date);
        char buffer[32];
        // Format: Tue, 15 Nov 1994 12:45:26 GMT
        std::strftime(buffer, 32, "%a %d %b %Y %H:%M:%S %Z", &ptm);
      #else
	      std::tm* ptm = std::localtime(&date);
        char buffer[32];
        // Format: Tue, 15 Nov 1994 12:45:26 GMT
        std::strftime(buffer, 32, "%a %d %b %Y %H:%M:%S %Z", ptm);
        delete ptm;
      #endif
      
      return buffer;
    }


    std::string WebResourceCache::GenerateETag(const std::string& resource_path, const std::string& last_modified){
      boost::hash<std::string> string_hash;
      std::size_t hash = string_hash(resource_path+last_modified);
      std::stringstream ss;
      ss << hash;
      return ss.str();
    }
  }
}
