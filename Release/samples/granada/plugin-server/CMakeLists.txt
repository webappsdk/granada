if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

include_directories(/opt/mozjs-38.0.0/js/src/build_OPT.OBJ/dist/include)
link_directories(/opt/mozjs-38.0.0/js/src/build_OPT.OBJ/dist/lib)

add_executable(plugin-server
  plugin-server.cpp
  ${GRANADA_SOURCE_DIR}/defaults.cpp
  ${GRANADA_SOURCE_DIR}/functions.cpp
  ${GRANADA_SOURCE_DIR}/util/file.cpp
  ${GRANADA_SOURCE_DIR}/http/parser.cpp
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cpp
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cpp
  ${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cpp
  ${GRANADA_SOURCE_DIR}/runner/spidermonkey_javascript_runner.cpp
  ${GRANADA_SOURCE_DIR}/plugin/plugin.cpp
  ${GRANADA_SOURCE_DIR}/plugin/spidermonkey_plugin.cpp
  ${GRANADA_SOURCE_DIR}/plugin/map_spidermonkey_plugin.cpp
  ${GRANADA_SOURCE_DIR}/http/session/session.cpp
  ${GRANADA_SOURCE_DIR}/http/session/map_session.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/plugin_controller.cpp
)

target_link_libraries(plugin-server ${Casablanca_LIBRARIES} -ljs_static -lz -lpthread -ldl)

add_executable(plugin-server-redis
  plugin-server-redis.cpp
  ${GRANADA_SOURCE_DIR}/defaults.cpp
  ${GRANADA_SOURCE_DIR}/functions.cpp
  ${GRANADA_SOURCE_DIR}/util/file.cpp
  ${GRANADA_SOURCE_DIR}/http/parser.cpp
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cpp
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cpp
  ${GRANADA_SOURCE_DIR}/cache/redis_cache_driver.cpp
  ${GRANADA_SOURCE_DIR}/runner/spidermonkey_javascript_runner.cpp
  ${GRANADA_SOURCE_DIR}/plugin/plugin.cpp
  ${GRANADA_SOURCE_DIR}/plugin/spidermonkey_plugin.cpp
  ${GRANADA_SOURCE_DIR}/plugin/redis_spidermonkey_plugin.cpp
  ${GRANADA_SOURCE_DIR}/http/session/session.cpp
  ${GRANADA_SOURCE_DIR}/http/session/redis_session.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/plugin_controller.cpp
)

target_link_libraries(plugin-server-redis ${Casablanca_LIBRARIES} -ljs_static -lz -lpthread -ldl)
