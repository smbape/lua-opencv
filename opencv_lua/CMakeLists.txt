# ===============
# OpenCV Lua
# ===============
set(target_name opencv_lua)

include(GNUInstallDirs)
include(version.cmake)

function(list_set_filename_component FileNames)
  list(SUBLIST ARGV 1 -1 ARGS)

  set(index 0)
  foreach(FileName IN LISTS ${FileNames})
    get_filename_component(FileName "${FileName}" ${ARGS})
    list(REMOVE_AT ${FileNames} ${index})
    list(INSERT ${FileNames} ${index} "${FileName}")
    math(EXPR index "${index}+1")
  endforeach()

  set(${FileNames} ${${FileNames}} PARENT_SCOPE)
endfunction()

file(RELATIVE_PATH CMAKE_RELATIVE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}")

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin" CACHE PATH "Output directory for applications")
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib" CACHE PATH "Output directory for libraries")

# generate source files
if(NOT EXISTS "${CMAKE_SOURCE_DIR}/node_modules")
  if (WIN32)
    set(npm npm.cmd)
  else()
    set(npm npm)
  endif()
  execute_process(
    COMMAND ${npm} ci
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
  )
endif()

set(ENV{CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}")
execute_process(
  COMMAND node --unhandled-rejections=strict --trace-uncaught --trace-warnings generator/index.js
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  COMMAND_ECHO STDERR
  COMMAND_ERROR_IS_FATAL ANY
)

file(GLOB_RECURSE project_source_headers "${CMAKE_RELATIVE_SOURCE_DIR}src/*.h" "${CMAKE_RELATIVE_SOURCE_DIR}src/*.hpp")
list_set_filename_component(project_source_headers ABSOLUTE)
source_group("Source Headers" FILES ${project_source_headers})

file(GLOB_RECURSE project_sources_files "${CMAKE_RELATIVE_SOURCE_DIR}src/*.c" "${CMAKE_RELATIVE_SOURCE_DIR}src/*.cpp")
list_set_filename_component(project_sources_files ABSOLUTE)
source_group("Source Files" FILES ${project_sources_files})


file(GLOB_RECURSE project_generated_headers "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.h" "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.hpp")
list_set_filename_component(project_generated_headers ABSOLUTE)
source_group("Generated Headers" FILES ${project_generated_headers})

file(GLOB_RECURSE project_generated_files "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.c" "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.cpp")
list_set_filename_component(project_generated_files ABSOLUTE)
source_group("Generated Files" FILES ${project_generated_files})


set(SRC_FILES
  ${project_source_headers}
  ${project_sources_files}

  ${project_generated_headers}
  ${project_generated_files}
)

# Build a shared library named as the project
if(POLICY CMP0091)
  cmake_policy(GET CMP0091 MSVC_RUNTIME_SET_BY_ABSTRACTION)
endif()

if(NOT BUILD_SHARED_LIBS AND BUILD_WITH_STATIC_CRT AND MSVC_RUNTIME_SET_BY_ABSTRACTION STREQUAL "NEW")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(${target_name} SHARED ${SRC_FILES})

install(TARGETS ${target_name}
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)

if (NOT WITH_LUA_ROCKS)
  install(TARGETS ${target_name}
    ARCHIVE
      DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  )
else()
  if (CMAKE_INSTALL_PREFIX)
    # Add any variables you need during post install.
    install(CODE "set(CMAKE_INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")
    install(CODE "set(TARGET_FILE \"$<TARGET_FILE:${target_name}>\")")

    # CMake will execute this last in the build.
    # Install the script that does the post install work.
    install(SCRIPT "PostInstall.cmake")
  endif()
endif()

target_compile_definitions(${target_name}
  PUBLIC
    LUA_MODULE_NAME=${target_name}
    LUA_MODULE_VERSION=${LUA_MODULE_VERSION}
    LUA_MODULE_LIB_NAME=OpenCV
    LUA_MODULE_LIB_VERSION=${OpenCV_VERSION}
  PRIVATE LUAAPI_EXPORTS)
target_precompile_headers(${target_name} PRIVATE
  "$<$<COMPILE_LANGUAGE:CXX>:lua_generated_pch.hpp>"
)

target_link_libraries(${target_name} PRIVATE ${LUA_LIBRARIES} ${OpenCV_LIBS} Eigen3::Eigen)
target_include_directories(${target_name} PRIVATE ${LUA_INCLUDE_DIR})
if (DEFINED OpenCV_INCLUDE_DIRS)
  target_include_directories(${target_name} PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()

target_include_directories(${target_name} PUBLIC "${CMAKE_SOURCE_DIR}/src/include" "${CMAKE_SOURCE_DIR}/src" "${CMAKE_SOURCE_DIR}/generated")

if (WITH_LUA_ROCKS)
  include(../cmake/set_startup_project.cmake)
endif()

set_target_properties(${target_name} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  COMPILE_PDB_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
  RUNTIME_OUTPUT_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
)

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(${target_name} PROPERTIES LINK_FLAGS "/DEBUG")
endif()

# /Zc:__cplusplus is required to make __cplusplus accurate
# /Zc:__cplusplus is available starting with Visual Studio 2017 version 15.7
# (according to https://docs.microsoft.com/en-us/cpp/build/reference/zc-cplusplus)
# That version is equivalent to _MSC_VER==1914
# (according to https://docs.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=vs-2019)
# CMake's ${MSVC_VERSION} is equivalent to _MSC_VER
# (according to https://cmake.org/cmake/help/latest/variable/MSVC_VERSION.html#variable:MSVC_VERSION)
if (MSVC AND (MSVC_VERSION GREATER_EQUAL 1914))
  target_compile_options(${target_name} PRIVATE "/Zc:__cplusplus" "/bigobj")
endif()

IF (WIN32 AND CMAKE_GENERATOR STREQUAL Ninja)
  target_compile_options(${target_name} PRIVATE "/Zc:__cplusplus" "/bigobj")
  target_compile_definitions(${target_name} PRIVATE _WINDLL)
ENDIF()

# ===============
# OpenCV Lua Test
# ===============
include(CTest)
enable_testing()

add_test(
  NAME test_build
  COMMAND "${LUA_INTERPRETER}" "samples/01-show-image.lua"
  WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

set_tests_properties(test_build PROPERTIES DEPENDS ${target_name})

if(EXISTS "${CMAKE_INSTALL_PREFIX}/bin/lua${LUA_SUFFIX}${CMAKE_EXECUTABLE_SUFFIX}")
  add_test(
    NAME test_install
    COMMAND "${CMAKE_INSTALL_PREFIX}/bin/lua${LUA_SUFFIX}${CMAKE_EXECUTABLE_SUFFIX}" "samples/01-show-image.lua"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  )

  set_tests_properties(test_install PROPERTIES DEPENDS ${target_name})
endif()

# Pack
include(CPack)
set(CPACK_PROJECT_NAME ${target_name})
set(CPACK_PROJECT_VERSION ${LUA_MODULE_VERSION})

include(../cmake/vcpkg_regex_replace_string.cmake)

file(GLOB_RECURSE files_to_change
  ${CMAKE_RELATIVE_SOURCE_DIR}README.md
  ${CMAKE_RELATIVE_SOURCE_DIR}docs/hosting-you-own-binary-rocks-*
  ${CMAKE_RELATIVE_SOURCE_DIR}generator/index.js
  ${CMAKE_RELATIVE_SOURCE_DIR}luarocks/${target_name}-scm-1.rockspec
  ${CMAKE_RELATIVE_SOURCE_DIR}samples/*.lua
  ${CMAKE_RELATIVE_SOURCE_DIR}scripts/new_version.lua
  ${CMAKE_RELATIVE_SOURCE_DIR}scripts/pack.js
)

foreach(item ${files_to_change})
  vcpkg_regex_replace_string("${item}" "(/lua-opencv/releases/download/v|/lua-opencv/tree/v)[0-9]+\\.[0-9]+\\.[0-9]+" "\\1${PROJECT_VERSION}")
  vcpkg_regex_replace_string("${item}" "(opencv-|/opencv/opencv/blob/|${target_name} )[0-9]+\\.[0-9]+\\.[0-9]+" "\\1${OpenCV_VERSION}")
  vcpkg_regex_replace_string("${item}" "(--branch )[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/opencv/opencv)" "\\1${OpenCV_VERSION}\\2")
  vcpkg_regex_replace_string("${item}" "(--branch v)[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/smbape/lua-opencv)" "\\1${PROJECT_VERSION}\\2")
  vcpkg_regex_replace_string("${item}" "opencv_videoio_ffmpeg[0-9]+_64\\.dll" "opencv_videoio_ffmpeg${OpenCV_DLLVERSION}_64.dll")
  vcpkg_regex_replace_string("${item}" "opencv_world[0-9]+\\.dll" "opencv_world${OpenCV_DLLVERSION}.dll")

  # Force LF EOL
  configure_file(${item} ${item} @ONLY NEWLINE_STYLE LF)
endforeach()
