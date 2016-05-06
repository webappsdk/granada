if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(redis-cookie-auth-server
  redis-cookie-auth-server.cc
  ../../src/granada/util/file.cc
  ../../src/granada/util/application.cc
  ../../src/granada/http/parser.cc
  ../../src/granada/cache/web_resource_cache.cc
  ../../src/granada/http/controller/browser_controller.cc
  ../../src/granada/cache/redis_cache_driver.cc
  ../../src/granada/http/session/session.cc
  ../../src/granada/http/session/redis_session_handler.cc
  ../../src/granada/http/session/redis_storage_session.cc
  ../../src/granada/http/session/redis_roles.cc
  src/http/controller/user_controller.cc
  src/http/controller/auth_controller.cc
  src/http/controller/message_controller.cc
  src/business/user.cc
  src/business/message.cc
  )

target_link_libraries(redis-cookie-auth-server ${Casablanca_LIBRARIES})
