if (UNIX)
  add_definitions(-Wno-sign-compare -Wno-enum-compare)
endif()

add_executable(sessions-get-and-post
  sessions-get-and-post.cc
  ../../src/granada/util/file.cc
  ../../src/granada/util/application.cc
  ../../src/granada/http/parser.cc
  ../../src/granada/cache/web_resource_cache.cc
  ../../src/granada/http/controller/browser_controller.cc
  ../../src/granada/http/session/session.cc
  ../../src/granada/http/session/storage_session.cc
  ../../src/granada/http/session/map_roles.cc
  ../../src/granada/http/session/shared_map_session_handler.cc
  ../../src/granada/cache/map_cache_driver.cc
  src/http/controller/test_controller.cc
  )

target_link_libraries(sessions-get-and-post ${Casablanca_LIBRARIES})
