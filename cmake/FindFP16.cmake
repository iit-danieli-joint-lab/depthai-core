find_path(FP16_INCLUDE_DIR fp16.h)

mark_as_advanced(FP16_INCLUDE_DIR)

if(FP16_INCLUDE_DIR AND NOT TARGET FP16::fp16)
  add_library(FP16::fp16 INTERFACE IMPORTED)
  set_target_properties(FP16::fp16 PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${FP16_INCLUDE_DIR}")
endif()


include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(FP16
                                  FOUND_VAR FP16_FOUND
                                  REQUIRED_VARS FP16_INCLUDE_DIR)
