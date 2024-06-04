# Copyright (C) 2024 Fondazione Istituto Italiano di Tecnologia
#

include(FetchContent)
FetchContent_Declare(FP16
                     GIT_REPOSITORY https://github.com/Maratyszcza/FP16
                     GIT_TAG master)

FetchContent_GetProperties(FP16)
if(NOT FP16_POPULATED)
  message(STATUS "Fetching FP16.")
  FetchContent_Populate(FP16)
  set(fp16_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(fp16_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
  add_subdirectory(${fp16_SOURCE_DIR} ${fp16_BINARY_DIR})
endif()
  
  
