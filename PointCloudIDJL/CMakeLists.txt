# Add an option to test examples
option(DEPTHAI_TEST_EXAMPLES "Test examples - examples will be ran as a part of test suite" OFF)

# Dependencies
find_package(OpenCV REQUIRED)
find_package(Sanitizers)

# Create utility library
add_library(utility utility/utility.cpp)
target_include_directories(utility PUBLIC "utility")
add_default_flags(utility LEAN)
target_link_libraries(utility FP16::fp16 ${OpenCV_LIBS})

# Create environments for usb & poe labels
set(test_usb_env
    # Misc
    "UBSAN_OPTIONS=halt_on_error=1"
    # DepthAI
    "DEPTHAI_PROTOCOL=usb"
)
set(test_poe_env
    # Misc
    "UBSAN_OPTIONS=halt_on_error=1"
    # DepthAI
    "DEPTHAI_PROTOCOL=tcpip;DEPTHAI_SEARCH_TIMEOUT=15000"
)

# Helper for adding new examples
function(dai_add_example example_name example_src enable_test use_pcl)

    # Add example
    add_executable(${example_name} ${example_src})
    add_default_flags(${example_name} LEAN)
    if(use_pcl)
        target_link_libraries(${example_name} PRIVATE utility depthai::core ${OpenCV_LIBS} Threads::Threads)
    else()
        target_link_libraries(${example_name} PRIVATE utility depthai::opencv ${OpenCV_LIBS} Threads::Threads)
    endif()
    # Set compiler features (c++14), and disables extensions (g++14)
    set_property(TARGET ${example_name} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${example_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    set_property(TARGET ${example_name} PROPERTY CXX_EXTENSIONS OFF)

    # Add sanitizers for example
    if(COMMAND add_sanitizers)
        add_sanitizers(${example_name})
    endif()
    # Add to clangformat target
    if(COMMAND target_clangformat_setup)
        target_clangformat_setup(${example_name} "")
    endif()

    # parse the rest of the arguments
    set(arguments ${ARGV})
    list(REMOVE_AT arguments 0 1 2 3)

    # If 'DEPTHAI_TEST_EXAMPLES' is ON, then examples will be part of the test suite
    if(${enable_test} AND DEPTHAI_TEST_EXAMPLES)
        add_test(NAME ${example_name} COMMAND ${CMAKE_COMMAND}
            -DTIMEOUT_SECONDS=10
            -P ${CMAKE_CURRENT_LIST_DIR}/cmake/ExecuteTestTimeout.cmake $<TARGET_FILE:${example_name}> ${arguments}
        )

        # Sets a regex catching any logged warnings, errors or critical (coming either from device or host)
        set_tests_properties(${example_name} PROPERTIES FAIL_REGULAR_EXPRESSION "\\[warning\\];\\[error\\];\\[critical\\]")

        # Add ubsan halt on error
        set_tests_properties(${example_name} PROPERTIES ENVIRONMENT "${test_usb_env}" LABELS usb)

        # Add PoE test variants
        add_test(NAME ${example_name}_poe COMMAND ${CMAKE_COMMAND}
            -DTIMEOUT_SECONDS=50
            -P ${CMAKE_CURRENT_LIST_DIR}/cmake/ExecuteTestTimeout.cmake $<TARGET_FILE:${example_name}> ${arguments}
        )
        # Add ubsan halt on error
        set_tests_properties(${example_name}_poe PROPERTIES ENVIRONMENT "${test_poe_env}" LABELS poe)

    endif()

endfunction()

dai_add_example(depth_preview ./depth_preview.cpp  ON OFF)
if(DEPTHAI_HAVE_PCL_SUPPORT)
    dai_add_example(visualize_pointcloud ./visualize_pointcloud.cpp ON OFF)
endif()