# Copyright (C) 2024 Fondazione Istituto Italiano di Tecnologia

set(ENV{CMAKE_POLICY_VERSION_MINIMUM} 3.5)
include(FetchContent)

message(STATUS "Fetching FP16.")
set(FP16_BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(FP16_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(FP16
                     GIT_REPOSITORY https://github.com/luxonis/FP16
                     # From https://github.com/luxonis/depthai-core/blob/v2.27.0/cmake/Hunter/config.cmake#L74
                     GIT_TAG c911175d2717e562976e606c6e5f799bf40cf94e)

FetchContent_MakeAvailable(FP16)

if(TARGET fp16 AND NOT TARGET FP16::fp16)
    add_library(FP16::fp16 ALIAS fp16)
endif()

if(NOT FP16_INCLUDE_DIR AND TARGET FP16::fp16)
    get_target_property(FP16_INCLUDE_DIR FP16::fp16 INTERFACE_INCLUDE_DIRECTORIES)
endif()

