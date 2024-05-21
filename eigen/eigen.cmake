set(EIGEN_VERSION 3.4.0 CACHE STRING "Choose the Eigen version.")
set_property(CACHE EIGEN_VERSION PROPERTY STRINGS "3.4.0" "3.3.9")
option(EIGEN_ENABLE_INSTALL "Enable installation of Eigen3" OFF)
option(EIGEN_BUILD_DOC "Enable creation of Eigen documentation" OFF)
option(BUILD_TESTING "Enable creation of Eigen tests." OFF)

set(CMAKE_EXPORT_NO_PACKAGE_REGISTRY ON)
if(NOT WIN32)
  # On Linux, we want Eigen to be embeded in the library, be it shared or static
  # Therefore, enforce BUILD_SHARED_LIBS OFF when building Eigen
  # Hack to ensure that Eigen is built with BUILD_SHARED_LIBS OFF
  set(BUILD_SHARED_LIBS_BACKUP ${BUILD_SHARED_LIBS})
  set(BUILD_SHARED_LIBS OFF)
endif()
# Tell cmake that we will need eigen. These command will pull in eigen.
include(FetchContent)
FetchContent_Declare(eigen
  GIT_REPOSITORY  https://gitlab.com/libeigen/eigen.git
  GIT_TAG         ${EIGEN_VERSION}
  GIT_SHALLOW     TRUE

  # PATCH_COMMAND is run once for each BUILD_TYPE
  # however, source download is only done once when it is a git repository
  # use a custom patch command to perform patch only once
  PATCH_COMMAND "${CMAKE_COMMAND}"
                  "-DSOURCE_DIR=<SOURCE_DIR>"
                  "-DGIT_EXECUTABLE=${GIT_EXECUTABLE}"
                  "-DPATCH_FILE=${CMAKE_SOURCE_DIR}/patches/001-eigen-src.patch"
                  -P "${CMAKE_SOURCE_DIR}/cmake/git_patch_once.cmake"
)
FetchContent_MakeAvailable(eigen)
if(NOT WIN32)
  set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BACKUP})
endif()
unset(CMAKE_EXPORT_NO_PACKAGE_REGISTRY)
set(ENV{Eigen3_DIR} "${eigen_BINARY_DIR}")
