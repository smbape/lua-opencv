cmake_minimum_required(VERSION 3.25)

set(DEFAULT_BUILD_TYPE "Release")

if((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE} " STREQUAL " "))
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
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
if ((NOT DEFINED Luajit_VERSION) OR ("${Luajit_VERSION} " STREQUAL " "))
  set(Luajit_VERSION 2.1.0-beta3 CACHE STRING "Choose the LuaJIT version." FORCE)
  set_property(CACHE Luajit_VERSION PROPERTY STRINGS "2.1.0-beta3")
else()
  if ("${Luajit_VERSION}" STREQUAL "2.1")
    set(Luajit_VERSION 2.1.0-beta3)
    set(Luajit_VERSION 2.1.0-beta3 PARENT_SCOPE)
    set(Luajit_VERSION 2.1.0-beta3 CACHE STRING "Choose the LuaJIT version." FORCE)
    set(Lua_VERSION luajit-${Luajit_VERSION})
    set(Lua_VERSION luajit-${Luajit_VERSION} PARENT_SCOPE)
    set(Lua_VERSION luajit-${Luajit_VERSION} CACHE STRING "Choose the Lua version." FORCE)
  endif()
endif()
string(REGEX REPLACE "[.-]" "" Luajit_DLLVERSION ${Luajit_VERSION})

string(REPLACE "." ";" Luajit_VERSION_LIST ${Luajit_VERSION})
list(GET Luajit_VERSION_LIST 0 MAJVER)
list(GET Luajit_VERSION_LIST 1 MINVER)
list(GET Luajit_VERSION_LIST 2 RELVER)

set(VERSION ${Luajit_VERSION})
set(ABIVER  5.1)
string(REPLACE "." "" NODOTABIVER ${ABIVER})
