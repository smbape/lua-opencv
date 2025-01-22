cmake_minimum_required(VERSION 3.25)

project(opencv_lua LANGUAGES C CXX ASM)

set(DEFAULT_BUILD_TYPE "Release")

if((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE} " STREQUAL " "))
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC AND NOT (CMAKE_GENERATOR STREQUAL "Ninja"))
  add_compile_options(/MP)
endif()

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) # CMake 3.15+: leave MSVC runtime selection out of default CMAKE_<LANG>_FLAGS_<CONFIG> flags
endif()

include(cmake/list_commands.cmake)

# ===============
# Lua
# ===============
include(cmake/FindLua.cmake)

# ===============
# Luarocks
# ===============
add_subdirectory(luarocks)

set(LUA_ONLY "$ENV{LUA_ONLY}")
if (LUA_ONLY)
  message(STATUS "Processing Lua related targets only")
  unset(LUA_INCLUDE_DIR CACHE)
  unset(LUA_INTERPRETER CACHE)
  unset(LUA_LIBRARY CACHE)
  return()
endif()

# ===============
# Python
# ===============
include(cmake/UsePython3Venv.cmake)
if (VIRTUAL_ENV_created)
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m pip install numpy
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
  )
  if (UNIX)
    execute_process(
      COMMAND "${Python3_EXECUTABLE}" -m pip install build auditwheel
      COMMAND_ECHO STDERR
      COMMAND_ERROR_IS_FATAL ANY
    )
    execute_process(
      COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/patches/patch_auditwheel_whitelist.py"
      COMMAND_ECHO STDERR
      COMMAND_ERROR_IS_FATAL ANY
    )
  endif()
endif()

# ===============
# OpenCV
# ===============
include(opencv/OpenCVConfig.cmake)
string(REPLACE "." "" OpenCV_DLLVERSION ${OpenCV_VERSION})
string(REPLACE "." ";" OpenCV_VERSION_PARTS ${OpenCV_VERSION})

# ===============
# OpenCV Lua
# ===============
add_subdirectory(opencv_lua "opencv_lua${LUA_MULTI_CONFIG_SUFFIX}")
