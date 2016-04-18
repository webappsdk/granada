if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(sessions-login-and-cart
  sessions-login-and-cart.cc
  ../../src/granada/util/file.cc
  ../../src/granada/util/application.cc
  ../../src/granada/http/parser.cc
  ../../src/granada/http/session/session.cc
  ../../src/granada/http/session/simple_session.cc
  ../../src/granada/http/session/shared_map_session_handler.cc
  ../../src/granada/cache/web_resource_cache.cc
  ../../src/granada/http/controller/browser_controller.cc
  ../../src/granada/http/session/map_roles.cc
  ../../src/granada/cache/shared_map_cache_driver.cc
  src/http/controller/auth_controller.cc
  src/http/controller/cart_controller.cc
  src/business/cart.cc
  )

target_link_libraries(sessions-login-and-cart ${Casablanca_LIBRARIES})
