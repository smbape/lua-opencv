cmake_minimum_required(VERSION 3.25)

set(DEFAULT_BUILD_TYPE "Release")

if((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE} " STREQUAL " "))
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

function(set_target_output_directories target)
  set_target_properties(${target} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  )
endfunction()

macro(cmake_script_append_var content_var)
  foreach(var_name IN ITEMS ${ARGN})
    string(REPLACE "\"" "\\\"" ${var_name} "${${var_name}}")
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

set(Luajit_COMMIT_HASH_2_1_1736781742 a4f56a459a588ae768801074b46ba0adcfb49eb1)
set(Luajit_COMMIT_HASH_2_1_1713773202 5790d253972c9d78a0c2aece527eda5b134bbbf7)
set(Luajit_COMMIT_HASH_2_1_1707061634 0d313b243194a0b8d2399d8b549ca5a0ff234db5)
set(Luajit_COMMIT_HASH_2_1_0_beta3 8271c643c21d1b2f344e339f559f2de6f3663191)

set(Luajit_URL_HASH_2_1_1736781742 cf926ee84ec19bfee85fafbc7bdd31056b06d65d1409ae1f9b8b1930d9041c87)
set(Luajit_URL_HASH_2_1_1713773202 b950732cb1865dbc2eb25a0991285a51864705b1de7cb0bd9ce1d198bf6b3e50)
set(Luajit_URL_HASH_2_1_1707061634 55c7a00edb7c18233ecf1b3a85ea232c1d7d7e3d5bd94b3d34bae28a61542d93)
set(Luajit_URL_HASH_2_1_0_beta3 7f3b1ec1e12b1e3ce31adcd33d73dd7c5056b1df02c7779e29f2239daed6bc93)

set(Luajit_VERSION_LATEST 2.1.1736781742)

if ((NOT DEFINED Luajit_VERSION) OR ("${Luajit_VERSION} " STREQUAL " "))
  set(Luajit_VERSION "2.1")
endif()

set(Luajit_VERSION "${Luajit_VERSION}" CACHE STRING "Choose the LuaJIT version." FORCE)
set_property(CACHE Luajit_VERSION PROPERTY STRINGS "2.1;2.1.1736781742;2.1.1713773202;2.1.1707061634;2.1.0-beta3")

if ("${Luajit_VERSION}" STREQUAL "2.1")
  set(Luajit_VERSION ${Luajit_VERSION_LATEST})
  set(Luajit_VERSION ${Luajit_VERSION} PARENT_SCOPE)
endif()

set(Lua_INTERPRETER_NAME "luajit-${Luajit_VERSION}" CACHE STRING "Lua interpreter name" FORCE)
set(Lua_VERSION luajit-${Luajit_VERSION})
set(Lua_VERSION luajit-${Luajit_VERSION} PARENT_SCOPE)

string(REGEX REPLACE "[.-]" "_" Luajit_HASH_VERSION ${Luajit_VERSION})

string(REPLACE "." ";" Luajit_VERSION_LIST ${Luajit_VERSION})
list(GET Luajit_VERSION_LIST 0 MAJVER)
list(GET Luajit_VERSION_LIST 1 MINVER)
list(GET Luajit_VERSION_LIST 2 RELVER)

set(VERSION ${Luajit_VERSION})
set(ABIVER  5.1)
string(REPLACE "." "" NODOTABIVER ${ABIVER})

include(GNUInstallDirs)
