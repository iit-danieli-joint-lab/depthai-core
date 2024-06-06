# Copyright (C) 2024 Fondazione Istituto Italiano di Tecnologia
#

include(FetchContent)
FetchContent_Declare(FP16
                     GIT_REPOSITORY https://github.com/luxonis/FP16
                     GIT_TAG c911175d2717e562976e606c6e5f799bf40cf94e)

FetchContent_GetProperties(FP16)
if(NOT FP16_POPULATED)
  message(STATUS "Fetching FP16.")
  FetchContent_Populate(FP16)
  set(fp16_BUILD_TESTS OFF CACHE BOOL "" FORCE)
  set(fp16_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
  add_subdirectory(${fp16_SOURCE_DIR} ${fp16_BINARY_DIR})
  if(NOT TARGET FP16::fp16)
    add_library( FP16::fp16 ALIAS fp16)
  endif()
endif()
  
  
