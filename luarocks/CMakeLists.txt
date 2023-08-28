cmake_minimum_required(VERSION 3.19)

# Sources: https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Windows

# Name of the project (will be the name of the plugin)
project(luarocks)

if (DEFINED LUA_DIR)
  set(ENV{LUA_DIR} "${LUA_DIR}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/Modules")
find_package(Lua REQUIRED)

set(LuaRocks_VERSION 3.9.2)

get_filename_component(PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}" DIRECTORY)
file(TO_NATIVE_PATH "${PROJECT_DIR}" PROJECT_DIR)

if (WIN32)
  set(LuaRocks_URL_HASH SHA256=ab7e34332eedd6270b97f44df462e4584d3a60377205d88ea2806ecc547f074f)
  set(LuaRocks_URL https://luarocks.github.io/luarocks/releases/luarocks-${LuaRocks_VERSION}-windows-64.zip)

  include(ExternalProject)
  ExternalProject_Add(luarocks
    URL                 ${LuaRocks_URL}
    URL_HASH            ${LuaRocks_URL_HASH}
    CONFIGURE_COMMAND   cd /d "${PROJECT_DIR}"
              COMMAND   "<SOURCE_DIR>\\luarocks.exe" --lua-version "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
              COMMAND   "<SOURCE_DIR>\\luarocks.exe" --lua-version "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
    BUILD_COMMAND       ""
    INSTALL_COMMAND     ""
    EXCLUDE_FROM_ALL  TRUE
  )
else()
  include(ExternalProject)
  ExternalProject_Add(luarocks
    GIT_REPOSITORY https://github.com/luarocks/luarocks.git
    GIT_TAG        v${LuaRocks_VERSION}
    CONFIGURE_COMMAND "<SOURCE_DIR>/configure --prefix=<BINARY_DIR> --with-lua=${LUA_DIR} --with-lua-include=${LUA_INCLUDE_DIR} --with-lua-lib=${LUA_LIBRARY}"
    BUILD_COMMAND     make COMMAND make install
    INSTALL_COMMAND   cd "${PROJECT_DIR}"
            COMMAND   "<BINARY_DIR>/bin/luarocks" --lua-version "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
            COMMAND   "<BINARY_DIR>/bin/luarocks" --lua-version "${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
    EXCLUDE_FROM_ALL  TRUE
  )
endif()

message(STATUS "luarocks_SOURCE_DIR = ${luarocks_SOURCE_DIR}")
