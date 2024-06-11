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
foreach(var_name BINDIR DIR INCDIR)
  if (DEFINED LUA_${var_name} AND "${LUA_${var_name}} " STREQUAL " ")
    unset(LUA_${var_name} CACHE)
  endif()

  if (DEFINED LUA_${var_name})
    set(ENV{LUA_${var_name}} "${LUA_${var_name}}")
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
if (DEFINED ENV{OpenCV_DIR})
  find_path (
    OpenCV_DIR
    "OpenCVConfig.cmake"
    PATHS ENV OpenCV_DIR
    PATH_SUFFIXES
      lib64/cmake/opencv4
      lib/cmake/opencv4
    DOC "Root directory of OpenCV"
    REQUIRED
  )
endif()

if (UNIX AND (NOT APPLE))
  set(WITH_QT "5" CACHE STRING "Build with Qt Backend support")
endif()

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
  macro(ocv_find_package_Qt4)
    find_package(Qt4 COMPONENTS QtCore QtGui QtTest ${ARGN})
    if(QT4_FOUND)
      set(QT_FOUND 1)
    endif()
  endmacro()

  macro(ocv_find_package_Qt OCV_QT_VER)
    find_package(Qt${OCV_QT_VER} COMPONENTS Core Gui Widgets Test Concurrent ${ARGN} NO_MODULE)
    if(Qt${OCV_QT_VER}_FOUND)
      set(QT_FOUND 1)
    endif()
  endmacro()

  if(WITH_QT)
    if(NOT WITH_QT GREATER 0)
      # BUG: Qt5Config.cmake script can't handle components properly: find_package(QT NAMES Qt6 Qt5 REQUIRED NO_MODULE COMPONENTS Core Gui Widgets Test Concurrent)
      ocv_find_package_Qt(6 QUIET)
      if(NOT QT_FOUND)
        ocv_find_package_Qt(5 QUIET)
      endif()
      if(NOT QT_FOUND)
        ocv_find_package_Qt4(REQUIRED)
      endif()
    elseif(WITH_QT EQUAL 4)
      ocv_find_package_Qt4(REQUIRED)
    else()  # WITH_QT=<major version>
      ocv_find_package_Qt("${WITH_QT}" REQUIRED)
    endif()
  endif()

  if (BUILD_contrib)
    find_package(Iconv QUIET)
  endif()

  find_package(OpenCV REQUIRED)
  string(REPLACE "." "" OpenCV_DLLVERSION ${OpenCV_VERSION})

  # When built as a static library, only world library is created
  # When world is part of the modules
  if (NOT OpenCV_SHARED AND ";${OpenCV_LIBS};" MATCHES ";opencv_world;")
    set(_OpenCV_LIBS opencv_world)

    set(opencv_sfm_IS_NOT_PART_OF_WORLD TRUE)
    set(opencv_img_hash_IS_NOT_PART_OF_WORLD TRUE)

    foreach(m ${OpenCV_LIBS})
      if(${m}_IS_NOT_PART_OF_WORLD)
        list(APPEND _OpenCV_LIBS ${m})
      endif()
    endforeach()
    set(OpenCV_LIBS ${_OpenCV_LIBS})
    unset(_OpenCV_LIBS)
  endif()

endif()

# ===============
# OpenCV Lua
# ===============
add_subdirectory(opencv_lua)
