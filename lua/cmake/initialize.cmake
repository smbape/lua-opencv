cmake_minimum_required(VERSION 3.25)

set(DEFAULT_BUILD_TYPE "Release")

if((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE} " STREQUAL " "))
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin${LUA_MULTI_CONFIG_SUFFIX}" CACHE INTERNAL "Output directory for applications" FORCE)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib${LUA_MULTI_CONFIG_SUFFIX}" CACHE INTERNAL "Output directory for libraries" FORCE)

function(set_target_output_directories target)
    set_target_properties(${target} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
        COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
        LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
        RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
    )
endfunction()

function(vcpkg_regex_replace_string filename match replace)
        file(READ "${filename}" old_contents)
        string(REGEX REPLACE "${match}" "${replace}" new_contents "${old_contents}")
        if (NOT "${new_contents}" STREQUAL "${old_contents}")
                file(WRITE "${filename}" "${new_contents}")
        endif()
endfunction()

include (GNUInstallDirs)

if(MSVC)
    add_compile_options(/MP)
endif()

if(NOT WIN32)
    add_compile_options(-Wall)
endif()

# Ugly warnings
if(MSVC)
    # add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
endif()
