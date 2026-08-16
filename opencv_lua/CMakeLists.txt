cmake_minimum_required(VERSION 3.25)

set(VERSION_REG "[0-9]+\\.[0-9]+\\.[0-9]+([+-][0-9a-zA-Z.+-]+)?")

# ================================================================
# bit.lua
# ================================================================
function(opencv_write_bit_lua)
    set(opencv_GENERATED_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/generated")
    file(READ "${CMAKE_SOURCE_DIR}/src/lua/bit.lua" _new_bit_string)

    set(_new_bit_string "R\"cdata(${_new_bit_string})cdata\"")
    set(bit_lua_FILE "${opencv_GENERATED_INCLUDE_DIR}/bit_string.lua.inc")

    if(EXISTS "${bit_lua_FILE}")
        file(READ "${bit_lua_FILE}" _old_bit_string)
    else()
        set(_old_bit_string "")
    endif()

    if(NOT (_old_bit_string STREQUAL _new_bit_string))
        file(WRITE "${bit_lua_FILE}" "${_new_bit_string}")
    endif()
endfunction()

opencv_write_bit_lua()
# ================================================================

# ===============
# OpenCV Lua
# ===============
set(target_name opencv_lua)
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_BINARY_DIR}/bin${LUA_MULTI_CONFIG_SUFFIX}" CACHE INTERNAL "Output directory for applications" FORCE)
set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib${LUA_MULTI_CONFIG_SUFFIX}" CACHE INTERNAL "Output directory for libraries" FORCE)

include(GNUInstallDirs)
include(version.cmake)
include(../cmake/vcpkg_regex_replace_string.cmake)

file(RELATIVE_PATH relative_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}")

file(GLOB_RECURSE files_to_change
    ${relative_SOURCE_DIR}README.md
    ${relative_SOURCE_DIR}docs/hosting-you-own-binary-rocks-*
    ${relative_SOURCE_DIR}generator/index.js
    ${relative_SOURCE_DIR}luarocks/${target_name}-scm-1.rockspec
    ${relative_SOURCE_DIR}samples/*.lua
    ${relative_SOURCE_DIR}samples/*.py
    ${relative_SOURCE_DIR}scripts/new_version.lua
    ${relative_SOURCE_DIR}scripts/manual.sh
    ${relative_SOURCE_DIR}scripts/pack.js
    ${relative_SOURCE_DIR}scripts/tasks.sh
)

foreach(item IN LISTS files_to_change)
    vcpkg_regex_replace_string("${item}" "(/lua-opencv/releases/download/v|/lua-opencv/tree/v)${VERSION_REG}" "\\1${PROJECT_VERSION}")
    vcpkg_regex_replace_string("${item}" "(opencv-|/opencv/opencv/blob/|OPENCV_VERSION:-|${target_name} |${target_name}-)[0-9]+\\.[0-9]+\\.[0-9]+" "\\1${OpenCV_VERSION}")
    vcpkg_regex_replace_string("${item}" "(--branch )[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/opencv/opencv)" "\\1${OpenCV_VERSION}\\2")
    vcpkg_regex_replace_string("${item}" "(--branch v)${VERSION_REG}( https://github.com/smbape/lua-opencv)" "\\1${PROJECT_VERSION}\\3")
    vcpkg_regex_replace_string("${item}" "opencv_videoio_ffmpeg[0-9]+_64\\.dll" "opencv_videoio_ffmpeg${OpenCV_DLLVERSION}_64.dll")
    vcpkg_regex_replace_string("${item}" "opencv_world[0-9]+\\.dll" "opencv_world${OpenCV_DLLVERSION}.dll")
endforeach()

option(SKIP_bindings_generation "Skip binding files generation" OFF)

if (NOT SKIP_bindings_generation)
    set(ENV{CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}")
    set(ENV{Python3_EXECUTABLE} "${Python3_EXECUTABLE}")
    execute_process(
        COMMAND "${NODE_EXECUTABLE}" --unhandled-rejections=strict --trace-uncaught --trace-warnings generator/index.js
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMAND_ECHO STDERR
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

file(GLOB_RECURSE project_source_headers "${relative_SOURCE_DIR}src/*.h" "${relative_SOURCE_DIR}src/*.hpp")
list_set_filename_component(ABSOLUTE project_source_headers)
source_group("Source Headers" FILES ${project_source_headers})

file(GLOB_RECURSE project_sources_files "${relative_SOURCE_DIR}src/*.c" "${relative_SOURCE_DIR}src/*.cpp")
list_set_filename_component(ABSOLUTE project_sources_files)
source_group("Source Files" FILES ${project_sources_files})


file(GLOB_RECURSE project_generated_headers "${relative_SOURCE_DIR}generated/*.h" "${relative_SOURCE_DIR}generated/*.hpp")
list_set_filename_component(ABSOLUTE project_generated_headers)
source_group("Generated Headers" FILES ${project_generated_headers})

file(GLOB_RECURSE project_generated_files "${relative_SOURCE_DIR}generated/*.c" "${relative_SOURCE_DIR}generated/*.cpp")
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

# ========================================================================
# Workaround prepcompiled header not regenerated when source files change
# ========================================================================
set(target_PRECOMPILED_HEADER "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.hxx")

if (WIN32)
    set(target_PRECOMPILED_HEADER_CXX "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.cxx")
else()
    set(target_PRECOMPILED_HEADER_CXX "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${target_name}.dir/cmake_pch.hxx.cxx")
endif()

# Any change in these files should trigger precompile header recompilation
set(target_PRECOMPILED_HEADER_DEPENDS
    ${project_source_headers}
    ${project_generated_headers}
)

# Any change in target_precompile_headers should trigger precompile header recompilation
if (EXISTS "${target_PRECOMPILED_HEADER}")
    list(APPEND target_PRECOMPILED_HEADER_DEPENDS "${target_PRECOMPILED_HEADER}")
endif()

add_custom_command(OUTPUT "${target_PRECOMPILED_HEADER_CXX}"
    COMMAND           "${CMAKE_COMMAND}" -E touch_nocreate "${target_PRECOMPILED_HEADER_CXX}"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS           ${target_PRECOMPILED_HEADER_DEPENDS}
    COMMENT           "Generate precompiled header"
    VERBATIM
    USES_TERMINAL
)
add_custom_target(${target_name}_precompiled_header DEPENDS ${target_PRECOMPILED_HEADER_DEPENDS})
add_dependencies(${target_name} ${target_name}_precompiled_header)
# ========================================================================

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

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if (APPLE)
        target_link_options(${target_name} PRIVATE "-Wl,-undefined,error")
    else()
        target_link_options(${target_name} PRIVATE "-Wl,--no-undefined")
    endif()
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

if (WIN32 AND CMAKE_GENERATOR STREQUAL "Ninja")
    target_compile_definitions(${target_name} PRIVATE _WINDLL)
endif()

# ===============
# OpenCV Lua Install
# ===============
if (WITH_LUA_ROCKS)
    file(RELATIVE_PATH INST_LIBDIR "${CMAKE_INSTALL_PREFIX}" "${LUA_INSTALL_LIBDIR}")
    file(RELATIVE_PATH INST_LUADIR "${CMAKE_INSTALL_PREFIX}" "${LUA_INSTALL_LUADIR}")

    set(LIBRAY_DESTINATION "${INST_LIBDIR}")
    set(ARCHIVE_DESTINATION "${INST_LIBDIR}/${target_name}/devel/lib")
    set(CMAKE_CONFIG_DESTINATION "${INST_LIBDIR}/${target_name}/devel/lib/cmake")
    set(LUAMOD_DESTINATION "${INST_LUADIR}")
else()
    set(LIBRAY_DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    set(ARCHIVE_DESTINATION "${CMAKE_INSTALL_LIBDIR}")
    set(CMAKE_CONFIG_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake")
    set(LUAMOD_DESTINATION "lua")
endif()

# https://cmake.org/cmake/help/latest/guide/tutorial/Adding%20Export%20Configuration.html

set(PACKAGE_NAME OpenCVLua)
set(TARGET_EXPORT_NAME ${PACKAGE_NAME}Targets)
set(CMAKE_CONFIG_DESTINATION "${CMAKE_CONFIG_DESTINATION}/${PACKAGE_NAME}")

install(TARGETS ${target_name}
    EXPORT ${TARGET_EXPORT_NAME}
    LIBRARY DESTINATION "${LIBRAY_DESTINATION}"
    RUNTIME DESTINATION "${LIBRAY_DESTINATION}"
    ARCHIVE DESTINATION "${ARCHIVE_DESTINATION}"
    INCLUDES DESTINATION "${OPENCV_INCLUDE_INSTALL_PATH}"
)

install(EXPORT ${TARGET_EXPORT_NAME}
    FILE "${TARGET_EXPORT_NAME}.cmake"
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

export(EXPORT ${TARGET_EXPORT_NAME}
    FILE "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_EXPORT_NAME}.cmake"
)

if (UNIX AND Qt5_DIR)
    unset(Qt5_PLUGINS_DIR)
    if (EXISTS "${Qt5_DIR}/../../../plugins")
        set(Qt5_PLUGINS_DIR "${Qt5_DIR}/../../../plugins")
    elseif (EXISTS "${Qt5_DIR}/../../qt5/plugins")
        set(Qt5_PLUGINS_DIR "${Qt5_DIR}/../../qt5/plugins")
    endif()
    if (Qt5_PLUGINS_DIR)
        install(FILES "${Qt5_PLUGINS_DIR}/platforms/libqxcb.so" DESTINATION "${LIBRAY_DESTINATION}/${target_name}/qt/plugins/platforms")
    endif()

    if (NOT APPLE)
        if (WITH_QT)
            set(WITH_QT_REQUIRED REQUIRED)
        else()
            unset(WITH_QT_REQUIRED)
        endif()

        find_path(DEJAVU_FONT
            NAMES DejaVuSans.ttf
            PATHS /usr/share/fonts
            PATH_SUFFIXES dejavu truetype/dejavu
            NO_DEFAULT_PATH
            NO_PACKAGE_ROOT_PATH
            NO_CMAKE_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH
            NO_CMAKE_INSTALL_PREFIX
            NO_CMAKE_FIND_ROOT_PATH
            ${WITH_QT_REQUIRED}
        )

        if (DEJAVU_FONT)
            file(GLOB_RECURSE DEJAVU_FONTS "${DEJAVU_FONT}/*.ttf")
            install(FILES ${DEJAVU_FONTS} DESTINATION "${LIBRAY_DESTINATION}/${target_name}/qt/fonts")
        endif()
    endif()
endif()

# copy FFmpeg dll to the output folder
if (WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(FFMPEG_SUFFIX _64)
    endif()
    set(ffmpeg_dir "${OpenCV_BINARY_DIR}/3rdparty/ffmpeg")
    set(ffmpeg_bare_name "opencv_videoio_ffmpeg${FFMPEG_SUFFIX}.dll")
    set(ffmpeg_bare_name_ver "opencv_videoio_ffmpeg${OPENCV_DLLVERSION}${FFMPEG_SUFFIX}.dll")
    set(ffmpeg_path "${ffmpeg_dir}/${ffmpeg_bare_name}")
    install(FILES "${ffmpeg_path}" DESTINATION "${LIBRAY_DESTINATION}/${target_name}" RENAME "${ffmpeg_bare_name_ver}")
endif()

# ===============
# OpenCV Lua Repair
# ===============
install(CODE "
set(TARGET_EXPORT_NAME ${TARGET_EXPORT_NAME})
set(LIBRAY_DESTINATION ${LIBRAY_DESTINATION})
set(ARCHIVE_DESTINATION ${ARCHIVE_DESTINATION})
set(CMAKE_CONFIG_DESTINATION ${CMAKE_CONFIG_DESTINATION})
set(LUAMOD_DESTINATION ${LUAMOD_DESTINATION})
")

if (WITH_LUA_ROCKS)
    install(SCRIPT "export_targets_repair.cmake")
endif()


option(ENABLE_REPAIR "Repair installed library to ease distribution" OFF)

if (ENABLE_REPAIR)
    if (WIN32)
        add_library(${target_name}_loader SHARED loader.cpp)

        target_compile_definitions(${target_name}_loader PRIVATE
            LUA_MODULE_NAME=${target_name}
            LUA_MODULE_VERSION=${LUA_MODULE_VERSION}
            LUA_MODULE_LIB_NAME=Allegro5
            LUA_MODULE_LIB_VERSION=${Allegro5_VERSION}
            LUAAPI_EXPORTS
        )

        target_include_directories(${target_name}_loader PRIVATE
            "${CMAKE_SOURCE_DIR}/src/include"
        )

        target_link_libraries(${target_name}_loader PRIVATE ${LUA_LIBRARIES})
        target_include_directories(${target_name}_loader PRIVATE ${LUA_INCLUDE_DIR})

        install(TARGETS ${target_name}_loader
            LIBRARY DESTINATION "${LIBRAY_DESTINATION}/${target_name}"
            RUNTIME DESTINATION "${LIBRAY_DESTINATION}/${target_name}"
            ARCHIVE DESTINATION "${ARCHIVE_DESTINATION}/${target_name}"
        )

        set(REPAIR_DIR repaired)

        set(repair_CODE
            "set(TARGET_NAME ${target_name})"
            "set(ORIGIN_FILE \"$<TARGET_FILE:${target_name}>\")"
            "set(PROJECT_VERSION ${PROJECT_VERSION})"
            "set(Python3_EXECUTABLE \"${Python3_EXECUTABLE}\")"
            "set(TARGET_SOURCE_DIR \"${CMAKE_SOURCE_DIR}\")"
            "set(TARGET_CURRENT_SOURCE_DIR \"${CMAKE_CURRENT_SOURCE_DIR}\")"
            "set(TARGET_CURRENT_BINARY_DIR \"${CMAKE_CURRENT_BINARY_DIR}\")"
            "set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})"
            "set(REPAIR_DIR ${REPAIR_DIR})"
            "set(PACKAGE_DATA \"${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}\")"
            "unset(DLL_DIRECTORIES)"
        )

        if (VCPKG_INSTALLED_DIR)
            if (CMAKE_BUILD_TYPE STREQUAL "Debug")
                list(APPEND repair_CODE "list(APPEND DLL_DIRECTORIES \"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/debug/bin\")")
            endif()

            list(APPEND repair_CODE "list(APPEND DLL_DIRECTORIES \"${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin\")")
        endif()

        foreach(flag IN ITEMS ignore_existing ignore_in_wheel no_mangle_all with_mangle strip custom_patch include_symbols include_imports)
            if (DELVEWHEEL_${flag})
                list(APPEND repair_CODE "set(DELVEWHEEL_${flag} ON)")
            endif()
        endforeach()

        foreach(option IN ITEMS add_path include add_dll exclude no_dll wheel_dir no_mangle lib_sdir namespace_pkg)
            foreach(value IN LISTS DELVEWHEEL_${option})
                list(APPEND repair_CODE "list(APPEND DELVEWHEEL_${option} \"${value}\")")
            endforeach()
        endforeach()

        list(JOIN repair_CODE "\n  " repair_CODE)
        install(CODE "${repair_CODE}")

        install(SCRIPT "delvewheel_repair.cmake")
    elseif(UNIX)
        set(REPAIR_DIR repaired)

        set(repair_CODE
            "set(TARGET_NAME ${target_name})"
            "set(ORIGIN_FILE \"$<TARGET_FILE:${target_name}>\")"
            "set(PROJECT_VERSION ${PROJECT_VERSION})"
            "set(Python3_EXECUTABLE \"${Python3_EXECUTABLE}\")"
            "set(TARGET_SOURCE_DIR \"${CMAKE_SOURCE_DIR}\")"
            "set(TARGET_CURRENT_SOURCE_DIR \"${CMAKE_CURRENT_SOURCE_DIR}\")"
            "set(TARGET_CURRENT_BINARY_DIR \"${CMAKE_CURRENT_BINARY_DIR}\")"
            "set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE})"
            "set(REPAIR_DIR ${REPAIR_DIR})"
            "set(PACKAGE_DATA \"${target_name}${CMAKE_SHARED_LIBRARY_SUFFIX}\")"
        )

        foreach(flag IN ITEMS strip only_plat disable_isa_ext_check)
            if (AUDITWHEEL_${flag})
                list(APPEND repair_CODE "set(AUDITWHEEL_${flag} ON)")
            endif()
        endforeach()

        foreach(option IN ITEMS plat exclude)
            foreach(value IN LISTS AUDITWHEEL_${option})
                list(APPEND repair_CODE "list(APPEND AUDITWHEEL_${option} \"${value}\")")
            endforeach()
        endforeach()

        if (Qt5_DIR AND Qt5_PLUGINS_DIR)
            list(APPEND repair_CODE "list(APPEND PACKAGE_DATA \"${target_name}/qt/plugins/platforms/libqxcb.so\")")
        endif()

        list(JOIN repair_CODE "\n  " repair_CODE)
        install(CODE "${repair_CODE}")

        install(SCRIPT "auditwheel_repair.cmake")
    endif()
endif()

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

# Pack
include(CPack)
set(CPACK_PROJECT_NAME ${target_name})
set(CPACK_PROJECT_VERSION ${LUA_MODULE_VERSION})
