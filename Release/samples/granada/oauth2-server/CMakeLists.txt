if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(oauth2-server
  oauth2-server.cc
  ${GRANADA_SOURCE_DIR}/defaults.cc
  ${GRANADA_SOURCE_DIR}/functions.cc
  ${GRANADA_SOURCE_DIR}/util/file.cc
  ${GRANADA_SOURCE_DIR}/util/application.cc
  ${GRANADA_SOURCE_DIR}/http/parser.cc
  ${GRANADA_SOURCE_DIR}/http/http_msg.cc
  ${GRANADA_SOURCE_DIR}/http/oauth2/oauth2.cc
  ${GRANADA_SOURCE_DIR}/http/oauth2/simple_oauth2.cc
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cc
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cc
  ${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cc
  ${GRANADA_SOURCE_DIR}/cache/map_cache_driver.cc
  ${GRANADA_SOURCE_DIR}/http/session/session.cc
  ${GRANADA_SOURCE_DIR}/http/session/simple_session.cc
  ${GRANADA_SOURCE_DIR}/http/session/shared_map_session_handler.cc
  ${GRANADA_SOURCE_DIR}/http/session/map_roles.cc
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cc
  ${GRANADA_SOURCE_DIR}/http/controller/oauth2_controller.cc
  src/http/controller/client_controller.cc
  src/http/controller/user_controller.cc
  src/http/controller/message_controller.cc
  src/http/controller/application_controller.cc
  src/business/message.cc
  )

add_executable(redis-oauth2-server
  redis-oauth2-server.cc
  ${GRANADA_SOURCE_DIR}/defaults.cc
  ${GRANADA_SOURCE_DIR}/functions.cc
  ${GRANADA_SOURCE_DIR}/util/file.cc
  ${GRANADA_SOURCE_DIR}/util/application.cc
  ${GRANADA_SOURCE_DIR}/http/parser.cc
  ${GRANADA_SOURCE_DIR}/http/http_msg.cc
  ${GRANADA_SOURCE_DIR}/http/oauth2/oauth2.cc
  ${GRANADA_SOURCE_DIR}/http/oauth2/redis_oauth2.cc
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cc
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cc
  ${GRANADA_SOURCE_DIR}/cache/redis_cache_driver.cc
  ${GRANADA_SOURCE_DIR}/http/session/session.cc
  ${GRANADA_SOURCE_DIR}/http/session/redis_storage_session.cc
  ${GRANADA_SOURCE_DIR}/http/session/redis_session_handler.cc
  ${GRANADA_SOURCE_DIR}/http/session/redis_roles.cc
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cc
  ${GRANADA_SOURCE_DIR}/http/controller/oauth2_controller.cc
  src/http/controller/client_controller.cc
  src/http/controller/user_controller.cc
  src/http/controller/message_controller.cc
  src/http/controller/application_controller.cc
  src/business/message.cc
  )

target_link_libraries(oauth2-server ${Casablanca_LIBRARIES})

target_link_libraries(redis-oauth2-server ${Casablanca_LIBRARIES})
