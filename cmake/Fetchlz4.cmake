include(FetchContent)

message(STATUS "Fetching lz4.")

FetchContent_Declare(
  lz4
  GIT_REPOSITORY https://github.com/lz4/lz4
  GIT_TAG        v1.9.4
  SOURCE_SUBDIR  build/cmake
)

FetchContent_MakeAvailable(lz4)

if(NOT TARGET lz4::lz4)
  if(TARGET lz4_static)
    add_library(lz4::lz4 ALIAS lz4_static)
  elseif(TARGET lz4_shared)
    add_library(lz4::lz4 ALIAS lz4_shared)
  endif()
endif()
