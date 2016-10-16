if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(oauth2-server
  oauth2-server.cpp
  ${GRANADA_SOURCE_DIR}/defaults.cpp
  ${GRANADA_SOURCE_DIR}/functions.cpp
  ${GRANADA_SOURCE_DIR}/util/file.cpp
  ${GRANADA_SOURCE_DIR}/util/application.cpp
  ${GRANADA_SOURCE_DIR}/http/parser.cpp
  ${GRANADA_SOURCE_DIR}/http/http_msg.cpp
  ${GRANADA_SOURCE_DIR}/http/oauth2/oauth2.cpp
  ${GRANADA_SOURCE_DIR}/http/oauth2/map_oauth2.cpp
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cpp
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cpp
  ${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cpp
  ${GRANADA_SOURCE_DIR}/http/session/session.cpp
  ${GRANADA_SOURCE_DIR}/http/session/map_session.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/oauth2_controller.cpp
  src/http/controller/client_controller.cpp
  src/http/controller/user_controller.cpp
  src/http/controller/message_controller.cpp
  src/http/controller/application_controller.cpp
  src/business/message.cpp
  )

add_executable(redis-oauth2-server
  redis-oauth2-server.cpp
  ${GRANADA_SOURCE_DIR}/defaults.cpp
  ${GRANADA_SOURCE_DIR}/functions.cpp
  ${GRANADA_SOURCE_DIR}/util/file.cpp
  ${GRANADA_SOURCE_DIR}/util/application.cpp
  ${GRANADA_SOURCE_DIR}/http/parser.cpp
  ${GRANADA_SOURCE_DIR}/http/http_msg.cpp
  ${GRANADA_SOURCE_DIR}/http/oauth2/oauth2.cpp
  ${GRANADA_SOURCE_DIR}/http/oauth2/redis_oauth2.cpp
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cpp
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cpp
  ${GRANADA_SOURCE_DIR}/cache/redis_cache_driver.cpp
  ${GRANADA_SOURCE_DIR}/http/session/session.cpp
  ${GRANADA_SOURCE_DIR}/http/session/redis_session.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/oauth2_controller.cpp
  src/http/controller/client_controller.cpp
  src/http/controller/user_controller.cpp
  src/http/controller/message_controller.cpp
  src/http/controller/application_controller.cpp
  src/business/message.cpp
  )

target_link_libraries(oauth2-server ${Casablanca_LIBRARIES})

target_link_libraries(redis-oauth2-server ${Casablanca_LIBRARIES})
