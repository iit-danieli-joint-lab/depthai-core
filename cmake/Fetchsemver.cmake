include(FetchContent)

message(STATUS "Fetching semver.")

FetchContent_Declare(
  semver
  GIT_REPOSITORY https://github.com/Neargye/semver
  GIT_TAG        v0.3.0
)

FetchContent_MakeAvailable(semver)

if(NOT TARGET semver::semver)
  add_library(semver::semver ALIAS semver)
endif()
