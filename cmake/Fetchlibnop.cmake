# Copyright (C) 2024 Fondazione Istituto Italiano di Tecnologia
#

include(FetchContent)
FetchContent_Declare(libnop
                     GIT_REPOSITORY https://github.com/luxonis/libnop
                     GIT_TAG ab842f51dc2eb13916dc98417c2186b78320ed10)

FetchContent_GetProperties(libnop)
if(NOT libnop_POPULATED)
  message(STATUS "Fetching libnop.")
  FetchContent_MakeAvailable(libnop)
endif()
  
  
