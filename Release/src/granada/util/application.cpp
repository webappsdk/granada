
#include "granada/util/application.h"

namespace granada{
  namespace util{
    namespace application{
		const std::string& get_selfpath(){
	        if (selfpath.empty()){
	          #ifdef __APPLE__
	        	int ret;
				pid_t pid; 
				char pathbuf[PROC_PIDPATHINFO_MAXSIZE];
				pid = getpid();
				ret = proc_pidpath (pid, pathbuf, sizeof(pathbuf));
				if ( ret > 0 ) {
					std::string totalPath = std::string(pathbuf);
					selfpath = totalPath.substr(0,totalPath.find_last_of("/"));
				}
	          #elif _WIN32
				std::vector<wchar_t> pathBuf;
				DWORD copied = 0;

				do {
					pathBuf.resize(pathBuf.size() + MAX_PATH);
					copied = GetModuleFileName(0, (PWSTR)&pathBuf.at(0), pathBuf.size());
				} while (copied >= pathBuf.size());

				pathBuf.resize(copied);
				selfpath = utility::conversions::to_utf8string(std::wstring(pathBuf.begin(), pathBuf.end()));
				selfpath = selfpath.substr(0, selfpath.find_last_of("\\"));
	          #else
	            char buff[PATH_MAX];
	            ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
	            if (len != -1) {
	              buff[len] = '\0';
	              std::string totalPath = std::string(buff);
	              selfpath = totalPath.substr(0,totalPath.find_last_of("/"));
	            }
	          #endif
	        }
	        return selfpath;
	    }


      const std::string GetProperty(const std::string& name){
        if (property_file_ == NULL){
          std::string configuration_file_path = get_selfpath() + "/server.conf";
          property_file_ = std::unique_ptr<granada::util::file::PropertyFile>(new granada::util::file::PropertyFile(configuration_file_path));
        }
        return property_file_->GetProperty(name);
      }
    }
  }
}