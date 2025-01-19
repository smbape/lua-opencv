cmake_minimum_required(VERSION 3.25)

project(minilua)

include(../cmake/initialize.cmake)
include("${luajit_BINARY_DIR}/LuaJITConfig.cmake")
include(../cmake/common.cmake)

add_executable(minilua "${luajit_SRC}/host/minilua.c")
add_compile_definitions(${TARGET_ARCH})
add_compile_options(${HOST_XCFLAGS})

if (UNIX)
  find_library(LIBM_LIBRARIES NAMES m REQUIRED)
  target_link_libraries(minilua ${LIBM_LIBRARIES})
endif()

set_target_output_directories(minilua)

if (MSVC)
  foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${config} config)
    set_target_properties(minilua PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${config} "${EXECUTABLE_OUTPUT_PATH}")
  endforeach()
endif()
