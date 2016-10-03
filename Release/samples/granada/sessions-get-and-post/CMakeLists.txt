if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(sessions-get-and-post
  sessions-get-and-post.cc
  ${GRANADA_SOURCE_DIR}/defaults.cc
  ${GRANADA_SOURCE_DIR}/functions.cc
  ${GRANADA_SOURCE_DIR}/util/file.cc
  ${GRANADA_SOURCE_DIR}/util/application.cc
  ${GRANADA_SOURCE_DIR}/http/parser.cc
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cc
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cc
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cc
  ${GRANADA_SOURCE_DIR}/http/session/session.cc
  ${GRANADA_SOURCE_DIR}/http/session/storage_session.cc
  ${GRANADA_SOURCE_DIR}/http/session/map_roles.cc
  ${GRANADA_SOURCE_DIR}/http/session/shared_map_session_handler.cc
  ${GRANADA_SOURCE_DIR}/cache/map_cache_driver.cc
  src/http/controller/test_controller.cc
  )

target_link_libraries(sessions-get-and-post ${Casablanca_LIBRARIES})
