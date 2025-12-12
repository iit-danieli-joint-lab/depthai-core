include(FetchContent)

message(STATUS "Fetching httplib.")

FetchContent_Declare(
  cpp-httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
  GIT_TAG        v0.18.3
)

FetchContent_MakeAvailable(cpp-httplib)

if(NOT TARGET httplib::httplib)
  add_library(httplib::httplib ALIAS httplib)
endif()
