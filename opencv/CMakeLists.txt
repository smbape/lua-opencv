cmake_minimum_required(VERSION 3.25)

# Sources: https://github.com/opencv/ci-gha-workflow/blob/main/.github/workflows/OCV-WinPack-4.x-W10.yaml

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

set(OpenCV_VERSION 4.12.0 CACHE STRING "Choose the OpenCV version.")
set_property(CACHE OpenCV_VERSION PROPERTY STRINGS "4.12.0")

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
    GIT_REPOSITORY  https://github.com/opencv/opencv.git
    GIT_TAG         ${OpenCV_VERSION}
    GIT_SHALLOW     TRUE

    # PATCH_COMMAND is run once for each BUILD_TYPE
    # however, source download is only done once when it is a git repository
    # use a custom patch command to perform patch only once
    PATCH_COMMAND   "${CMAKE_COMMAND}"
                    "-DSOURCE_DIR=<SOURCE_DIR>"
                    "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                    "-DPATCH_FILE=${CMAKE_SOURCE_DIR}/patches/001-opencv-src.patch"
                    -P "${CMAKE_SOURCE_DIR}/cmake/git_patch_once.cmake"
)

option(BUILD_contrib "Build with opencv contrib" OFF)

if (BUILD_contrib)
    # https://github.com/opencv/opencv-python/tree/88
    set(OPENCV_CONTRIB_TAG_4120 d943e1d61c8bc556a13783e1546ee7c1a9e0b1cf)

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
        PATCH_COMMAND   "${CMAKE_COMMAND}"
                        "-DSOURCE_DIR=<SOURCE_DIR>"
                        "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                        "-DPATCH_FILE=${CMAKE_SOURCE_DIR}/patches/001-opencv_contrib-src.patch"
                        -P "${CMAKE_SOURCE_DIR}/cmake/git_patch_once.cmake"
    )

    set(OPENCV_EXTRA_MODULES_PATH "${opencv_contrib_SOURCE_DIR}/modules" CACHE PATH "Where to look for additional OpenCV modules (can be ;-separated list of paths)")
endif()

set(OPENCV_SOURCE_DIR "${opencv_SOURCE_DIR}")
set(OPENCV_BINARY_DIR "${opencv_BINARY_DIR}")
include("${opencv_SOURCE_DIR}/cmake/OpenCVUtils.cmake")

set(opencv_BINARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build" CACHE STRING "Where to build opencv.")
file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/opencv" OPENCV_INSTALL_DATA_DIR_RELATIVE)


# 3rd party libs
option(BUILD_TIFF               "Build libtiff from source"          ON )
option(BUILD_JPEG               "Build libjpeg from source"          ON )
option(BUILD_WEBP               "Build WebP from source"             ON )

# OpenCV build components
# ===================================================
option(BUILD_opencv_apps        "Build utility applications (used for example to train classifiers)" OFF )
option(BUILD_DOCS               "Create build rules for OpenCV Documentation" OFF )
option(BUILD_PERF_TESTS         "Build performance tests"                     OFF )
option(BUILD_TESTS              "Build accuracy & regression tests"           OFF )

# OpenCV installation options
# ===================================================
option(INSTALL_CREATE_DISTRIB   "Change install rules to build the distribution package" ON)

if (MSVC AND BUILD_SHARED_LIBS)
    option(INSTALL_PDB_COMPONENT_EXCLUDE_FROM_ALL "Don't install PDB files by default" OFF)
endif()

# set(VIDEOIO_PLUGIN_LIST "" CACHE STRING "List of videoio backends to be compiled as plugins (ffmpeg, gstreamer, mfx, msmf or special value 'all')")
option(BUILD_opencv_world "All OpenCV modules" OFF)
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
            REQUIRED
            NO_DEFAULT_PATH
        )
        find_library(FREETYPE_LINK_LIBRARIES "freetype"
            PATHS "${FREETYPE_DIR}" ENV FREETYPE_DIR
            PATH_SUFFIXES "lib"
            REQUIRED
            NO_DEFAULT_PATH
        )
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
            REQUIRED
            NO_DEFAULT_PATH
        )
        find_library(HARFBUZZ_LINK_LIBRARIES "harfbuzz"
            PATHS "${HARFBUZZ_DIR}" ENV HARFBUZZ_DIR
            PATH_SUFFIXES "lib"
            REQUIRED
            NO_DEFAULT_PATH
        )
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
        find_package(Freetype REQUIRED)
        find_package(HARFBUZZ REQUIRED)
    endif()
endif()

if (WIN32)
    option(ENABLE_EXPERIMENTAL_WIDE_CHAR           "Attempt to support UTF-16 (wide character) strings on Windows."           OFF)

    if (ENABLE_EXPERIMENTAL_WIDE_CHAR)
        add_definitions(-DENABLE_WIDE_CHAR "-DOPENCV_INCLUDE_PORT_FILE=<enable_wide_char.h>")
        include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

        if (WITH_CUDA)
            if (ENABLE_EXPERIMENTAL_WIDE_CHAR)
                set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} --diag-suppress 767)
            endif()
        endif()
    endif()
endif()

set(__opencv_lua_find_package_targets_global ${CMAKE_FIND_PACKAGE_TARGETS_GLOBAL})
set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL TRUE)
add_subdirectory("${opencv_SOURCE_DIR}" "${opencv_BINARY_DIR}")
set(CMAKE_FIND_PACKAGE_TARGETS_GLOBAL ${__opencv_lua_find_package_targets_global})
unset(__opencv_lua_find_package_targets_global)

set(opencv_SOURCE_DIR "${opencv_SOURCE_DIR}" PARENT_SCOPE)
set(OpenCV_INCLUDE_DIRS "${OPENCV_CONFIG_FILE_INCLUDE_DIR}" "${opencv_SOURCE_DIR}/include")


list(REMOVE_DUPLICATES OPENCV_MODULES_PUBLIC)
unset(OpenCV_LIBS)

foreach(m opencv_core ${OPENCV_MODULES_PUBLIC})
    if(TARGET ${m})
        list(APPEND OpenCV_LIBS ${m})
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
