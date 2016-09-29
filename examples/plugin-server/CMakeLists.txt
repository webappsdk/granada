if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

include_directories({PATH_TO_MOZJS}/mozjs-38.0.0/js/src/build_OPT.OBJ/dist/include)
link_directories({PATH_TO_MOZJS}/mozjs-38.0.0/js/src/build_OPT.OBJ/dist/lib)

add_executable(plugin-server
  plugin-server.cc
  ../../src/granada/defaults.cc
  ../../src/granada/functions.cc
  ../../src/granada/util/file.cc
  ../../src/granada/util/application.cc
  ../../src/granada/http/parser.cc
  ../../src/granada/crypto/nonce_generator.cc
  ../../src/granada/cache/web_resource_cache.cc
  ../../src/granada/cache/shared_map_cache_driver.cc
  ../../src/granada/runner/spidermonkey_javascript_runner.cc
  ../../src/granada/plugin/plugin.cc
  ../../src/granada/plugin/spidermonkey_plugin.cc
  ../../src/granada/plugin/map_spidermonkey_plugin.cc
  ../../src/granada/http/session/session.cc
  ../../src/granada/http/session/simple_session.cc
  ../../src/granada/http/session/shared_map_session_handler.cc
  ../../src/granada/http/session/map_roles.cc
  ../../src/granada/http/controller/browser_controller.cc
  ../../src/granada/http/controller/plugin_controller.cc
)

target_link_libraries(plugin-server ${Casablanca_LIBRARIES} -ljs_static -lz -lpthread -ldl)

add_executable(plugin-server-redis
  plugin-server-redis.cc
  ../../src/granada/defaults.cc
  ../../src/granada/functions.cc
  ../../src/granada/util/file.cc
  ../../src/granada/util/application.cc
  ../../src/granada/http/parser.cc
  ../../src/granada/crypto/nonce_generator.cc
  ../../src/granada/cache/web_resource_cache.cc
  ../../src/granada/cache/redis_cache_driver.cc
  ../../src/granada/runner/spidermonkey_javascript_runner.cc
  ../../src/granada/plugin/plugin.cc
  ../../src/granada/plugin/spidermonkey_plugin.cc
  ../../src/granada/plugin/redis_spidermonkey_plugin.cc
  ../../src/granada/http/session/session.cc
  ../../src/granada/http/session/redis_storage_session.cc
  ../../src/granada/http/session/redis_session_handler.cc
  ../../src/granada/http/session/redis_roles.cc
  ../../src/granada/http/controller/browser_controller.cc
  ../../src/granada/http/controller/plugin_controller.cc
)

target_link_libraries(plugin-server-redis ${Casablanca_LIBRARIES} -ljs_static -lz -lpthread -ldl)
