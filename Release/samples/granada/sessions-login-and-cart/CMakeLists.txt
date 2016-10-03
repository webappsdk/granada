if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(sessions-login-and-cart
  sessions-login-and-cart.cc
  ${GRANADA_SOURCE_DIR}/defaults.cc
  ${GRANADA_SOURCE_DIR}/functions.cc
  ${GRANADA_SOURCE_DIR}/util/file.cc
  ${GRANADA_SOURCE_DIR}/util/application.cc
  ${GRANADA_SOURCE_DIR}/http/parser.cc
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cc
  ${GRANADA_SOURCE_DIR}/http/session/session.cc
  ${GRANADA_SOURCE_DIR}/http/session/simple_session.cc
  ${GRANADA_SOURCE_DIR}/http/session/shared_map_session_handler.cc
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cc
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cc
  ${GRANADA_SOURCE_DIR}/http/session/map_roles.cc
  ${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cc
  src/http/controller/auth_controller.cc
  src/http/controller/cart_controller.cc
  src/business/cart.cc
  )

target_link_libraries(sessions-login-and-cart ${Casablanca_LIBRARIES})
