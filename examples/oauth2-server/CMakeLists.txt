if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(oauth2-server
  oauth2-server.cc
  ../../src/granada/defaults.cc
  ../../src/granada/util/file.cc
  ../../src/granada/util/application.cc
  ../../src/granada/http/parser.cc
  ../../src/granada/http/http_msg.cc
  ../../src/granada/http/oauth2/oauth2.cc
  ../../src/granada/http/oauth2/simple_oauth2.cc
  ../../src/granada/crypto/nonce_generator.cc
  ../../src/granada/cache/web_resource_cache.cc
  ../../src/granada/cache/shared_map_cache_driver.cc
  ../../src/granada/cache/map_cache_driver.cc
  ../../src/granada/http/session/session.cc
  ../../src/granada/http/session/simple_session.cc
  ../../src/granada/http/session/shared_map_session_handler.cc
  ../../src/granada/http/session/map_roles.cc
  ../../src/granada/http/controller/browser_controller.cc
  ../../src/granada/http/controller/oauth2_controller.cc
  src/http/controller/client_controller.cc
  src/http/controller/user_controller.cc
  src/http/controller/message_controller.cc
  src/http/controller/application_controller.cc
  src/business/message.cc
  )

add_executable(redis-oauth2-server
  redis-oauth2-server.cc
  ../../src/granada/defaults.cc
  ../../src/granada/util/file.cc
  ../../src/granada/util/application.cc
  ../../src/granada/http/parser.cc
  ../../src/granada/http/http_msg.cc
  ../../src/granada/http/oauth2/oauth2.cc
  ../../src/granada/http/oauth2/redis_oauth2.cc
  ../../src/granada/crypto/nonce_generator.cc
  ../../src/granada/cache/web_resource_cache.cc
  ../../src/granada/cache/redis_cache_driver.cc
  ../../src/granada/http/session/session.cc
  ../../src/granada/http/session/redis_storage_session.cc
  ../../src/granada/http/session/redis_session_handler.cc
  ../../src/granada/http/session/redis_roles.cc
  ../../src/granada/http/controller/browser_controller.cc
  ../../src/granada/http/controller/oauth2_controller.cc
  src/http/controller/client_controller.cc
  src/http/controller/user_controller.cc
  src/http/controller/message_controller.cc
  src/http/controller/application_controller.cc
  src/business/message.cc
  )

target_link_libraries(oauth2-server ${Casablanca_LIBRARIES})

target_link_libraries(redis-oauth2-server ${Casablanca_LIBRARIES})
