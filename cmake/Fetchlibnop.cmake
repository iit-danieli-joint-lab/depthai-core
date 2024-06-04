# Copyright (C) 2024 Fondazione Istituto Italiano di Tecnologia
#

include(FetchContent)
FetchContent_Declare(libnop
                     GIT_REPOSITORY https://github.com/google/libnop
                     GIT_TAG master)

FetchContent_GetProperties(libnop)
if(NOT libnop_POPULATED)
  message(STATUS "Fetching libnop.")
  FetchContent_Populate(libnop)
  add_library(libnop)
endif()
  
  
