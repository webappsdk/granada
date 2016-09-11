#pragma once
#include <memory>
#include <string>
#ifdef __APPLE__
  #include <libproc.h>
  #include <unistd.h>
#elif WINDOWS
  #include <limits>
#else
  #include <linux/limits.h>
#endif
#include "file.h"

namespace granada{
  namespace util{

    /**
     * Utils for application.
     */
    namespace application{

      /**
       * Path of the application.
       */
      static std::string selfpath;


      /**
       * Application property file.
       */
      static std::unique_ptr<granada::util::file::PropertyFile> property_file_;


      /**
       * Returns the path of the application.
       * @return Path.
       */
      const std::string& get_selfpath();


      /**
       * Returns the value of a property of the application config file.
       * @param  name Name of the property to retrieve.
       * @return      Value of the property.
       */
      const std::string GetProperty(const std::string& name);


      /**
       * Homogenize a given path in the form of string to a format like: /path/to/directory
       * Example: /my/path  =>  will stay as it is, the / at the begining is interpreted as if
       *                        we had been given a complete path.
       *          my/path   =>  /path/to/application/directory/my/path The absence of / at the
       *                        begining is interpreted as if we had been given a relative path.
       *          /my/path/ => /my/path   Last / is always removed.
       */
      static const std::string FormatDirectoryPath(const std::string& directory_path){
        std::string formatted_directory_path = directory_path;
        if (!directory_path.empty()){

          // if the repository path ends with a slash remove it.
          if (formatted_directory_path.back() == '/'){
            formatted_directory_path.pop_back();
          }
          
          // if the repository does not start with a slash
          // we supose it is a relative path
          if (formatted_directory_path[0] != '/'){

            // complete the path adding the application directory path before the given relative path.
            formatted_directory_path = granada::util::application::get_selfpath() + "/" + formatted_directory_path;
          }
        }

        return formatted_directory_path;
      };
    }
  }
}