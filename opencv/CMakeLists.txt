cmake_minimum_required(VERSION 3.25)

# Sources: https://github.com/opencv/ci-gha-workflow/blob/main/.github/workflows/OCV-WinPack-4.x-W10.yaml

# Name of the project (will be the name of the plugin)
project(opencv)

include(../cmake/FindPatch.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEFAULT_BUILD_TYPE "Release")

if((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE} " STREQUAL " "))
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

set(OpenCV_VERSION 4.10.0 CACHE STRING "Choose the OpenCV version.")
set_property(CACHE OpenCV_VERSION PROPERTY STRINGS "4.10.0" "4.9.0" "4.8.1" "4.8.0" "4.7.0" "4.6.0" "4.5.5" "4.5.4" "4.5.3" "4.5.2" "4.5.1")

string(REPLACE "." ";" OpenCV_VERSION_LIST ${OpenCV_VERSION})
list(GET OpenCV_VERSION_LIST 0 OpenCV_VERSION_MAJOR)
list(GET OpenCV_VERSION_LIST 1 OpenCV_VERSION_MINOR)
list(GET OpenCV_VERSION_LIST 2 OpenCV_VERSION_PATCH)

set(OpenCV_DLLVERSION "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}")
set(OpenCV_DLLVERSION "${OpenCV_DLLVERSION}" PARENT_SCOPE)
set(OpenCV_DEBUG_POSTFIX d)

# Tell cmake we will need opencv.
include(FetchContent)
FetchContent_Populate(opencv
  GIT_REPOSITORY    https://github.com/opencv/opencv.git
  GIT_TAG           ${OpenCV_VERSION}
  GIT_SHALLOW       TRUE

  # PATCH_COMMAND is run once for each BUILD_TYPE
  # however, source download is only done once when it is a git repository
  # use a custom patch command to perform patch only once
  PATCH_COMMAND "${CMAKE_COMMAND}"
                  "-DSOURCE_DIR=<SOURCE_DIR>"
                  "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                  "-DPATCH_FILE=${CMAKE_SOURCE_DIR}/patches/001-opencv-src.patch"
                  -P "${CMAKE_SOURCE_DIR}/cmake/git_patch_once.cmake"
)

option(BUILD_contrib "Build with opencv contrib" OFF)

if (BUILD_contrib)
  # https://github.com/opencv/opencv-python/tree/84
  set(OPENCV_CONTRIB_TAG_4100 1ed3dd2c53888e3289afdb22ec4e9ebbff3dba87)
  set(OPENCV_CONTRIB_TAG_490 c7602a8f74205e44389bd6a4e8d727d32e7e27b4)
  set(OPENCV_CONTRIB_TAG_481 f10c84d48b0714f2b408c9e5cccfac1277c8e6cc)
  set(OPENCV_CONTRIB_TAG_480 f10c84d48b0714f2b408c9e5cccfac1277c8e6cc)
  set(OPENCV_CONTRIB_TAG_470 e247b680a6bd396f110274b6c214406a93171350)
  set(OPENCV_CONTRIB_TAG_460 db16caf6ceee76b43b94c846be276e92a43e9700)
  set(OPENCV_CONTRIB_TAG_455 49e8f123ca08e76891856a1ecce491b62d08ba20)
  set(OPENCV_CONTRIB_TAG_454 2bc3e1a169afd2675ef68221176f32cc0ea5427a)
  set(OPENCV_CONTRIB_TAG_453 d5317d6297a8129b66dba1a1f7cc784e94639da9)
  set(OPENCV_CONTRIB_TAG_452 f5d7f6712d4ff229ba4f45cf79dfd11c557d56fd)
  set(OPENCV_CONTRIB_TAG_451 b91a781cbc1285d441aa682926d93d8c23678b0b)

  if (NOT DEFINED OPENCV_CONTRIB_TAG_${OpenCV_DLLVERSION})
    message(FATAL_ERROR "Unable to find git contrib tag for OPENCV_CONTRIB_TAG_${OpenCV_DLLVERSION}
      OpenCV_VERSION       = ${OpenCV_VERSION}
      OpenCV_DLLVERSION    = ${OpenCV_DLLVERSION}
      OpenCV_VERSION_MAJOR = ${OpenCV_VERSION_MAJOR}
      OpenCV_VERSION_MINOR = ${OpenCV_VERSION_MINOR}
      OpenCV_VERSION_PATCH = ${OpenCV_VERSION_PATCH}
      ")
  endif()

  # Tell cmake that we will need opencv_contrib.
  include(FetchContent)
  FetchContent_Populate(opencv_contrib
    GIT_REPOSITORY  https://github.com/opencv/opencv_contrib.git
    GIT_TAG         ${OPENCV_CONTRIB_TAG_${OpenCV_DLLVERSION}}
    GIT_SHALLOW     TRUE

    # PATCH_COMMAND is run once for each BUILD_TYPE
    # however, source download is only done once when it is a git repository
    # use a custom patch command to perform patch only once
    PATCH_COMMAND "${CMAKE_COMMAND}"
                    "-DSOURCE_DIR=<SOURCE_DIR>"
                    "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                    "-DPATCH_FILE=${CMAKE_SOURCE_DIR}/patches/001-opencv_contrib-src.patch"
                    -P "${CMAKE_SOURCE_DIR}/cmake/git_patch_once.cmake"
  )

  set(OPENCV_EXTRA_MODULES_PATH "${opencv_contrib_SOURCE_DIR}/modules" CACHE PATH "Where to look for additional OpenCV modules (can be ;-separated list of paths)")
endif()

include("${opencv_SOURCE_DIR}/cmake/OpenCVUtils.cmake")

set(opencv_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build" CACHE STRING "Where to build opencv.")
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/opencv" OPENCV_INSTALL_DATA_DIR_RELATIVE)


# 3rd party libs
OCV_OPTION(BUILD_TIFF               "Build libtiff from source"          ON )
OCV_OPTION(BUILD_JPEG               "Build libjpeg from source"          ON )
OCV_OPTION(BUILD_WEBP               "Build WebP from source"             ON )

# OpenCV build components
# ===================================================
OCV_OPTION(BUILD_opencv_apps        "Build utility applications (used for example to train classifiers)" OFF )
OCV_OPTION(BUILD_DOCS               "Create build rules for OpenCV Documentation" OFF )
OCV_OPTION(BUILD_PERF_TESTS         "Build performance tests"                     OFF )
OCV_OPTION(BUILD_TESTS              "Build accuracy & regression tests"           OFF )

# OpenCV installation options
# ===================================================
OCV_OPTION(INSTALL_CREATE_DISTRIB   "Change install rules to build the distribution package" ON)

if (MSVC AND BUILD_SHARED_LIBS)
  option(INSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL "Don't install PDB files by default" OFF)
endif()

# set(VIDEOIO_PLUGIN_LIST "" CACHE STRING "List of videoio backends to be compiled as plugins (ffmpeg, gstreamer, mfx, msmf or special value 'all')")
option(BUILD_opencv_world "All OpenCV modules" ON)
option(BUILD_opencv_python2 "The python2 module" OFF)
option(BUILD_opencv_python3 "The python3 module" OFF)
option(BUILD_opencv_java "The java module" OFF)
# set(BUILD_opencv_freetype OFF)
# set(WITH_EIGEN ON)
# set(BUILD_OPENEXR ON)
option(BUILD_opencv_java_bindings_generator "The java bindings" OFF)
option(BUILD_opencv_js_bindings_generator "The JavaScript(JS) bindings" OFF)
option(BUILD_opencv_objc_bindings_generator "The Objective-C bindings" OFF)
option(BUILD_opencv_python_bindings_generator "The Python bindings" ON)

if (WITH_FREETYPE)
  if (WIN32)
    find_path(FREETYPE_INCLUDE_DIRS "ft2build.h"
      PATHS "${FREETYPE_DIR}" ENV FREETYPE_DIR
      PATH_SUFFIXES "include"
      NO_DEFAULT_PATH)
    find_library(FREETYPE_LINK_LIBRARIES "freetype"
      PATHS "${FREETYPE_DIR}" ENV FREETYPE_DIR
      PATH_SUFFIXES "lib"
      NO_DEFAULT_PATH)
    if (FREETYPE_INCLUDE_DIRS AND FREETYPE_LINK_LIBRARIES)
      file(READ "${FREETYPE_INCLUDE_DIRS}/freetype/freetype.h" FREETYPE_VERSION)

      string(REGEX MATCH "FREETYPE_MAJOR +([0-9]+)" _ "${FREETYPE_VERSION}")
      set(FREETYPE_VERSION_MAJOR ${CMAKE_MATCH_1})

      string(REGEX MATCH "FREETYPE_MINOR +([0-9]+)" _ "${FREETYPE_VERSION}")
      set(FREETYPE_VERSION_MINOR ${CMAKE_MATCH_1})

      string(REGEX MATCH "FREETYPE_PATCH +([0-9]+)" _ "${FREETYPE_VERSION}")
      set(FREETYPE_VERSION_PATCH ${CMAKE_MATCH_1})

      set(FREETYPE_VERSION "${FREETYPE_VERSION_MAJOR}.${FREETYPE_VERSION_MINOR}.${FREETYPE_VERSION_PATCH}")

      # Fake ocv_check_modules(FREETYPE)
      set(FREETYPE_LIBRARIES "${FREETYPE_LINK_LIBRARIES}")
      set(FREETYPE_LINK_LIBRARIES_XXXXX TRUE)
      set(FREETYPE_FOUND TRUE)
    endif()

    find_path(HARFBUZZ_INCLUDE_DIRS "hb-ft.h"
      PATHS "${HARFBUZZ_DIR}" ENV HARFBUZZ_DIR
      PATH_SUFFIXES "include/harfbuzz"
      NO_DEFAULT_PATH)
    find_library(HARFBUZZ_LINK_LIBRARIES "harfbuzz"
      PATHS "${HARFBUZZ_DIR}" ENV HARFBUZZ_DIR
      PATH_SUFFIXES "lib"
      NO_DEFAULT_PATH)
    if (HARFBUZZ_INCLUDE_DIRS AND HARFBUZZ_LINK_LIBRARIES)
      file(READ "${HARFBUZZ_INCLUDE_DIRS}/hb-version.h" HARFBUZZ_VERSION)

      string(REGEX MATCH "HB_VERSION_MAJOR +([0-9]+)" _ ${HARFBUZZ_VERSION})
      set(HARFBUZZ_VERSION_MAJOR ${CMAKE_MATCH_1})

      string(REGEX MATCH "HB_VERSION_MINOR +([0-9]+)" _ ${HARFBUZZ_VERSION})
      set(HARFBUZZ_VERSION_MINOR ${CMAKE_MATCH_1})

      string(REGEX MATCH "HB_VERSION_MICRO +([0-9]+)" _ ${HARFBUZZ_VERSION})
      set(HARFBUZZ_VERSION_PATCH ${CMAKE_MATCH_1})

      set(HARFBUZZ_VERSION "${HARFBUZZ_VERSION_MAJOR}.${HARFBUZZ_VERSION_MINOR}.${HARFBUZZ_VERSION_PATCH}")

      # Fake ocv_check_modules(HARFBUZZ)
      set(HARFBUZZ_LIBRARIES "${HARFBUZZ_LINK_LIBRARIES}")
      set(HARFBUZZ_LINK_LIBRARIES_XXXXX TRUE)
      set(HARFBUZZ_FOUND TRUE)
    endif()
  else()
    find_package(Freetype)
    find_package(HARFBUZZ)
  endif()
endif()

if (WIN32)
  option(ENABLE_EXPERIMENTAL_WIDE_CHAR           "Attempt to support UTF-16 (wide character) strings on Windows."           OFF)

  if (ENABLE_EXPERIMENTAL_WIDE_CHAR)
    add_definitions(-DENABLE_WIDE_CHAR "-DOPENCV_INCLUDE_PORT_FILE=<enable_wide_char.h>")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

    if (WIN32 AND WITH_CUDA)
      if (ENABLE_EXPERIMENTAL_WIDE_CHAR)
        set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} --diag-suppress 767)
      endif()
      # Workaround long paths on windows
      list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/cuda/Modules")
    endif()
  endif()
endif()

add_subdirectory("${opencv_SOURCE_DIR}" "${opencv_BINARY_DIR}")

set(opencv_SOURCE_DIR "${opencv_SOURCE_DIR}" PARENT_SCOPE)
set(OpenCV_LIBS "")
set(OpenCV_INCLUDE_DIRS "${OPENCV_CONFIG_FILE_INCLUDE_DIR}" "${opencv_SOURCE_DIR}/include")

foreach(m opencv_core ${OPENCV_MODULES_PUBLIC})
  if(TARGET ${m})
    list(APPEND OpenCV_LIBS ${m})

    if (WITH_LUA_ROCKS)
      # luarocks does not support VERSION and SOVERSION
      # https://github.com/luarocks/luarocks/issues/1532
      set_property(TARGET ${m} PROPERTY VERSION)
      set_property(TARGET ${m} PROPERTY SOVERSION)
    endif()

    install(TARGETS ${m}
      LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
      RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    )

    if (NOT WITH_LUA_ROCKS)
      install(TARGETS ${m} ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    endif()
  endif()

  if(EXISTS "${OPENCV_MODULE_${m}_LOCATION}/include")
    list(APPEND OpenCV_INCLUDE_DIRS "${OPENCV_MODULE_${m}_LOCATION}/include")
  endif()
endforeach()

set(OpenCV_LIBS "${OpenCV_LIBS}" PARENT_SCOPE)
set(OpenCV_INCLUDE_DIRS "${OpenCV_INCLUDE_DIRS}" PARENT_SCOPE)

if (MSVC AND TARGET opencv_world)
  set_target_properties(opencv_world PROPERTIES
    VS_PROJECT_IMPORT "${CMAKE_CURRENT_SOURCE_DIR}/opencv_world.user.props"
  )
endif()
