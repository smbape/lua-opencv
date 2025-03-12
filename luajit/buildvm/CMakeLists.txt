cmake_minimum_required(VERSION 3.25)

project(buildvm)

include(../cmake/initialize.cmake)
include("${luajit_BINARY_DIR}/LuaJITConfig.cmake")
include(../cmake/common.cmake)

file(GLOB_RECURSE buildvm_source_files "${luajit_SRC}/host/buildvm*.c")
add_executable(buildvm ${buildvm_source_files})
target_include_directories(buildvm PRIVATE "${LUA_INCLUDE_DIR}/host" ${LUA_INCLUDE_DIRS})
add_compile_definitions(${TARGET_ARCH})
add_compile_options(${HOST_XCFLAGS})

set_target_output_directories(buildvm)

if (MSVC)
    foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
        string(TOUPPER ${config} config)
        set_target_properties(buildvm PROPERTIES RUNTIME_OUTPUT_DIRECTORY_${config} "${EXECUTABLE_OUTPUT_PATH}")
    endforeach()
endif()
