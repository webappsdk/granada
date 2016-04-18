#pragma once
#ifdef WINDOWS
  #include <limits>
#else
  #include <linux/limits.h>
  #include <unistd.h>
#endif
#include <memory>
#include <string>
#include "file.h"

namespace granada{
  namespace util{
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
      std::string get_selfpath();


      /**
       * Returns the value of a property of the application config file.
       * @param  name Name of the property to retrieve.
       * @return      Value of the property.
       */
      std::string GetProperty(const std::string& name);
    }
  }
}
