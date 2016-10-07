if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

include_directories(/opt/mozjs-38.0.0/js/src/build_OPT.OBJ/dist/include)
link_directories(/opt/mozjs-38.0.0/js/src/build_OPT.OBJ/dist/lib)

add_executable(plugin-server
  plugin-server.cc
  ${GRANADA_SOURCE_DIR}/defaults.cc
  ${GRANADA_SOURCE_DIR}/functions.cc
  ${GRANADA_SOURCE_DIR}/util/file.cc
  ${GRANADA_SOURCE_DIR}/util/application.cc
  ${GRANADA_SOURCE_DIR}/http/parser.cc
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cc
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cc
  ${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cc
  ${GRANADA_SOURCE_DIR}/runner/spidermonkey_javascript_runner.cc
  ${GRANADA_SOURCE_DIR}/plugin/plugin.cc
  ${GRANADA_SOURCE_DIR}/plugin/spidermonkey_plugin.cc
  ${GRANADA_SOURCE_DIR}/plugin/map_spidermonkey_plugin.cc
  ${GRANADA_SOURCE_DIR}/http/session/session.cc
  ${GRANADA_SOURCE_DIR}/http/session/map_session.cc
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cc
  ${GRANADA_SOURCE_DIR}/http/controller/plugin_controller.cc
)

target_link_libraries(plugin-server ${Casablanca_LIBRARIES} -ljs_static -lz -lpthread -ldl)

add_executable(plugin-server-redis
  plugin-server-redis.cc
  ${GRANADA_SOURCE_DIR}/defaults.cc
  ${GRANADA_SOURCE_DIR}/functions.cc
  ${GRANADA_SOURCE_DIR}/util/file.cc
  ${GRANADA_SOURCE_DIR}/util/application.cc
  ${GRANADA_SOURCE_DIR}/http/parser.cc
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cc
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cc
  ${GRANADA_SOURCE_DIR}/cache/redis_cache_driver.cc
  ${GRANADA_SOURCE_DIR}/runner/spidermonkey_javascript_runner.cc
  ${GRANADA_SOURCE_DIR}/plugin/plugin.cc
  ${GRANADA_SOURCE_DIR}/plugin/spidermonkey_plugin.cc
  ${GRANADA_SOURCE_DIR}/plugin/redis_spidermonkey_plugin.cc
  ${GRANADA_SOURCE_DIR}/http/session/session.cc
  ${GRANADA_SOURCE_DIR}/http/session/redis_session.cc
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cc
  ${GRANADA_SOURCE_DIR}/http/controller/plugin_controller.cc
)

target_link_libraries(plugin-server-redis ${Casablanca_LIBRARIES} -ljs_static -lz -lpthread -ldl)
