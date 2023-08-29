cmake_minimum_required(VERSION 3.19)

# Sources: https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Windows

# Name of the project (will be the name of the plugin)
project(luarocks)

set(LuaRocks_VERSION 3.9.2)

set(LUA_DIR "$ENV{LUA_DIR}")

get_filename_component(LUA_BINDIR "${LUA_INTERPRETER}" DIRECTORY)
file(TO_NATIVE_PATH "${LUA_BINDIR}" LUA_BINDIR)
file(TO_NATIVE_PATH "${LUA_DIR}" LUA_DIR)
file(TO_NATIVE_PATH "${LUA_INCLUDE_DIR}" LUA_INCDIR)
get_filename_component(LUA_INTERPRETER_NAME "${LUA_INTERPRETER}" NAME)

set(ABIVER ${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR})

file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" PROJECT_DIR)

if (WIN32)
  set(LuaRocks_URL https://luarocks.github.io/luarocks/releases/luarocks-${LuaRocks_VERSION}-windows-64.zip)
  set(LuaRocks_URL_HASH SHA256=ab7e34332eedd6270b97f44df462e4584d3a60377205d88ea2806ecc547f074f)

  include(ExternalProject)
  ExternalProject_Add(luarocks
    URL                 ${LuaRocks_URL}
    URL_HASH            ${LuaRocks_URL_HASH}
    CONFIGURE_COMMAND   ""
    BUILD_COMMAND       ""
    INSTALL_COMMAND     cd /d "${PROJECT_DIR}"
              COMMAND   "<SOURCE_DIR>\\luarocks.exe" --lua-version "${ABIVER}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
              COMMAND   "<SOURCE_DIR>\\luarocks.exe" --lua-version "${ABIVER}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
              COMMAND   luarocks.bat --lua-version "${ABIVER}" config --scope project lua_interpreter "${LUA_INTERPRETER_NAME}"
              COMMAND   luarocks.bat --lua-version "${ABIVER}" config --scope project variables.LUA_BINDIR "${LUA_BINDIR}"
              COMMAND   luarocks.bat --lua-version "${ABIVER}" config --scope project variables.LUA_DIR "${LUA_DIR}"
              COMMAND   luarocks.bat --lua-version "${ABIVER}" config --scope project variables.LUA_INCDIR "${LUA_INCDIR}"
              COMMAND   "${CMAKE_COMMAND}"
                          "-DLUAROCKS_EXE=<SOURCE_DIR>\\luarocks.exe"
                          "-DLUA_BINDIR=${LUA_BINDIR}"
                          "-DLUA_INTERPRETER_NAME=${DLUA_INTERPRETER_NAME}"
                          "-DABIVER=${ABIVER}"
                          "-DEXTNAME=.bat"
                          -P luarocks-init.cmake
    EXCLUDE_FROM_ALL  TRUE
  )
else()
  set(LuaRocks_URL https://github.com/luarocks/luarocks/archive/refs/tags/v${LuaRocks_VERSION}.tar.gz)
  set(LuaRocks_URL_HASH SHA256=a0b36cd68586cd79966d0106bb2e5a4f5523327867995fd66bee4237062b3e3b)

  include(ExternalProject)
  ExternalProject_Add(luarocks
    URL                 ${LuaRocks_URL}
    URL_HASH            ${LuaRocks_URL_HASH}
    CONFIGURE_COMMAND   cd "<SOURCE_DIR>" COMMAND ./configure "--prefix=<BINARY_DIR>" "--with-lua=${LUA_DIR}"
    BUILD_COMMAND       cd "<SOURCE_DIR>" COMMAND make COMMAND make install
    INSTALL_COMMAND     cd "${PROJECT_DIR}"
            COMMAND     "<BINARY_DIR>/bin/luarocks" --lua-version "${ABIVER}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
            COMMAND     "<BINARY_DIR>/bin/luarocks" --lua-version "${ABIVER}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
            COMMAND     ./luarocks --lua-version "${ABIVER}" config --scope project cmake_generator Ninja
            COMMAND     ./luarocks --lua-version "${ABIVER}" config --scope project lua_interpreter "${LUA_INTERPRETER_NAME}"
            COMMAND     ./luarocks --lua-version "${ABIVER}" config --scope project variables.LUA_BINDIR "${LUA_BINDIR}"
            COMMAND     ./luarocks --lua-version "${ABIVER}" config --scope project variables.LUA_DIR "${LUA_DIR}"
            COMMAND     ./luarocks --lua-version "${ABIVER}" config --scope project variables.LUA_INCDIR "${LUA_INCDIR}"
    EXCLUDE_FROM_ALL    TRUE
  )
endif()
