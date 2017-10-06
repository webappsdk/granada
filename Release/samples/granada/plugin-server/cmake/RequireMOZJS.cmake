# Find the SpiderMonkey library (mozjs)
#
# This module defines
# MOZJS_INCLUDE_DIR
# MOZJS_LIBRARY
# MOZJS_FOUND

FILE(GLOB _MOZJS_INCLUDE_DIRS /usr/include/mozjs*)

FIND_PATH(MOZJS_INCLUDE_DIR jsapi.h
  PATHS
  /usr/local
  /usr
  ${_MOZJS_INCLUDE_DIRS}
  $ENV{MOZJS_DIR}
  PATH_SUFFIXES include include-unix
)

FIND_LIBRARY(MOZJS_LIBRARY
  NAMES mozjs mozjs-38 mozjs-24 mozjs185
  PATHS
  /usr/local
  /usr
  $ENV{MOZJS_DIR}
  PATH_SUFFIXES lib64 lib lib/shared lib/static lib64/static
)

SET(MOZJS_FOUND "NO")
IF(MOZJS_LIBRARY AND MOZJS_INCLUDE_DIR)
  MESSAGE(STATUS "MOZJS found")
  SET(MOZJS_FOUND "YES")
ENDIF(MOZJS_LIBRARY AND MOZJS_INCLUDE_DIR)

if (MOZJS_FOUND)
  include_directories(${MOZJS_INCLUDE_DIR})
else(MOZJS_FOUND)
  MESSAGE(FATAL_ERROR "MOZJS library is required")
endif(MOZJS_FOUND)
