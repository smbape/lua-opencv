cmake_minimum_required(VERSION 3.25)

# https://www.youtube.com/watch?v=mczb2COlt1g

project(opencv_lua)

set(DEFAULT_BUILD_TYPE "Release")

if((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE} " STREQUAL " "))
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

if (WIN32)
  # We want a unique dll with all it's dependencies.
  # On Windows, this is only achievable through static libraries
  # Therefore, build a static library on Windows
  option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" OFF)
else()
  # On Linux, lua look for shared libraries by default.
  # Therefore, build a shared library on Linux.
  option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC AND NOT (CMAKE_GENERATOR STREQUAL Ninja))
  add_compile_options(/MP)
endif()

if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW) # CMake 3.15+: leave MSVC runtime selection out of default CMAKE_<LANG>_FLAGS_<CONFIG> flags
endif()

# ===============
# Lua
# ===============
foreach(env_var BINDIR DIR INCDIR)
  if (DEFINED LUA_${env_var} AND "${LUA_${env_var}} " STREQUAL " ")
    unset(LUA_${env_var} CACHE)
  endif()

  if (DEFINED LUA_${env_var})
    set(ENV{LUA_${env_var}} "${LUA_${env_var}}")
  endif()
endforeach()

# Hack to ensure that Lua is built with BUILD_SHARED_LIBS ON
set(BUILD_SHARED_LIBS_BACKUP ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS ON)
include(cmake/FindLua.cmake)
set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BACKUP})

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
    COMMAND "${Python3_EXECUTABLE}" -m pip install --upgrade pip
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
  )
  execute_process(
    COMMAND "${Python3_EXECUTABLE}" -m pip install numpy
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
  )
endif()

# ===============
# Eigen
# ===============
include(eigen/eigen.cmake)

# ===============
# OpenCV
# ===============
if ((NOT DEFINED OpenCV_DIR) AND (NOT DEFINED OpenCV_LIBS))
  if (WIN32)
    # On Windows, BUILD_SHARED_LIBS must be the same in other for the build to be successfull
    add_subdirectory(opencv)
  else()
    # On Linux, we want OpenCV to be embeded in the library, be it shared or static
    # Therefore, enforce BUILD_SHARED_LIBS OFF when building OpenCV
    # Hack to ensure that OpenCV is built with BUILD_SHARED_LIBS OFF
    set(BUILD_SHARED_LIBS_BACKUP ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(opencv)
    set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BACKUP})
  endif()
else()
  find_package(OpenCV REQUIRED)
endif()

# ===============
# OpenCV Lua
# ===============
add_subdirectory(opencv_lua)
