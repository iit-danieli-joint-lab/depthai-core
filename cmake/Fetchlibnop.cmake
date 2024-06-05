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
  add_library(libnop INTERFACE IMPORTED)
  set_target_properties(libnop PROPERTIES
    INTERFACE_COMPILE_FEATURES "cxx_std_14"
    INTERFACE_INCLUDE_DIRECTORIES "${libnop_SOURCE_DIR}/include")
  #add_library(libnop::libnop ALIAS libnop)  
  #target_include_directories(libnop INTERFACE ${libnop_SOURCE_DIR}/include)
  #message("${libnop_SOURCE_DIR}")
endif()
  
  
