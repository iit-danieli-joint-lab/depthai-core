include(FetchContent)

message(STATUS "Fetching mp4v2.")

FetchContent_Declare(
  mp4v2
  GIT_REPOSITORY https://github.com/TechSmith/mp4v2
  GIT_TAG        3918d0afb7fe1259b8930d682487898b1c6da5e1
)

FetchContent_MakeAvailable(mp4v2)

if(NOT TARGET mp4v2::mp4v2)
  if(TARGET mp4v2)
    add_library(mp4v2::mp4v2 ALIAS mp4v2)
  endif()
endif()
