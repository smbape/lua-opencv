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

set(Luajit_HASH_2_1_1702296283 29b0b282f59ac533313199f4f7be79490b7eee51)
set(Luajit_HASH_2_1_1702233742 ff204d0350575cf710f6f4af982db146cb454e1a)
set(Luajit_HASH_2_1_1700008891 43d0a19158ceabaa51b0462c1ebc97612b420a2e)
set(Luajit_HASH_2_1_0_beta3 8271c643c21d1b2f344e339f559f2de6f3663191)

set(Luajit_URL_HASH_2_1_1702296283 395218cd88c7648d85d5fc5506b9deeb7d82c162df1e1c29a5317adbda3c7b39)
set(Luajit_URL_HASH_2_1_1702233742 fbb7ba93b181c5451ce3c6beef342458ea2e58826acded60b6663ca372bafbdd)
set(Luajit_URL_HASH_2_1_1700008891 f76df759e8096c990606852339cd50a2d0be662b9cfb4d9bae77fa6695d5871d)
set(Luajit_URL_HASH_2_1_0_beta3 7f3b1ec1e12b1e3ce31adcd33d73dd7c5056b1df02c7779e29f2239daed6bc93)

set(Luajit_VERSION_LATEST 2.1.1702296283)

if ((NOT DEFINED Luajit_VERSION) OR ("${Luajit_VERSION} " STREQUAL " "))
  set(Luajit_VERSION "2.1")
endif()

set(Luajit_VERSION "${Luajit_VERSION}" CACHE STRING "Choose the LuaJIT version." FORCE)
set_property(CACHE Luajit_VERSION PROPERTY STRINGS "2.1;2.1.1702296283;2.1.1702233742;2.1.1700008891;2.1.0-beta3")

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
