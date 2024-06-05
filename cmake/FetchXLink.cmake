# Copyright (C) 2024 Fondazione Istituto Italiano di Tecnologia
#

include(FetchContent)
FetchContent_Declare(XLink
                     GIT_REPOSITORY https://github.com/luxonis/XLink
                     GIT_TAG master)

FetchContent_GetProperties(XLink)
if(NOT XLink_POPULATED)
  message(STATUS "Fetching XLink.")
  FetchContent_Populate(XLink)
  set(usb-1.0_DIR ${CMAKE_SOURCE_DIR}/cmake/usb-1.0)
  set(XLINK_LIBSUB_SYSTEM ON CACHE BOOL "" FORCE)
  add_subdirectory(${xlink_SOURCE_DIR} ${xlink_BINARY_DIR})
endif()
  
  
