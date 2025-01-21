cmake_minimum_required(VERSION 3.25)

# ===============
# OpenCV Lua
# ===============
set(target_name opencv_lua)

include(GNUInstallDirs)
include(version.cmake)
include(../cmake/vcpkg_regex_replace_string.cmake)

file(RELATIVE_PATH CMAKE_RELATIVE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE files_to_change
  ${CMAKE_RELATIVE_SOURCE_DIR}README.md
  ${CMAKE_RELATIVE_SOURCE_DIR}docs/hosting-you-own-binary-rocks-*
  ${CMAKE_RELATIVE_SOURCE_DIR}generator/index.js
  ${CMAKE_RELATIVE_SOURCE_DIR}luarocks/${target_name}-scm-1.rockspec
  ${CMAKE_RELATIVE_SOURCE_DIR}samples/*.lua
  ${CMAKE_RELATIVE_SOURCE_DIR}scripts/new_version.lua
  ${CMAKE_RELATIVE_SOURCE_DIR}scripts/pack.js
  ${CMAKE_RELATIVE_SOURCE_DIR}scripts/tasks.sh
)

foreach(item IN LISTS files_to_change)
  vcpkg_regex_replace_string("${item}" "(/lua-opencv/releases/download/v|/lua-opencv/tree/v)[0-9]+\\.[0-9]+\\.[0-9]+" "\\1${PROJECT_VERSION}")
  vcpkg_regex_replace_string("${item}" "(opencv-|/opencv/opencv/blob/|${target_name} )[0-9]+\\.[0-9]+\\.[0-9]+" "\\1${OpenCV_VERSION}")
  vcpkg_regex_replace_string("${item}" "OPENCV_VERSION:-[0-9]+\\.[0-9]+\\.[0-9]+" "OPENCV_VERSION:-${OpenCV_VERSION}")
  vcpkg_regex_replace_string("${item}" "(--branch )[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/opencv/opencv)" "\\1${OpenCV_VERSION}\\2")
  vcpkg_regex_replace_string("${item}" "(--branch v)[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/smbape/lua-opencv)" "\\1${PROJECT_VERSION}\\2")
  vcpkg_regex_replace_string("${item}" "opencv_videoio_ffmpeg[0-9]+_64\\.dll" "opencv_videoio_ffmpeg${OpenCV_DLLVERSION}_64.dll")
  vcpkg_regex_replace_string("${item}" "opencv_world[0-9]+\\.dll" "opencv_world${OpenCV_DLLVERSION}.dll")

  # Force LF EOL
  configure_file(${item} ${item} @ONLY NEWLINE_STYLE LF)
endforeach()

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

if (NOT opencv_lua_SKIP_GENERATION)
  set(ENV{CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}")
  set(ENV{Python3_EXECUTABLE} "${Python3_EXECUTABLE}")
  execute_process(
    COMMAND node --unhandled-rejections=strict --trace-uncaught --trace-warnings generator/index.js
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    COMMAND_ECHO STDERR
    COMMAND_ERROR_IS_FATAL ANY
  )
endif()

file(GLOB_RECURSE project_source_headers "${CMAKE_RELATIVE_SOURCE_DIR}src/*.h" "${CMAKE_RELATIVE_SOURCE_DIR}src/*.hpp")
list_set_filename_component(ABSOLUTE project_source_headers)
source_group("Source Headers" FILES ${project_source_headers})

file(GLOB_RECURSE project_sources_files "${CMAKE_RELATIVE_SOURCE_DIR}src/*.c" "${CMAKE_RELATIVE_SOURCE_DIR}src/*.cpp")
list_set_filename_component(ABSOLUTE project_sources_files)
source_group("Source Files" FILES ${project_sources_files})


file(GLOB_RECURSE project_generated_headers "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.h" "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.hpp")
list_set_filename_component(ABSOLUTE project_generated_headers)
source_group("Generated Headers" FILES ${project_generated_headers})

file(GLOB_RECURSE project_generated_files "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.c" "${CMAKE_RELATIVE_SOURCE_DIR}generated/*.cpp")
list_set_filename_component(ABSOLUTE project_generated_files)
source_group("Generated Files" FILES ${project_generated_files})

set(SRC_FILES
  ${project_source_headers}
  ${project_sources_files}

  ${project_generated_headers}
  ${project_generated_files}

  ${OpenCV_SOURCE_OBJECTS}
)

if(POLICY CMP0091)
  cmake_policy(GET CMP0091 MSVC_RUNTIME_SET_BY_ABSTRACTION)
endif()

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(${target_name} SHARED ${SRC_FILES})

if(NOT BUILD_SHARED_LIBS AND (BUILD_WITH_STATIC_CRT OR NOT OpenCV_SHARED) AND MSVC_RUNTIME_SET_BY_ABSTRACTION STREQUAL "NEW")
  set_target_properties(${target_name} PROPERTIES
    MSVC_RUNTIME_LIBRARY  "MultiThreaded$<$<CONFIG:Debug>:Debug>"
  )
endif()

target_compile_definitions(${target_name} PRIVATE
  LUA_MODULE_NAME=${target_name}
  LUA_MODULE_VERSION=${LUA_MODULE_VERSION}
  LUA_MODULE_LIB_NAME=OpenCV
  LUA_MODULE_LIB_VERSION=${OpenCV_VERSION}
  LUAAPI_EXPORTS
)

target_include_directories(${target_name} PRIVATE
  "${CMAKE_SOURCE_DIR}/src/include"
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/generated"
)

target_precompile_headers(${target_name} PRIVATE
  "$<$<COMPILE_LANGUAGE:CXX>:lua_generated_pch.hpp>"
)

target_link_libraries(${target_name} PRIVATE ${LUA_LIBRARIES})
target_include_directories(${target_name} PRIVATE ${LUA_INCLUDE_DIR})

if (DEFINED OpenCV_DEPENDENCIES)
  add_dependencies(${target_name} ${OpenCV_DEPENDENCIES})
endif()

if (DEFINED OpenCV_INCLUDE_DIRS)
  target_include_directories(${target_name} PRIVATE ${OpenCV_INCLUDE_DIRS})
endif()

if (DEFINED OpenCV_LIBS)
  target_link_libraries(${target_name} PRIVATE ${OpenCV_LIBS})
endif()

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
  target_link_options(${target_name} PRIVATE "/DEBUG")
endif()

if(WITH_CUDA AND MSVC AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  target_link_options(${target_name} PRIVATE "/INCREMENTAL:NO")
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

if (WIN32 AND CMAKE_GENERATOR STREQUAL Ninja)
  target_compile_definitions(${target_name} PRIVATE _WINDLL)
endif()

# ===============
# OpenCV Lua Install
# ===============
if (WITH_LUA_ROCKS)
  if (UNIX AND CMAKE_INSTALL_PREFIX)
    if(Qt5_DIR)
      unset(Qt5_PLUGINS_DIR)
      if (EXISTS "${Qt5_DIR}/../../../plugins")
        set(Qt5_PLUGINS_DIR "${Qt5_DIR}/../../../plugins")
      elseif (EXISTS "${Qt5_DIR}/../../qt5/plugins")
        set(Qt5_PLUGINS_DIR "${Qt5_DIR}/../../qt5/plugins")
      endif()
      if (Qt5_PLUGINS_DIR)
        install(FILES "${Qt5_PLUGINS_DIR}/platforms/libqxcb.so" DESTINATION "lib/${target_name}/qt/plugins/platforms")
      endif()

      if (NOT APPLE)
        install(DIRECTORY "/usr/share/fonts/dejavu" DESTINATION "lib/${target_name}/qt/fonts")
      endif()
    endif()

    # Add any variables you need during post install.
    install(CODE "set(Qt5_DIR \"${Qt5_DIR}\")")
    install(CODE "set(target_name \"${target_name}\")")
    install(CODE "set(PROJECT_VERSION \"${PROJECT_VERSION}\")")
    install(CODE "set(Python3_EXECUTABLE \"${Python3_EXECUTABLE}\")")
    install(CODE "set(CMAKE_SOURCE_DIR \"${CMAKE_SOURCE_DIR}\")")
    install(CODE "set(CMAKE_CURRENT_SOURCE_DIR \"${CMAKE_CURRENT_SOURCE_DIR}\")")
    install(CODE "set(CMAKE_INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")
    install(CODE "set(CMAKE_BINARY_DIR \"${CMAKE_BINARY_DIR}\")")
    install(CODE "set(CMAKE_CURRENT_BINARY_DIR \"${CMAKE_CURRENT_BINARY_DIR}\")")
    install(CODE "set(CMAKE_INSTALL_LIBDIR \"lib\")")
    install(CODE "set(TARGET_FILE \"$<TARGET_FILE:${target_name}>\")")

    # CMake will execute this last in the build.
    # Install the script that does the post install work.
    install(SCRIPT "auditwheel_repair.cmake")
  endif()

  set(LIBRAY_DESTINATION lib)
  set(ARCHIVE_DESTINATION lib/${target_name}/devel/lib)
  set(CMAKE_CONFIG_DESTINATION lib/${target_name}/devel/lib/cmake)
else()
  set(LIBRAY_DESTINATION "${CMAKE_INSTALL_LIBDIR}")
  set(ARCHIVE_DESTINATION "${CMAKE_INSTALL_LIBDIR}")
  set(CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
endif()

# https://cmake.org/cmake/help/latest/guide/tutorial/Adding%20Export%20Configuration.html

set(PACKAGE_NAME OpenCVLua)
set(EXPORT_NAME ${PACKAGE_NAME}Targets)
set(CMAKE_CONFIG_DESTINATION "${CMAKE_CONFIG_DESTINATION}/${PACKAGE_NAME}")

install(TARGETS ${target_name}
  EXPORT ${EXPORT_NAME}
  LIBRARY DESTINATION "${LIBRAY_DESTINATION}"
  RUNTIME DESTINATION "${LIBRAY_DESTINATION}"
  ARCHIVE DESTINATION "${ARCHIVE_DESTINATION}"
  INCLUDES DESTINATION "${OPENCV_INCLUDE_INSTALL_PATH}"
)

install(EXPORT ${EXPORT_NAME}
  FILE "${EXPORT_NAME}.cmake"
  NAMESPACE ${target_name}::
  DESTINATION "${CMAKE_CONFIG_DESTINATION}"
)

include(CMakePackageConfigHelpers)

# generate the config file that includes the exports
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
  INSTALL_DESTINATION "${CMAKE_CONFIG_DESTINATION}"
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
  VERSION "${OpenCV_VERSION}"
  COMPATIBILITY AnyNewerVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}ConfigVersion.cmake"
  DESTINATION "${CMAKE_CONFIG_DESTINATION}"
)

export(EXPORT ${EXPORT_NAME}
  FILE "${CMAKE_CURRENT_BINARY_DIR}/${EXPORT_NAME}.cmake"
)

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
