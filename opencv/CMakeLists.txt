cmake_minimum_required(VERSION 3.19)

# Sources: https://github.com/opencv/ci-gha-workflow/blob/main/.github/workflows/OCV-WinPack-4.x-W10.yaml

# Name of the project (will be the name of the plugin)
project(opencv)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(DEFAULT_BUILD_TYPE "Release")

if(NOT DEFINED CMAKE_BUILD_TYPE)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build.")
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

set(OpenCV_VERSION 4.8.0 CACHE STRING "Choose the OpenCV version.")
set_property(CACHE OpenCV_VERSION PROPERTY STRINGS "4.8.0" "4.7.0" "4.6.0" "4.5.5" "4.5.4" "4.5.3" "4.5.2" "4.5.1")

string(REPLACE "." ";" OpenCV_VERSION_LIST ${OpenCV_VERSION})
list(GET OpenCV_VERSION_LIST 0 OpenCV_VERSION_MAJOR)
list(GET OpenCV_VERSION_LIST 1 OpenCV_VERSION_MINOR)
list(GET OpenCV_VERSION_LIST 2 OpenCV_VERSION_PATCH)

set(OpenCV_DLLVERSION "${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}")
set(OpenCV_DEBUG_POSTFIX d)

# Tell cmake we will need opencv.
include(FetchContent)
FetchContent_Populate(opencv
  GIT_REPOSITORY    https://github.com/opencv/opencv.git
  GIT_TAG           ${OpenCV_VERSION}

  # PATCH_COMMAND is run once for each BUILD_TYPE
  # however, source download is only done once when it is a git repository
  # use a custom patch command to perform patch only once
  PATCH_COMMAND "${CMAKE_COMMAND}"
                  "-DSOURCE_DIR=<SOURCE_DIR>"
                  "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                  "-DPATCH_FILE=${CMAKE_CURRENT_SOURCE_DIR}/../patches/001-opencv-src.patch"
                  -P "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/git_patch_once.cmake"
)

set(OPENCV_CONTRIB_TAG_480 f10c84d48b0714f2b408c9e5cccfac1277c8e6cc)
set(OPENCV_CONTRIB_TAG_470 e247b680a6bd396f110274b6c214406a93171350)
set(OPENCV_CONTRIB_TAG_460 db16caf6ceee76b43b94c846be276e92a43e9700)
set(OPENCV_CONTRIB_TAG_455 49e8f123ca08e76891856a1ecce491b62d08ba20)
set(OPENCV_CONTRIB_TAG_454 2bc3e1a169afd2675ef68221176f32cc0ea5427a)
set(OPENCV_CONTRIB_TAG_453 d5317d6297a8129b66dba1a1f7cc784e94639da9)
set(OPENCV_CONTRIB_TAG_452 f5d7f6712d4ff229ba4f45cf79dfd11c557d56fd)
set(OPENCV_CONTRIB_TAG_451 b91a781cbc1285d441aa682926d93d8c23678b0b)

# Tell cmake that we will need opencv_contrib.
include(FetchContent)
FetchContent_Populate(opencv_contrib
  GIT_REPOSITORY https://github.com/opencv/opencv_contrib.git
  GIT_TAG        ${OPENCV_CONTRIB_TAG_${OpenCV_DLLVERSION}} # https://github.com/opencv/opencv-python/tree/74
)

set(opencv_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build" CACHE STRING "Where to build opencv.")
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/opencv" OPENCV_INSTALL_DATA_DIR_RELATIVE)

set(OPENCV_EXTRA_MODULES_PATH "${opencv_contrib_SOURCE_DIR}/modules")
# set(OPENCV_EXTRA_MODULES_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../modules")

# set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/install")
set(INSTALL_PDB ON)
set(INSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL OFF)
set(INSTALL_CREATE_DISTRIB ON)
set(OPENCV_GENERATE_SETUPVARS ON)

# set(VIDEOIO_PLUGIN_LIST all)
set(BUILD_opencv_world ON)
set(BUILD_opencv_python2 OFF)
set(BUILD_opencv_python3 OFF)
set(BUILD_opencv_java OFF)
set(BUILD_opencv_apps OFF)
# set(BUILD_opencv_freetype OFF)
set(BUILD_SHARED_LIBS ON)
set(BUILD_TESTS OFF)
set(BUILD_PERF_TESTS OFF)
set(BUILD_DOCS OFF)
set(BUILD_EXAMPLES OFF)
# set(WITH_EIGEN ON)
# set(BUILD_OPENEXR ON)
set(BUILD_opencv_java_bindings_generator OFF)
set(BUILD_opencv_js_bindings_generator OFF)
set(BUILD_opencv_objc_bindings_generator OFF)
set(BUILD_opencv_python_bindings_generator ON)

add_subdirectory("${opencv_SOURCE_DIR}" "${opencv_BINARY_DIR}")
