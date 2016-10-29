if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(sessions-get-and-post
  sessions-get-and-post.cpp
  ${GRANADA_SOURCE_DIR}/defaults.cpp
  ${GRANADA_SOURCE_DIR}/functions.cpp
  ${GRANADA_SOURCE_DIR}/util/file.cpp
  ${GRANADA_SOURCE_DIR}/http/parser.cpp
  ${GRANADA_SOURCE_DIR}/crypto/nonce_generator.cpp
  ${GRANADA_SOURCE_DIR}/cache/web_resource_cache.cpp
  ${GRANADA_SOURCE_DIR}/http/controller/browser_controller.cpp
  ${GRANADA_SOURCE_DIR}/http/session/session.cpp
  ${GRANADA_SOURCE_DIR}/http/session/map_session.cpp
  ${GRANADA_SOURCE_DIR}/cache/shared_map_cache_driver.cpp
  src/http/controller/test_controller.cpp
  )

target_link_libraries(sessions-get-and-post ${Casablanca_LIBRARIES})
