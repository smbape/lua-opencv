cmake_minimum_required(VERSION 3.25)

set(MAJVER  2)
set(MINVER  1)
set(RELVER  0)
set(PREREL  -beta3)
set(VERSION ${MAJVER}.${MINVER}.${RELVER}${PREREL})
set(ABIVER  5.1)
string(REPLACE "." "" NODOTABIVER ${ABIVER})

set(DEFAULT_BUILD_TYPE "Release")

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build.")
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin" CACHE PATH "Output directory for applications")
set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

function(set_target_output_directories target)
  set_target_properties(${target} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  )
endfunction()

macro(cmake_script_append_var content_var)
  foreach(var_name ${ARGN})
    set(${content_var} "${${content_var}}
set(${var_name} \"${${var_name}}\")
")
  endforeach()
endmacro()

function(vcpkg_regex_replace_string filename match replace)
    file(READ "${filename}" old_contents)
    string(REGEX REPLACE "${match}" "${replace}" new_contents "${old_contents}")
    if (NOT "${new_contents}" STREQUAL "${old_contents}")
        file(WRITE "${filename}" "${new_contents}")
    endif()
endfunction()

include (GNUInstallDirs)
