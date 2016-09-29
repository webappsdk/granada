Use CMake to build the project. After doing a “make” two executables will be created in the same folder where the server.conf file is: the "plugin-server" executable will use maps for data storage and the other executable "plugin-server-redis" will use redis as data storage system.

In the CMakeList.txt of the example substitute {PATH_TO_MOZJS} by the real path to your mozjs library.

After executing the server use a browser and type the url: http://localhost to use the client application. You can configure the server using “server.conf” file.