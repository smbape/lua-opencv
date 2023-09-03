cmake_minimum_required(VERSION 3.25)

# https://www.youtube.com/watch?v=mczb2COlt1g

# Name of the project (will be the name of the plugin)
project(opencv_lua)
enable_language(ASM)

set(DEFAULT_BUILD_TYPE "Release")

if((NOT DEFINED CMAKE_BUILD_TYPE) OR ("${CMAKE_BUILD_TYPE} " STREQUAL " "))
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
endif()

# Set the possible values of build type for cmake-gui
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;MinSizeRel;RelWithDebInfo" CACHE STRING "Configs" FORCE)
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${CMAKE_CONFIGURATION_TYPES}")

option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC AND NOT (CMAKE_GENERATOR STREQUAL Ninja))
  add_compile_options(/MP)
endif()

foreach(env_var BINDIR DIR INCDIR)
  if (DEFINED LUA_${env_var} AND "${LUA_${env_var}} " STREQUAL " ")
    unset(LUA_${env_var} CACHE)
  endif()

  if (DEFINED LUA_${env_var})
    set(ENV{LUA_${env_var}} "${LUA_${env_var}}")
  endif()
endforeach()

# find LuaJIT
if (NOT DEFINED ENV{LUA_DIR})
  set(Lua_VERSION 5.4.6 CACHE STRING "Choose the Lua version.")
  set_property(CACHE Lua_VERSION PROPERTY STRINGS "luajit-2.1.0-beta3" "5.4.6" "5.3.6" "5.2.4" "5.1.5")

  if (Lua_VERSION MATCHES "^luajit-")
    string(SUBSTRING "${Lua_VERSION}" 7 -1 Luajit_VERSION)
    set(LUA_SUFFIX "jit")
  else()
    set(LUA_SUFFIX "")
    unset(Luajit_VERSION CACHE)
  endif()

  add_subdirectory(lua${LUA_SUFFIX})

  set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT lua${LUA_SUFFIX})
  file(TO_NATIVE_PATH "${${CMAKE_CURRENT_SOURCE_DIR}/test}" VS_DEBUGGER_WORKING_DIRECTORY)
  set_target_properties(lua${LUA_SUFFIX} PROPERTIES
    VS_DEBUGGER_COMMAND_ARGUMENTS test.lua
    VS_DEBUGGER_WORKING_DIRECTORY "${VS_DEBUGGER_WORKING_DIRECTORY}"
  )
else()
  if (WITH_LUA_ROCKS)
    unset(LUA_INCLUDE_DIR CACHE)
    unset(LUA_INTERPRETER CACHE)
    unset(LUA_LIBRARY CACHE)
  endif()

  message(STATUS "LUA_BINDIR=${LUA_BINDIR}")
  message(STATUS "LUA_DIR=${LUA_DIR}")
  message(STATUS "LUA_INCDIR=${LUA_INCDIR}")

  if (Lua_VERSION MATCHES "^luajit-")
    string(SUBSTRING "${Lua_VERSION}" 7 -1 Luajit_VERSION)
    set(Lua_VERSION 5.1)
    set(LUA_SUFFIX "jit")
  else()
    set(LUA_SUFFIX "")
    unset(Luajit_VERSION CACHE)
  endif()

  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

  if (Lua_VERSION)
    find_package(Lua ${Lua_VERSION} EXACT REQUIRED)
  else()
    find_package(Lua REQUIRED)
  endif()

  unset(_lua_interpreter_names)

  if (Lua_VERSION)
    if (Luajit_VERSION)
      set(_lua_interpreter_names luajit-${Luajit_VERSION})
    else()
      set(_lua_interpreter_names lua-${Lua_VERSION})
    endif()
  else()
    if (LUA_VERSION_STRING)
      set(_lua_interpreter_names
        luajit-${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua${LUA_VERSION_MAJOR}${LUA_VERSION_MINOR}
        lua${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua-${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua.${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        )
    endif()
    list(APPEND _lua_interpreter_names luajit lua)
  endif()

  set(LUA_HINTS ENV LUA_DIR)
  if (LUA_BINDIR)
    list(PREPEND LUA_HINTS "${LUA_BINDIR}")
  endif()

  find_program(LUA_INTERPRETER
    NAMES ${_lua_interpreter_names}
    NAMES_PER_DIR
    HINTS ${LUA_HINTS}
    PATH_SUFFIXES bin
    REQUIRED
  )

  unset(_lua_interpreter_names)
endif()

message(STATUS "LUA_VERSION_STRING = ${LUA_VERSION_STRING}")
message(STATUS "LUA_LIBRARIES = ${LUA_LIBRARIES}")
message(STATUS "LUA_INCLUDE_DIR = ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_INTERPRETER = ${LUA_INTERPRETER}")

add_subdirectory(luarocks)

set(LUA_ONLY "$ENV{LUA_ONLY}")
if (LUA_ONLY)
  message(STATUS "Processing Lua related targets only")
  unset(LUA_INCLUDE_DIR CACHE)
  unset(LUA_INTERPRETER CACHE)
  unset(LUA_LIBRARY CACHE)
  return()
endif()

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

# find OpenCV
if ((NOT DEFINED OpenCV_DIR) AND (NOT DEFINED OpenCV_LIBS))
  add_subdirectory(opencv)
else()
  find_package(OpenCV REQUIRED)
endif()

set(SOL2_VERSION 3.3.0 CACHE STRING "Choose the sol2 version.")
set_property(CACHE SOL2_VERSION PROPERTY STRINGS "3.3.0" "3.2.2")

# Tell cmake that we will need sol2. These command will pull in sol2.
include(FetchContent)
FetchContent_Declare(sol2
  GIT_REPOSITORY https://github.com/ThePhD/sol2.git
  GIT_TAG        v${SOL2_VERSION}
)
FetchContent_MakeAvailable(sol2)

include(cmake/version.cmake)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

# generate source files
if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/node_modules")
  if (WIN32)
    set(npm npm.cmd)
  else()
    set(npm npm)
  endif()
  execute_process(
    COMMAND ${npm} ci
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
  )
endif()

set(ENV{CMAKE_CURRENT_BINARY_DIR} "${CMAKE_CURRENT_BINARY_DIR}")
execute_process(
  COMMAND node --unhandled-rejections=strict --trace-uncaught --trace-warnings generator/index.js
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  COMMAND_ECHO STDERR
  COMMAND_ERROR_IS_FATAL ANY
)

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
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    # PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
    #                 GROUP_EXECUTE GROUP_READ
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

if (NOT WITH_LUA_ROCKS)
  install(TARGETS ${PROJECT_NAME}
    ARCHIVE
      DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  )
endif()

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
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
if (DEFINED OpenCV_INCLUDE_DIRS)
  target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"

  # opencv is in ${CMAKE_INSTALL_LIBDIR}/
  # opencv is in ${CMAKE_INSTALL_LIBDIR}/
  # use rpath $ORIGIN to allow finding opencv
  INSTALL_RPATH "$ORIGIN"
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
if (MSVC AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus" "/bigobj")
endif()

IF (WIN32 AND CMAKE_GENERATOR STREQUAL Ninja)
  target_compile_options(${PROJECT_NAME} PRIVATE "/Zc:__cplusplus" "/bigobj")
  target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32 _WINDOWS _WINDLL)
ENDIF()

# Test
include(CTest)
enable_testing()

add_test(
  NAME test_build
  COMMAND "${LUA_INTERPRETER}" test.lua
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test"
)

set_tests_properties(test_build PROPERTIES DEPENDS ${PROJECT_NAME})

if(EXISTS "${CMAKE_INSTALL_PREFIX}/bin/lua${LUA_SUFFIX}${CMAKE_EXECUTABLE_SUFFIX}")
  add_test(
    NAME test_install
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/lua${LUA_SUFFIX}${CMAKE_EXECUTABLE_SUFFIX}" test.lua
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/test"
  )

  set_tests_properties(test_install PROPERTIES DEPENDS ${PROJECT_NAME})
endif()

# Pack
include(CPack)
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(cmake/vcpkg_regex_replace_string.cmake)

file(GLOB_RECURSE files_to_change
  README.md
  generator/index.js
  luarocks/opencv_lua-scm-1.rockspec
  samples/*.lua
  scripts/pack.js
)

foreach(item ${files_to_change})
  vcpkg_regex_replace_string("${item}" "(/lua-opencv/releases/download/v|/lua-opencv/tree/v)[0-9]+\\.[0-9]+\\.[0-9]+" "\\1${PROJECT_VERSION}")
  vcpkg_regex_replace_string("${item}" "(opencv-|/opencv/opencv/blob/|opencv_lua )[0-9]+\\.[0-9]+\\.[0-9]+" "\\1${OpenCV_VERSION}")
  vcpkg_regex_replace_string("${item}" "(--branch )[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/opencv/opencv)" "\\1${OpenCV_VERSION}\\2")
  vcpkg_regex_replace_string("${item}" "(--branch )[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/smbape/lua-opencv)" "\\1${PROJECT_VERSION}\\2")

  # Force LF EOL
  configure_file(${item} ${item} @ONLY NEWLINE_STYLE LF)
endforeach()
