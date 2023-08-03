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

# find OpenCV
if ((NOT DEFINED OpenCV_DIR) AND (NOT DEFINED OpenCV_LIBS))
  add_subdirectory(opencv)

  set(OpenCV_LIBS "")
  set(OpenCV_INCLUDE_DIRS "${OPENCV_CONFIG_FILE_INCLUDE_DIR}" "${opencv_SOURCE_DIR}/include")

  foreach(m opencv_core ${OPENCV_MODULES_PUBLIC})
    if(TARGET ${m})
      list(APPEND OpenCV_LIBS ${m})
    endif()
    if(EXISTS "${OPENCV_MODULE_${m}_LOCATION}/include")
      list(APPEND OpenCV_INCLUDE_DIRS "${OPENCV_MODULE_${m}_LOCATION}/include")
    endif()
  endforeach()
else()
  find_package(OpenCV REQUIRED)
endif()

# find LuaJIT
if ((NOT DEFINED LUA_DIR) AND (NOT DEFINED ENV{LUA_DIR}))
  add_subdirectory(luajit)
  set(LUA_LIBRARIES libluajit::libluajit)
else()
  if (DEFINED LUA_DIR)
    set(ENV{LUA_DIR} "${LUA_DIR}")
  endif()

  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
  find_package(Lua REQUIRED)
endif()

# message(STATUS "OpenCV_LIBS = ${OpenCV_LIBS}")
# message(STATUS "OpenCV_INCLUDE_DIRS = ${OpenCV_INCLUDE_DIRS}")

# include(cmake/RequireOpenCV.cmake)
# set(OpenCV_LIBS "")
# set(OpenCV_DLLVERSION 480)
# set(OpenCV_DEBUG_POSTFIX d)

# include(cmake/vcpkg_regex_replace_string.cmake)

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

include(cmake/version.cmake)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

# generate source files
# TODO

file(GLOB_RECURSE project_source_headers "src/*.h*")
file(GLOB_RECURSE project_sources_files "src/*.cpp")

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

install(TARGETS ${PROJECT_NAME}
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/lua/${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}"
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/lua/${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}"
)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC
    LUA_MODULE_NAME=${PROJECT_NAME}
    LUA_MODULE_VERSION=${PROJECT_VERSION}
    LUA_MODULE_LIB_NAME=OpenCV
    LUA_MODULE_LIB_VERSION=${OpenCV_VERSION}
  PRIVATE LUAAPI_EXPORTS)
target_precompile_headers(${PROJECT_NAME} PRIVATE
  "$<$<COMPILE_LANGUAGE:CXX>:lua_generated_pch.hpp>"
)

target_link_libraries(${PROJECT_NAME} PRIVATE sol2::sol2 ${LUA_LIBRARIES} ${OpenCV_LIBS})
if (DEFINED LUA_INCLUDE_DIR)
  target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
endif()
if (DEFINED OpenCV_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()

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

  # library is in ${CMAKE_INSTALL_LIBDIR}/lua/
  # however, opencv is in ${CMAKE_INSTALL_LIBDIR}/
  # use rpath $ORIGIN/.. to allow finding opencv
  INSTALL_RPATH "$ORIGIN/.."
)

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/DEBUG")
endif()

target_include_directories(${PROJECT_NAME} PUBLIC "src/include" "src" "generated")

# /Zc:__cplusplus is required to make __cplusplus accurate
# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
# That version is equivalent to _MSC_VER==1914
# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)
if ((MSVC) AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus" "/bigobj")
endif()

IF ((WIN32) AND CMAKE_GENERATOR STREQUAL Ninja)
  target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus" "/bigobj")
  target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32 _WINDOWS _WINDLL)
ENDIF()


file(RELATIVE_PATH TEST_LUA_PATH "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/test/test.lua")
file(TO_NATIVE_PATH "${TEST_LUA_PATH}" TEST_LUA_PATH)

set_target_properties(${PROJECT_NAME} PROPERTIES
  VS_DEBUGGER_COMMAND "$<TARGET_FILE:luajit>"
  VS_DEBUGGER_COMMAND_ARGUMENTS "${TEST_LUA_PATH}"
)
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# add executable target that represents require_from_dll program
add_executable(require_from_dll test/require_from_dll.cpp)
target_link_libraries(require_from_dll PRIVATE ${PROJECT_NAME} ${LUA_LIBRARIES} sol2::sol2)
if (DEFINED LUA_INCLUDE_DIR)
  target_include_directories(require_from_dll PRIVATE ${LUA_INCLUDE_DIR})
endif()
if (DEFINED OpenCV_INCLUDE_DIRS)
  target_include_directories(require_from_dll PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()
