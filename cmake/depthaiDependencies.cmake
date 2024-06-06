if(CONFIG_MODE)
    set(_DEPTHAI_PREFIX_PATH_ORIGINAL ${CMAKE_PREFIX_PATH})
    set(_DEPTHAI_MODULE_PATH_ORIGINAL ${CMAKE_MODULE_PATH})
    set(_DEPTHAI_FIND_ROOT_PATH_MODE_PACKAGE_ORIGINAL ${CMAKE_FIND_ROOT_PATH_MODE_PACKAGE})
    # Fixes Android NDK build, where prefix path is ignored as its not inside sysroot
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE "BOTH")
    # Sets where to search for packages about to follow
    set(CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/${_IMPORT_PREFIX}" ${CMAKE_PREFIX_PATH})
    set(_QUIET "QUIET")
else()
    set(DEPTHAI_SHARED_LIBS ${BUILD_SHARED_LIBS})
    hunter_add_package(nlohmann_json)
    if(NOT DEPTHAI_XLINK_LOCAL)
        hunter_add_package(XLink)
    endif()
    hunter_add_package(BZip2)
    hunter_add_package(FP16)
    hunter_add_package(libarchive-luxonis)
    hunter_add_package(spdlog)
    hunter_add_package(ZLIB)
    if(DEPTHAI_ENABLE_BACKWARD)
        hunter_add_package(Backward)
    endif()
    hunter_add_package(libnop)
    if(DEPTHAI_PCL_SUPPORT)
        hunter_add_package(jsoncpp)
    endif()
endif()

# If library was build as static, find all dependencies
if(NOT CONFIG_MODE OR (CONFIG_MODE AND NOT DEPTHAI_SHARED_LIBS))

    # BZip2 (for bspatch)
    #find_package(BZip2 ${_QUIET} CONFIG REQUIRED)
    find_package(BZip2 REQUIRED)

    # FP16 for conversions
    #find_package(FP16 ${_QUIET} CONFIG REQUIRED)
    #find_package(FP16 REQUIRED)
    include(FetchFP16)

    # libarchive for firmware packages
    #find_package(archive_static ${_QUIET} CONFIG REQUIRED)
    #find_package(lzma ${_QUIET} CONFIG REQUIRED)
    find_package(LibArchive REQUIRED)
    find_package(LibLZMA REQUIRED)


    # ZLIB for compressing Apps
    #find_package(ZLIB CONFIG REQUIRED)
    find_package(ZLIB REQUIRED)    

    # spdlog for library and device logging
    #find_package(spdlog ${_QUIET} CONFIG REQUIRED)
    find_package(spdlog REQUIRED)

    # Backward
    if(DEPTHAI_ENABLE_BACKWARD)
        # Disable automatic check for additional stack unwinding libraries
        # Just use the default compiler one
        set(STACK_DETAILS_AUTO_DETECT FALSE CACHE BOOL "Auto detect backward's stack details dependencies")
        find_package(Backward ${_QUIET} CONFIG REQUIRED)
        unset(STACK_DETAILS_AUTO_DETECT)
    endif()

endif()

# Add threads (c++)
find_package(Threads ${_QUIET} REQUIRED)

# Nlohmann JSON
find_package(nlohmann_json 3.6.0 ${_QUIET} CONFIG REQUIRED)

# libnop for serialization
include(Fetchlibnop)
#find_package(libnop ${_QUIET} CONFIG REQUIRED)

# XLink
if(DEPTHAI_XLINK_LOCAL AND (NOT CONFIG_MODE))
    set(_BUILD_SHARED_LIBS_SAVED "${BUILD_SHARED_LIBS}")
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory("${DEPTHAI_XLINK_LOCAL}" ${CMAKE_CURRENT_BINARY_DIR}/XLink)
    set(BUILD_SHARED_LIBS "${_BUILD_SHARED_LIBS_SAVED}")
    unset(_BUILD_SHARED_LIBS_SAVED)
    list(APPEND targets_to_export XLink)
else()
    FetchContent_Declare(
      XLink
      GIT_REPOSITORY https://github.com/luxonis/XLink
      GIT_TAG        e9eb1ef38030176ad70cddd3b545d5e6c509f1e1 # release-1.10.0
    )
    set(XLINK_ENABLE_LIBUSB ${XLINK_ENABLE_LIBUSB} CACHE BOOL "" FORCE)
    set(XLINK_LIBUSB_SYSTEM ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(XLink)
    #find_package(XLink ${_QUIET} CONFIG REQUIRED HINTS "${CMAKE_CURRENT_LIST_DIR}/XLink" "${CMAKE_CURRENT_LIST_DIR}/../XLink")
endif()

# OpenCV 4 - (optional, quiet always)
find_package(OpenCV 4 QUIET CONFIG)
if(DEPTHAI_PCL_SUPPORT AND NOT TARGET JsonCpp::JsonCpp)
    find_package(jsoncpp)
endif()
set(MODULE_TEMP ${CMAKE_MODULE_PATH})
set(PREFIX_TEMP ${CMAKE_PREFIX_PATH})
set(CMAKE_MODULE_PATH ${_DEPTHAI_MODULE_PATH_ORIGINAL})
set(CMAKE_PREFIX_PATH ${_DEPTHAI_PREFIX_PATH_ORIGINAL})
if(DEPTHAI_PCL_SUPPORT)
    find_package(PCL CONFIG COMPONENTS common visualization)
endif()
set(CMAKE_MODULE_PATH ${MODULE_TEMP})
set(CMAKE_PREFIX_PATH ${PREFIX_TEMP})

# include optional dependency cmake
if(DEPTHAI_DEPENDENCY_INCLUDE)
    include(${DEPTHAI_DEPENDENCY_INCLUDE} OPTIONAL)
endif()

# Cleanup
if(CONFIG_MODE)
    set(CMAKE_PREFIX_PATH ${_DEPTHAI_PREFIX_PATH_ORIGINAL})
    set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ${_DEPTHAI_FIND_ROOT_PATH_MODE_PACKAGE_ORIGINAL})
    unset(_DEPTHAI_PREFIX_PATH_ORIGINAL)
    unset(_DEPTHAI_FIND_ROOT_PATH_MODE_PACKAGE_ORIGINAL)
    unset(_QUIET)
else()
    set(DEPTHAI_SHARED_LIBS)
endif()
