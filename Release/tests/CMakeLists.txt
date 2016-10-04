set(UnitTestpp_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common/UnitTestpp)
set(Utilities_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/common/utilities/include)

include_directories (${UnitTestpp_INCLUDE_DIR} ${Utilities_INCLUDE_DIR})

add_subdirectory(common)
add_subdirectory(functional)

# Add granada tests
set(GRANADA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/granada")
add_subdirectory(granada)