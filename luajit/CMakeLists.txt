# https://askubuntu.com/questions/355565/how-do-i-install-the-latest-version-of-cmake-from-the-command-line
# https://askubuntu.com/a/865294
cmake_minimum_required(VERSION 3.25)

project(luajit LANGUAGES C CXX ASM)

option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)

include(cmake/initialize.cmake)
include(cmake/options.cmake)

set(Luajit_URL_HASH_210beta3 409f7fe570d3c16558e594421c47bdd130238323c9d6fd6c83dedd2aaeb082a8)

include(FetchContent)
FetchContent_Declare(luajit
  # URL                 https://github.com/openresty/luajit2/archive/refs/tags/v2.1-20230410.tar.gz
  URL                 https://github.com/LuaJIT/LuaJIT/archive/refs/tags/v${VERSION}.tar.gz
  URL_HASH            SHA256=${Luajit_URL_HASH_${Luajit_DLLVERSION}}
)
Fetchcontent_Populate(luajit)

set(luajit_SRC "${luajit_SOURCE_DIR}/src")
set(LUA_SHARE_DIR "${luajit_BINARY_DIR}/share/luajit-${VERSION}/jit")
set(LUA_INCLUDE_DIR "${luajit_BINARY_DIR}/include/luajit-${MAJVER}.${MINVER}")

set(DASMDIR "${luajit_SOURCE_DIR}/dynasm")

set(LUA_INCLUDE_DIRS
  "${LUA_INCLUDE_DIR}"
  "${luajit_SRC}"
  "${DASMDIR}"
)

include(testarch/testarch.cmake)
include(cmake/standalone.cmake)
include(minilua/minilua.cmake)
include(buildvm/buildvm.cmake)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "PS3")
  add_compile_definitions(LUAJIT_USE_SYSMALLOC)
  link_libraries(pthread)
endif()

if (NOT ("${CMAKE_INSTALL_PREFIX}" STREQUAL "/usr/local"))
  add_compile_definitions("LUA_ROOT=\"${CMAKE_INSTALL_PREFIX}\"")
endif()

if (NOT ("${CMAKE_INSTALL_LIBDIR} " STREQUAL " "))
  add_compile_definitions("LUA_MULTILIB=\"${CMAKE_INSTALL_LIBDIR}\"")
endif()

if (NOT ("${CMAKE_INSTALL_LIBDIR} " STREQUAL " "))
  add_compile_definitions("LUA_LMULTILIB=\"${CMAKE_INSTALL_LIBDIR}\"")
endif()

if (NOT WIN32)
  include(CheckCCompilerFlag)
  check_c_compiler_flag(-fno-stack-protector NO_STACK_PROTECTOR_FLAG)
  if (NO_STACK_PROTECTOR_FLAG)
    add_compile_options(-fno-stack-protector)
  endif()
endif()

if (NOT WIN32)
  add_compile_options(-fomit-frame-pointer)
endif()

if (ENABLE_LARGEFILE)
  add_compile_definitions(_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
endif()

if (NOT MSVC)
  add_compile_options(-U_FORTIFY_SOURCE)
endif()

if (UNIX)
  find_library(LIBM_LIBRARIES NAMES m REQUIRED)
  link_libraries(${LIBM_LIBRARIES})
endif()

if ((NOT APPLE) AND (("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") OR ("${CMAKE_SYSTEM_NAME}" MATCHES "(Open|Free|Net)BSD")))
  find_library(LIBDL_LIBRARIES NAMES dl REQUIRED)
  link_libraries(${LIBDL_LIBRARIES})
endif()

if (CMAKE_CROSSCOMPILING)
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_LINUX")
  elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_WINDOWS")
  elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_OSX" "TARGET_OS_IPHONE=1")
  elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_OSX" "TARGET_OS_IPHONE=0")
  elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_LINUX")
  elseif ("${CMAKE_SYSTEM_NAME}" STREQUAL "Haiku")
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_POSIX")
  elseif ("${CMAKE_SYSTEM_NAME}" MATCHES "(Open|Free|Net)BSD")
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_BSD")
  else()
    add_compile_definitions("LUAJIT_OS=LUAJIT_OS_OTHER")
  endif()
endif()

if (APPLE)
  set(LJVM_MODE machasm)
elseif (WIN32 OR CYGWIN OR MINGW)
  set(LJVM_MODE peobj)
else()
  set(LJVM_MODE elfasm)
endif()

set(gen_lj_targets "")

# gen_lj_vm
file(MAKE_DIRECTORY "${luajit_BINARY_DIR}/objects")

if (WIN32 OR CYGWIN OR MINGW)
  set(LJ_VM_OBJECT "${luajit_BINARY_DIR}/objects/lj_vm${CMAKE_C_OUTPUT_EXTENSION}")
else()
  set(LJ_VM_OBJECT "${luajit_BINARY_DIR}/objects/lj_vm.S")
endif()

file(TO_NATIVE_PATH "${LJ_VM_OBJECT}" LJ_VM_OBJECT_NATIVE)
add_custom_command(
  OUTPUT "${LJ_VM_OBJECT}"
  COMMAND "${BUILDVM_TARGET_FILE}" -m ${LJVM_MODE} -o "${LJ_VM_OBJECT_NATIVE}"
  WORKING_DIRECTORY "${luajit_BINARY_DIR}"
  DEPENDS buildvm
  COMMENT "Generate lj_vm${CMAKE_C_OUTPUT_EXTENSION}"
)

add_custom_target(gen_lj_vm ALL DEPENDS buildvm "${LJ_VM_OBJECT}")
list(APPEND gen_lj_targets gen_lj_vm)

# gen_lj_headers
set(LJLIB_C
  lib_base.c
  lib_math.c
  lib_bit.c
  lib_string.c
  lib_table.c
  lib_io.c
  lib_os.c
  lib_package.c
  lib_debug.c
  lib_jit.c
  lib_ffi.c
)

set(LJ_BCDEF_HEADER "${LUA_INCLUDE_DIR}/lj_bcdef.h")
file(TO_NATIVE_PATH "${LJ_BCDEF_HEADER}" LJ_BCDEF_HEADER_NATIVE)

set(LJ_FFDEF_HEADER "${LUA_INCLUDE_DIR}/lj_ffdef.h")
file(TO_NATIVE_PATH "${LJ_FFDEF_HEADER}" LJ_FFDEF_HEADER_NATIVE)

set(LJ_LIBDEF_HEADER "${LUA_INCLUDE_DIR}/lj_libdef.h")
file(TO_NATIVE_PATH "${LJ_LIBDEF_HEADER}" LJ_LIBDEF_HEADER_NATIVE)

set(LJ_RECDEF_HEADER "${LUA_INCLUDE_DIR}/lj_recdef.h")
file(TO_NATIVE_PATH "${LJ_RECDEF_HEADER}" LJ_RECDEF_HEADER_NATIVE)

set(LJ_VMDEF_LUA "${LUA_SHARE_DIR}/vmdef.lua")
file(TO_NATIVE_PATH "${LJ_VMDEF_LUA}" LJ_VMDEF_LUA_NATIVE)

set(LJ_FOLDDEF_HEADER "${LUA_INCLUDE_DIR}/lj_folddef.h")
file(TO_NATIVE_PATH "${LJ_FOLDDEF_HEADER}" LJ_FOLDDEF_HEADER_NATIVE)

file(MAKE_DIRECTORY "${LUA_INCLUDE_DIR}" "${LUA_SHARE_DIR}")
add_custom_command(
  OUTPUT
    "${LJ_BCDEF_HEADER}"
    "${LJ_FFDEF_HEADER}"
    "${LJ_LIBDEF_HEADER}"
    "${LJ_RECDEF_HEADER}"
    "${LJ_VMDEF_LUA}"
    "${LJ_FOLDDEF_HEADER}"
  COMMAND "${BUILDVM_TARGET_FILE}" -m bcdef -o "${LJ_BCDEF_HEADER_NATIVE}" ${LJLIB_C}
  COMMAND "${BUILDVM_TARGET_FILE}" -m ffdef -o "${LJ_FFDEF_HEADER_NATIVE}" ${LJLIB_C}
  COMMAND "${BUILDVM_TARGET_FILE}" -m libdef -o "${LJ_LIBDEF_HEADER_NATIVE}" ${LJLIB_C}
  COMMAND "${BUILDVM_TARGET_FILE}" -m recdef -o "${LJ_RECDEF_HEADER_NATIVE}" ${LJLIB_C}
  COMMAND "${BUILDVM_TARGET_FILE}" -m vmdef -o "${LJ_VMDEF_LUA_NATIVE}" ${LJLIB_C}
  COMMAND "${BUILDVM_TARGET_FILE}" -m folddef -o "${LJ_FOLDDEF_HEADER_NATIVE}" lj_opt_fold.c
  WORKING_DIRECTORY "${luajit_SRC}"
  DEPENDS buildvm
)
add_custom_target(gen_lj_headers ALL DEPENDS buildvm "${LJ_FOLDDEF_HEADER}")
list(APPEND gen_lj_targets gen_lj_headers)

file(GLOB_RECURSE libluajit_source_files "${luajit_SRC}/lj_*.c" "${luajit_SRC}/lib_*.c")

add_library(libluajit SHARED ${libluajit_source_files})

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(libluajit PROPERTIES LINK_FLAGS "/DEBUG")
endif()

target_sources(libluajit
  PUBLIC FILE_SET HEADERS TYPE HEADERS
  BASE_DIRS "${luajit_SRC}"
  FILES
    "${luajit_SRC}/lua.h"
    "${luajit_SRC}/lualib.h"
    "${luajit_SRC}/lauxlib.h"
    "${luajit_SRC}/luaconf.h"
    "${luajit_SRC}/lua.hpp"
    "${luajit_SRC}/luajit.h"
)


add_library(libluajit::libluajit ALIAS libluajit)

target_link_libraries(libluajit PRIVATE "${LJ_VM_OBJECT}")
target_include_directories(libluajit PRIVATE ${LUA_INCLUDE_DIRS})
add_dependencies(libluajit ${gen_lj_targets})

IF (WIN32 AND (CMAKE_GENERATOR STREQUAL Ninja))
  target_compile_definitions(libluajit PRIVATE WIN32)
  target_compile_definitions(libluajit PRIVATE _WINDOWS)
  target_compile_definitions(libluajit PRIVATE _WINDLL)
ENDIF()

if (("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin") AND ("${TARGET_LJARCH}" STREQUAL x64))
  target_link_options(libluajit PRIVATE -image_base 7fff04c4a000)
endif()

if ((WIN32 OR CYGWIN OR MINGW) AND BUILD_SHARED_LIBS)
  target_compile_definitions(libluajit PRIVATE LUA_BUILD_AS_DLL)
endif()

if (WIN32 OR CYGWIN OR MINGW)
  set_target_properties(libluajit PROPERTIES
    OUTPUT_NAME lua${NODOTABIVER}
  )
else()
  set_target_properties(libluajit PROPERTIES
    OUTPUT_NAME luajit-${ABIVER}
    VERSION "${VERSION}"
    SOVERSION "${MAJVER}"
  )
endif()

set_target_output_directories(libluajit)

if (UNIX)
  get_target_property(libluajit_OUTPUT_NAME libluajit OUTPUT_NAME)
  add_custom_command(
    TARGET libluajit POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory pkgconfig
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${luajit_SOURCE_DIR}/etc/luajit.pc" pkgconfig/luajit.pc
    COMMAND "${CMAKE_COMMAND}"
      "-DPREFIX=${CMAKE_INSTALL_PREFIX}"
      "-DMULTILIB=${CMAKE_INSTALL_LIBDIR}"
      "-DLIBNAME=${libluajit_OUTPUT_NAME}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkgconfig.cmake"
    WORKING_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    VERBATIM)
  install(DIRECTORY "${LIBRARY_OUTPUT_PATH}/pkgconfig" DESTINATION "${CMAKE_INSTALL_LIBDIR}" USE_SOURCE_PERMISSIONS)
endif()

add_library(amalg STATIC
  "${luajit_SRC}/ljamalg.c"
)

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(amalg PROPERTIES LINK_FLAGS "/DEBUG")
endif()

target_link_libraries(amalg PRIVATE "${LJ_VM_OBJECT}")
target_include_directories(amalg PRIVATE ${LUA_INCLUDE_DIRS})
add_dependencies(amalg ${gen_lj_targets})

add_executable(luajit "${luajit_SRC}/luajit.c")
if (MSVC)
  target_sources(luajit PRIVATE luajit.manifest)
endif()

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(luajit PROPERTIES LINK_FLAGS "/DEBUG")
endif()

target_link_libraries(luajit libluajit)
target_include_directories(luajit PRIVATE ${LUA_INCLUDE_DIRS})
set_target_output_directories(luajit)
set_target_properties(luajit PROPERTIES OUTPUT_NAME "luajit-${VERSION}")

if (("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin") AND ("${TARGET_LJARCH}" STREQUAL x64))
  target_link_options(luajit PRIVATE -pagezero_size 10000 -image_base 100000000)
endif()

if ((NOT WIN32) AND (NOT APPLE) AND (NOT (CMAKE_SYSTEM MATCHES "SunOS")) AND (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "PS3")))
  target_link_options(luajit PRIVATE -Wl,-E)
endif()

if (MSVC AND NOT (CMAKE_GENERATOR STREQUAL Ninja))
  set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}")
endif()

add_custom_command(
  TARGET luajit POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink "luajit-${VERSION}${CMAKE_EXECUTABLE_SUFFIX}" "luajit${CMAKE_EXECUTABLE_SUFFIX}"
  WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  VERBATIM)

if (UNIX)
  install(FILES "${luajit_SOURCE_DIR}/etc/luajit.1" DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
endif()

install(TARGETS libluajit
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"

  FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/luajit-${MAJVER}.${MINVER}"
)

install(DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/"
  DESTINATION "${CMAKE_INSTALL_BINDIR}"
  USE_SOURCE_PERMISSIONS
  PATTERN "minilua*" EXCLUDE
  PATTERN "buildvm*" EXCLUDE
)

install(DIRECTORY "${luajit_SRC}/jit"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/luajit-${VERSION}"
  USE_SOURCE_PERMISSIONS
  FILES_MATCHING PATTERN "*.lua"
)

install(DIRECTORY "${LUA_SHARE_DIR}"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/luajit-${VERSION}"
  USE_SOURCE_PERMISSIONS
  FILES_MATCHING PATTERN "*.lua"
)

if (WIN32 AND NOT (CYGWIN OR MINGW))
install(DIRECTORY "${luajit_SRC}/jit"
  DESTINATION "${EXECUTABLE_OUTPUT_PATH}/lua"
  USE_SOURCE_PERMISSIONS
  FILES_MATCHING PATTERN "*.lua"
)

install(DIRECTORY "${LUA_SHARE_DIR}"
  DESTINATION "${EXECUTABLE_OUTPUT_PATH}/lua"
  USE_SOURCE_PERMISSIONS
  FILES_MATCHING PATTERN "*.lua"
)
endif()

set(LUA_SUFFIX jit PARENT_SCOPE)

set(LUA_LIBRARIES liblua${LUA_SUFFIX}::liblua${LUA_SUFFIX} PARENT_SCOPE)
set(LUA_INTERPRETER $<TARGET_FILE:lua${LUA_SUFFIX}> PARENT_SCOPE)
set(LUA_INCDIR "${lua${LUA_SUFFIX}_SRC}" PARENT_SCOPE)
set(LUA_VERSION_STRING ${Lua${LUA_SUFFIX}_VERSION} PARENT_SCOPE)
set(LUA_VERSION_MAJOR ${MAJVER} PARENT_SCOPE)
set(LUA_VERSION_MINOR ${MINVER} PARENT_SCOPE)
set(LUA_VERSION_PATCH ${RELVER} PARENT_SCOPE)
set(LUA_INCLUDE_DIR "${LUA_INCLUDE_DIRS}" PARENT_SCOPE)
