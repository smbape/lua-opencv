# https://github.com/luvit/luv/blob/1.45.0-0/deps/lua.cmake
cmake_minimum_required(VERSION 3.25)

project(lua)

if(MSVC AND NOT (CMAKE_GENERATOR STREQUAL Ninja))
  add_compile_options(/MP)
endif()

include(../cmake/FindPatch.cmake)
include(cmake/initialize.cmake)

option(BUILD_SHARED_LIBS "Build shared libraries (.dll/.so) instead of static ones (.lib/.a)" ON)

set(Lua_URL_HASH_515 2640fc56a795f29d28ef15e13c34a47e223960b0240e8cb0a82d9b0738695333)
set(Lua_URL_HASH_524 b9e2e4aad6789b3b63a056d442f7b39f0ecfca3ae0f1fc0ae4e9614401b69f4b)
set(Lua_URL_HASH_536 fc5fd69bb8736323f026672b1b7235da613d7177e72558893a0bdcd320466d60)
set(Lua_URL_HASH_546 7d5ea1b9cb6aa0b59ca3dde1c6adcb57ef83a1ba8e5432c0ecd06bf439b3ad88)

if ((NOT DEFINED Lua_VERSION) OR ("${Lua_VERSION} " STREQUAL " "))
  set(Lua_VERSION 5.4.6)
  set(Lua_VERSION "${Lua_VERSION}" CACHE STRING "Choose the Lua version." FORCE)
else()
  set(Lua_VERSION "${Lua_VERSION}" CACHE STRING "Choose the Lua version." FORCE)

  if ("${Lua_VERSION}" STREQUAL "5.4")
    set(Lua_VERSION 5.4.6)
    set(Lua_VERSION 5.4.6 PARENT_SCOPE)
  elseif("${Lua_VERSION}" STREQUAL "5.3")
    set(Lua_VERSION 5.3.6)
    set(Lua_VERSION 5.3.6 PARENT_SCOPE)
  elseif("${Lua_VERSION}" STREQUAL "5.2")
    set(Lua_VERSION 5.2.4)
    set(Lua_VERSION 5.2.4 PARENT_SCOPE)
  elseif("${Lua_VERSION}" STREQUAL "5.1")
    set(Lua_VERSION 5.1.5)
    set(Lua_VERSION 5.1.5 PARENT_SCOPE)
  endif()
endif()

set(Lua_INTERPRETER_NAME "lua-${Lua_VERSION}" CACHE STRING "Lua interpreter name" FORCE)

string(REPLACE "." "" Lua_DLLVERSION ${Lua_VERSION})

string(REPLACE "." ";" Lua_VERSION_LIST ${Lua_VERSION})
list(GET Lua_VERSION_LIST 0 MAJVER)
list(GET Lua_VERSION_LIST 1 MINVER)
list(GET Lua_VERSION_LIST 2 RELVER)

set(VERSION ${Lua_VERSION})
set(ABIVER  ${MAJVER}.${MINVER})
string(REPLACE "." "" NODOTABIVER ${ABIVER})


# https://lfs.lug.org.cn/mirrors.html
include(FetchContent)
FetchContent_Declare(lua
  URL                 https://sources.buildroot.net/lua/lua-${Lua_VERSION}.tar.gz
                      https://ftp.wrz.de/pub/BLFS/conglomeration/lua/lua-${Lua_VERSION}.tar.gz
                      https://ftp2.osuosl.org/pub/blfs/conglomeration/lua/lua-${Lua_VERSION}.tar.gz
                      https://mirror-hk.koddos.net/blfs/conglomeration/lua/lua-${Lua_VERSION}.tar.gz
                      https://mirror.dogado.de/BLFS/conglomeration/lua/lua-${Lua_VERSION}.tar.gz
                      https://mirror.koddos.net/blfs/conglomeration/lua/lua-${Lua_VERSION}.tar.gz
                      https://repo.jing.rocks/blfs/conglomeration/lua/lua-${Lua_VERSION}.tar.gz
                      https://www.lua.org/ftp/lua-${Lua_VERSION}.tar.gz
  URL_HASH            SHA256=${Lua_URL_HASH_${Lua_DLLVERSION}}
  PATCH_COMMAND       "${PATCH}" -p 1 -d "<SOURCE_DIR>" -i "${CMAKE_CURRENT_SOURCE_DIR}/patches/001-lua-${MAJVER}.${MINVER}-src.patch"
)

Fetchcontent_Populate(lua)

if ("${ABIVER}" STREQUAL "5.2")
  add_compile_definitions(LUA_COMPAT_ALL)
elseif ("${ABIVER}" STREQUAL "5.3")
  add_compile_definitions(LUA_COMPAT_5_2)
elseif ("${ABIVER}" STREQUAL "5.4")
  add_compile_definitions(LUA_COMPAT_5_3)
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

if (UNIX)
  find_library(LIBM_LIBRARIES NAMES m REQUIRED)
  link_libraries(${LIBM_LIBRARIES})
endif()

if (CMAKE_SYSTEM MATCHES "SunOS")
  add_compile_definitions(LUA_USE_DLOPEN)
endif()

if ((NOT APPLE) AND (("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") OR ("${CMAKE_SYSTEM_NAME}" MATCHES "(Open|Free|Net)BSD")))
  find_library(LIBDL_LIBRARIES NAMES dl REQUIRED)
  link_libraries(${LIBDL_LIBRARIES})
endif()

if (APPLE OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux") OR ("${CMAKE_SYSTEM_NAME}" MATCHES "(Open|Free|Net)BSD"))
  find_library(LIBREADLINE_LIBRARIES NAMES readline REQUIRED)
  link_libraries(${LIBREADLINE_LIBRARIES})
endif()

if ("${ABIVER}" STREQUAL "5.1")
    if (CYGWIN OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"))
      find_library(LIBDEPS_LIBRARIES NAMES history ncurses REQUIRED)
      link_libraries(${LIBDEPS_LIBRARIES})
    endif()
endif()

if (CYGWIN OR ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux"))
  add_compile_definitions(LUA_USE_LINUX)
elseif (APPLE)
  if ("${ABIVER}" STREQUAL "5.1")
    add_compile_definitions(LUA_USE_LINUX)
  else()
    add_compile_definitions(LUA_USE_MACOSX)
  endif()
endif()

if ((NOT WIN32) AND (NOT APPLE) AND (NOT (CMAKE_SYSTEM MATCHES "SunOS")) AND (NOT ("${CMAKE_SYSTEM_NAME}" STREQUAL "PS3")))
  add_link_options(-Wl,-E)
endif()

set(lua_SRC "${lua_SOURCE_DIR}/src")
set(LUA_INCLUDE_DIRS
    "${lua_SRC}"
    "${lua_SOURCE_DIR}/etc"
)

if (ANDROID AND (${CMAKE_SYSTEM_VERSION} LESS 21))
  set(ENABLE_LARGEFILE OFF)
elseif (WIN32 OR CYGWIN OR MINGW)
  set(ENABLE_LARGEFILE OFF)
else()
  set(ENABLE_LARGEFILE ON)
endif()

if (ENABLE_LARGEFILE)
  add_compile_definitions(_FILE_OFFSET_BITS=64 _LARGEFILE_SOURCE)
endif()

set(LUA_SRC_CORE_51
  "${lua_SRC}/lapi.c"
  "${lua_SRC}/lcode.c"
  "${lua_SRC}/ldebug.c"
  "${lua_SRC}/ldo.c"
  "${lua_SRC}/ldump.c"
  "${lua_SRC}/lfunc.c"
  "${lua_SRC}/lgc.c"
  "${lua_SRC}/llex.c"
  "${lua_SRC}/lmem.c"
  "${lua_SRC}/lobject.c"
  "${lua_SRC}/lopcodes.c"
  "${lua_SRC}/lparser.c"
  "${lua_SRC}/lstate.c"
  "${lua_SRC}/lstring.c"
  "${lua_SRC}/ltable.c"
  "${lua_SRC}/ltm.c"
  "${lua_SRC}/lundump.c"
  "${lua_SRC}/lvm.c"
  "${lua_SRC}/lzio.c"
)

set(LUA_SRC_CORE_52
  ${LUA_SRC_CORE_51}
  "${lua_SRC}/lctype.c"
)

set(LUA_SRC_CORE_53
  ${LUA_SRC_CORE_52}
)

set(LUA_SRC_CORE_54
  ${LUA_SRC_CORE_53}
)

set(LUA_SRC_LIB_51
  "${lua_SRC}/lauxlib.c"
  "${lua_SRC}/lbaselib.c"
  "${lua_SRC}/ldblib.c"
  "${lua_SRC}/linit.c"
  "${lua_SRC}/liolib.c"
  "${lua_SRC}/lmathlib.c"
  "${lua_SRC}/loadlib.c"
  "${lua_SRC}/loslib.c"
  "${lua_SRC}/lstrlib.c"
  "${lua_SRC}/ltablib.c"
)

set(LUA_SRC_LIB_52
  ${LUA_SRC_LIB_51} 
  "${lua_SRC}/lbitlib.c"
  "${lua_SRC}/lcorolib.c"
)

set(LUA_SRC_LIB_53
  ${LUA_SRC_LIB_52}
  "${lua_SRC}/lutf8lib.c"
)

set(LUA_SRC_LIB_54
  ${LUA_SRC_LIB_53}
)
list(REMOVE_ITEM LUA_SRC_LIB_54 "${lua_SRC}/lbitlib.c")

set(LUA_SRC_CORE ${LUA_SRC_CORE_${NODOTABIVER}})
set(LUA_SRC_LIB ${LUA_SRC_LIB_${NODOTABIVER}})

add_library(liblua SHARED ${LUA_SRC_CORE} ${LUA_SRC_LIB})

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(liblua PROPERTIES LINK_FLAGS "/DEBUG")
endif()

set(LUA_INCLUDE_FILES
    "${lua_SRC}/lua.h"
    "${lua_SRC}/luaconf.h"
    "${lua_SRC}/lualib.h"
    "${lua_SRC}/lauxlib.h"
)

if ("${ABIVER}" STREQUAL "5.1")
  install(FILES "${lua_SOURCE_DIR}/etc/lua.hpp" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lua-${MAJVER}.${MINVER}")
else()
  list(APPEND LUA_INCLUDE_FILES "${lua_SRC}/lua.hpp")
endif()

target_sources(liblua
  PUBLIC FILE_SET HEADERS TYPE HEADERS
  BASE_DIRS "${lua_SRC}"
  FILES ${LUA_INCLUDE_FILES}
)

add_library(liblua::liblua ALIAS liblua)

target_include_directories(liblua PRIVATE ${LUA_INCLUDE_DIRS})

IF (WIN32 AND (CMAKE_GENERATOR STREQUAL Ninja))
  target_compile_definitions(liblua PRIVATE _WINDLL)
ENDIF()

if ((WIN32 OR CYGWIN OR MINGW) AND BUILD_SHARED_LIBS)
  target_compile_definitions(liblua PRIVATE LUA_BUILD_AS_DLL)
endif()

if (WIN32 OR CYGWIN OR MINGW)
  set_target_properties(liblua PROPERTIES
    OUTPUT_NAME lua${NODOTABIVER}
  )
else()
  set_target_properties(liblua PROPERTIES
    OUTPUT_NAME lua-${ABIVER}
    VERSION "${VERSION}"
    SOVERSION "${MAJVER}"
  )
endif()

set_target_output_directories(liblua)

if (UNIX)
  get_target_property(liblua_OUTPUT_NAME liblua OUTPUT_NAME)
  add_custom_command(
    TARGET liblua POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E make_directory pkgconfig
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${lua_SOURCE_DIR}/etc/lua.pc" pkgconfig/lua.pc
    COMMAND "${CMAKE_COMMAND}"
      "-DPREFIX=${CMAKE_INSTALL_PREFIX}"
      "-DMULTILIB=${CMAKE_INSTALL_LIBDIR}"
      "-DLIBNAME=${liblua_OUTPUT_NAME}"
      -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pkgconfig.cmake"
    WORKING_DIRECTORY "${LIBRARY_OUTPUT_PATH}"
    VERBATIM)
  install(DIRECTORY "${LIBRARY_OUTPUT_PATH}/pkgconfig" DESTINATION "${CMAKE_INSTALL_LIBDIR}" USE_SOURCE_PERMISSIONS)
endif()

add_executable(lua "${lua_SRC}/lua.c")
if (MSVC)
  target_sources(lua PRIVATE lua_exe.rc lua.manifest)
  target_compile_definitions(lua PRIVATE "-DLua_VERSION=${Lua_VERSION}" "-DMAJVER=${MAJVER}" "-DMINVER=${MINVER}" "-DRELVER=${RELVER}")
endif()

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(lua PROPERTIES LINK_FLAGS "/DEBUG")
endif()

target_link_libraries(lua liblua)
target_include_directories(lua PRIVATE ${LUA_INCLUDE_DIRS})
set_target_output_directories(lua)
set_target_properties(lua PROPERTIES OUTPUT_NAME "lua-${VERSION}")

if (MSVC AND NOT (CMAKE_GENERATOR STREQUAL Ninja))
  set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}")
endif()

add_custom_command(
  TARGET lua POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink "lua-${VERSION}${CMAKE_EXECUTABLE_SUFFIX}" "lua${CMAKE_EXECUTABLE_SUFFIX}"
  WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  VERBATIM)

if (UNIX)
  install(FILES "${lua_SOURCE_DIR}/doc/lua.1" DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
endif()

set(LUAC_SRC "${lua_SRC}/luac.c")
if ("${ABIVER}" STREQUAL "5.1")
  list(APPEND LUAC_SRC "${lua_SRC}/print.c")
endif()

add_executable(luac ${LUAC_SRC} ${LUA_SRC_CORE} ${LUA_SRC_LIB})
if (MSVC)
  target_sources(luac PRIVATE lua_exe.rc lua.manifest)
  target_compile_definitions(luac PRIVATE "-DLua_VERSION=${Lua_VERSION}" "-DMAJVER=${MAJVER}" "-DMINVER=${MINVER}" "-DRELVER=${RELVER}")
endif()

# Always generate debug files
if(MSVC AND NOT "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set_target_properties(luac PROPERTIES LINK_FLAGS "/DEBUG")
endif()

target_include_directories(luac PRIVATE ${LUA_INCLUDE_DIRS})
set_target_output_directories(luac)
set_target_properties(luac PROPERTIES OUTPUT_NAME "luac-${VERSION}")

if (MSVC AND NOT (CMAKE_GENERATOR STREQUAL Ninja))
  set(EXECUTABLE_OUTPUT_PATH "${EXECUTABLE_OUTPUT_PATH}/${CMAKE_BUILD_TYPE}")
endif()

add_custom_command(
  TARGET luac POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E create_symlink "luac-${VERSION}${CMAKE_EXECUTABLE_SUFFIX}" "luac${CMAKE_EXECUTABLE_SUFFIX}"
  WORKING_DIRECTORY "${EXECUTABLE_OUTPUT_PATH}"
  VERBATIM)

if (UNIX)
  install(FILES "${lua_SOURCE_DIR}/doc/luac.1" DESTINATION "${CMAKE_INSTALL_MANDIR}/man1")
endif()

install(TARGETS liblua
  LIBRARY
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ
                    GROUP_EXECUTE GROUP_READ
  ARCHIVE
    DESTINATION "${CMAKE_INSTALL_LIBDIR}"

  FILE_SET HEADERS DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/lua-${MAJVER}.${MINVER}"
)

install(DIRECTORY "${EXECUTABLE_OUTPUT_PATH}/"
  DESTINATION "${CMAKE_INSTALL_BINDIR}"
  USE_SOURCE_PERMISSIONS
)

set(LUA_SUFFIX "" PARENT_SCOPE)

set(LUA_LIBRARIES liblua${LUA_SUFFIX}::liblua${LUA_SUFFIX} PARENT_SCOPE)
set(LUA_INTERPRETER $<TARGET_FILE:lua${LUA_SUFFIX}> PARENT_SCOPE)
set(LUA_INCDIR "${lua${LUA_SUFFIX}_SRC}" PARENT_SCOPE)
set(LUA_VERSION_STRING ${Lua${LUA_SUFFIX}_VERSION} PARENT_SCOPE)
set(LUA_VERSION_MAJOR ${MAJVER} PARENT_SCOPE)
set(LUA_VERSION_MINOR ${MINVER} PARENT_SCOPE)
set(LUA_VERSION_PATCH ${RELVER} PARENT_SCOPE)
set(LUA_INCLUDE_DIR "${LUA_INCLUDE_DIRS}" PARENT_SCOPE)
