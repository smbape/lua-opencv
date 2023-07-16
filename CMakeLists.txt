cmake_minimum_required(VERSION 3.19)

# https://www.youtube.com/watch?v=mczb2COlt1g

# Name of the project (will be the name of the plugin)
project(opencv_lua)

set(DEFAULT_BUILD_TYPE "Release")

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build.")
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(WIN32 AND MSVC)
  add_compile_options(/MP)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
find_package(Lua REQUIRED)

# include(cmake/RequireOpenCV.cmake)
set(OpenCV_LIBS "")
set(OpenCV_DLLVERSION 480)
set(OpenCV_DEBUG_POSTFIX d)

include(cmake/vcpkg_regex_replace_string.cmake)

set(SOL2_VERSION 3.3.0 CACHE STRING "Choose the sol2 version.")
set_property(CACHE SOL2_VERSION PROPERTY STRINGS "3.3.0" "3.2.2")

# Tell cmake that we will need sol2. These command will pull in sol2.
include(FetchContent)
FetchContent_Declare(sol2
  GIT_REPOSITORY https://github.com/ThePhD/sol2.git
  GIT_TAG        v${SOL2_VERSION}
)
FetchContent_MakeAvailable(sol2)

# update version in files
# TODO

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

# generate source files
# TODO

file(GLOB_RECURSE project_source_headers "src/*.h*")
file(GLOB_RECURSE project_sources_files "src/*.c" "src/*.cpp")

file(GLOB_RECURSE project_generated_headers "generated/*.h*")
file(GLOB_RECURSE project_generated_files "generated/*.cpp")

source_group("Source Headers" FILES ${project_source_headers})
source_group("Source Files" FILES ${project_sources_files})

source_group("Generated Headers" FILES ${project_generated_headers})
source_group("Generated Files" FILES ${project_generated_files})

set(SRC_FILES
  ${project_source_headers}
  ${project_sources_files}

  ${project_generated_headers}
  ${project_generated_files}
)

# Build a shared library named as the project
set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC LUA_MODULE_NAME=${PROJECT_NAME})
target_compile_definitions(${PROJECT_NAME} PRIVATE LUAAPI_EXPORTS)
target_precompile_headers(${PROJECT_NAME} PRIVATE
  "$<$<COMPILE_LANGUAGE:CXX>:lua_bridge_generated_pch.hpp>"
)

target_link_libraries(${PROJECT_NAME} PRIVATE sol2::sol2 ${LUA_LIBRARIES} ${OpenCV_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})

# set(PROJECT_VERSION "${SOL2_VERSION}-${OpenCV_DLLVERSION}")
set(PROJECT_VERSION "0.0.0")
string(REPLACE "." ";" PROJECT_VERSION_LIST ${PROJECT_VERSION})
list(GET PROJECT_VERSION_LIST 0 PROJECT_VERSION_MAJOR)
list(GET PROJECT_VERSION_LIST 1 PROJECT_VERSION_MINOR)
list(GET PROJECT_VERSION_LIST 2 PROJECT_VERSION_PATCH)

set_target_properties(${PROJECT_NAME} PROPERTIES
  # OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}"
  # VERSION "${PROJECT_VERSION}"
  # SOVERSION "${PROJECT_VERSION_MAJOR}"
  # DEBUG_POSTFIX "${OpenCV_DEBUG_POSTFIX}"
  COMPILE_PDB_NAME "${PROJECT_NAME}${OpenCV_DLLVERSION}"
  COMPILE_PDB_NAME_DEBUG "${PROJECT_NAME}${OpenCV_DLLVERSION}${OpenCV_DEBUG_POSTFIX}"
  ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
)

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/DEBUG")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "src/include")
target_include_directories(${PROJECT_NAME} PUBLIC "generated")

# /Zc:__cplusplus is required to make __cplusplus accurate
# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
# That version is equivalent to _MSC_VER==1914
# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus")
  target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
endif()

IF ((WIN32) AND CMAKE_GENERATOR STREQUAL Ninja)
  target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus")
  target_compile_options(${PROJECT_NAME} PRIVATE "/bigobj")
  target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _WINDOWS)
  target_compile_definitions(${PROJECT_NAME} PRIVATE _WINDLL)
ENDIF()

# add executable target that represents require_from_dll program
add_executable(require_from_dll test/require_from_dll.cpp)
target_link_libraries(require_from_dll
  PRIVATE ${PROJECT_NAME} ${LUA_LIBRARIES} sol2::sol2)
target_include_directories(require_from_dll
  PRIVATE ${LUA_INCLUDE_DIR})
