cmake_minimum_required(VERSION 3.25)

# Sources: https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Windows

# Name of the project (will be the name of the plugin)
project(luarocks)

include(../cmake/FindPatch.cmake)

set(LuaRocks_VERSION 3.11.0)

get_filename_component(LUA_BINDIR "${LUA_INTERPRETER}" DIRECTORY)
file(TO_NATIVE_PATH "${LUA_BINDIR}" LUA_BINDIR)
file(TO_NATIVE_PATH "${LUA_DIR}" LUA_DIR)
file(TO_NATIVE_PATH "${LUA_INCLUDE_DIR}" LUA_INCDIR)
get_filename_component(LUA_INTERPRETER_NAME "${LUA_INTERPRETER}" NAME)

set(ABIVER ${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR})

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" PROJECT_DIR)

if (WIN32)
  set(luarocks_exe "<SOURCE_DIR>\\luarocks.exe")
  set(luarocks luarocks.bat)
else()
  set(luarocks_exe "<BINARY_DIR>/bin/luarocks")
  set(luarocks ./luarocks)
endif()

set(COMMANDS
  COMMAND "${luarocks_exe}" --lua-version "${ABIVER}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
  COMMAND "${luarocks_exe}" --lua-version "${ABIVER}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
  COMMAND ${luarocks} --lua-version "${ABIVER}" config --scope project lua_interpreter "${LUA_INTERPRETER_NAME}"
  COMMAND ${luarocks} --lua-version "${ABIVER}" config --scope project variables.LUA_BINDIR "${LUA_BINDIR}"
  COMMAND ${luarocks} --lua-version "${ABIVER}" config --scope project variables.LUA_DIR "${LUA_DIR}"
  COMMAND ${luarocks} --lua-version "${ABIVER}" config --scope project variables.LUA_INCDIR "${LUA_INCDIR}"
)

if (WIN32)
  set(LuaRocks_URL https://luarocks.github.io/luarocks/releases/luarocks-${LuaRocks_VERSION}-windows-64.zip)
  set(LuaRocks_URL_HASH SHA256=a638ea4c8e858106e8e2598e50e9b3fd563c111b44c36d33fe0f0e64d9f42685)

  include(ExternalProject)
  ExternalProject_Add(luarocks
    URL                 ${LuaRocks_URL}
    URL_HASH            ${LuaRocks_URL_HASH}
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       ""
    INSTALL_COMMAND     cd /d "${PROJECT_DIR}" ${COMMANDS}
            COMMAND     "${CMAKE_COMMAND}"
                          "-DLUAROCKS_EXE=<SOURCE_DIR>/luarocks.exe"
                          "-DLUA_BINDIR=${LUA_BINDIR}"
                          "-DLUA_INTERPRETER_NAME=${LUA_INTERPRETER_NAME}"
                          "-DABIVER=${ABIVER}"
                          "-DEXTNAME=.bat"
                          -P luarocks-init.cmake
    EXCLUDE_FROM_ALL  TRUE
  )
else()
  set(LuaRocks_URL https://github.com/luarocks/luarocks/archive/refs/tags/v${LuaRocks_VERSION}.tar.gz)
  set(LuaRocks_URL_HASH SHA256=78698aa185f636e72865881425780a84c1dfb171f1789070f657f34116ff6521)

  include(ExternalProject)
  ExternalProject_Add(luarocks
    URL                 ${LuaRocks_URL}
    URL_HASH            ${LuaRocks_URL_HASH}
    PATCH_COMMAND       "${PATCH}" -p 1 -d "<SOURCE_DIR>" -i "${CMAKE_CURRENT_SOURCE_DIR}/patches/001-luarocks-src.patch"
    CONFIGURE_COMMAND   cd "<SOURCE_DIR>" COMMAND ./configure "--prefix=<BINARY_DIR>" "--with-lua=${LUA_DIR}"
    BUILD_COMMAND       cd "<SOURCE_DIR>" COMMAND make COMMAND make install
    INSTALL_COMMAND     cd "${PROJECT_DIR}" ${COMMANDS}
    EXCLUDE_FROM_ALL    TRUE
  )
endif()
