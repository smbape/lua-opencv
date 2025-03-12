cmake_minimum_required(VERSION 3.25)

# Sources: https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Windows

project(luarocks)

include(../cmake/FindPatch.cmake)

set(LuaRocks_VERSION 3.11.1)

get_filename_component(LUA_BINDIR "${LUA_INTERPRETER}" DIRECTORY)
file(TO_NATIVE_PATH "${LUA_BINDIR}" LUA_BINDIR)
file(TO_NATIVE_PATH "${LUA_DIR}" LUA_DIR)
file(TO_NATIVE_PATH "${LUA_INCLUDE_DIR}" LUA_INCDIR)
get_filename_component(LUA_INTERPRETER_NAME "${LUA_INTERPRETER}" NAME)

set(ABIVER ${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR})

if (WIN32)
    set(Luarocks_EXECUTABLE "<SOURCE_DIR>\\luarocks.exe")
    set(luarocks_COMMAND luarocks.bat)
else()
    set(Luarocks_EXECUTABLE "<BINARY_DIR>/bin/luarocks")
    set(luarocks_COMMAND ./luarocks)
endif()

set(luarocks_INSTALL_COMMANDS
    COMMAND "${Luarocks_EXECUTABLE}" --lua-version "${ABIVER}" --lua-dir "${LUA_DIR}" init --lua-versions "5.1,5.2,5.3,5.4"
    COMMAND "${luarocks_COMMAND}" --lua-version "${ABIVER}" config --scope project lua_interpreter "${LUA_INTERPRETER_NAME}"
    COMMAND "${luarocks_COMMAND}" --lua-version "${ABIVER}" config --scope project variables.LUA_BINDIR "${LUA_BINDIR}"
    COMMAND "${luarocks_COMMAND}" --lua-version "${ABIVER}" config --scope project variables.LUA_DIR "${LUA_DIR}"
    COMMAND "${luarocks_COMMAND}" --lua-version "${ABIVER}" config --scope project variables.LUA_INCDIR "${LUA_INCDIR}"
)

if (WIN32)
    list(APPEND luarocks_INSTALL_COMMANDS COMMAND "${CMAKE_COMMAND}"
        "-DLUAROCKS_EXE=<SOURCE_DIR>/luarocks.exe"
        "-DLUA_BINDIR=${LUA_BINDIR}"
        "-DLUA_INTERPRETER_NAME=${LUA_INTERPRETER_NAME}"
        "-DABIVER=${ABIVER}"
        "-DEXTNAME=.bat"
        -P luarocks-init.cmake
    )
else()
    list(PREPEND luarocks_INSTALL_COMMANDS COMMAND rm -f luarocks lua)
endif()

if (WIN32)
    set(LuaRocks_URL https://luarocks.org/releases/luarocks-${LuaRocks_VERSION}-windows-64.zip)
    set(LuaRocks_URL_HASH SHA256=c71dba3d03e12305e9ccd022c621c8869aba3d124d9249e214aed5c16f3682a3)
    file(TO_NATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}" NATIVE_CURRENT_SOURCE_DIR)

    include(ExternalProject)
    ExternalProject_Add(luarocks
        URL                 ${LuaRocks_URL}
        URL_HASH            ${LuaRocks_URL_HASH}
        CONFIGURE_COMMAND   ""
        BUILD_COMMAND       ""
        INSTALL_COMMAND     cd /d "${NATIVE_CURRENT_SOURCE_DIR}" ${luarocks_INSTALL_COMMANDS}
        EXCLUDE_FROM_ALL  TRUE
    )
else()
    set(LuaRocks_URL https://luarocks.org/releases/luarocks-${LuaRocks_VERSION}.tar.gz)
    set(LuaRocks_URL_HASH SHA256=c3fb3d960dffb2b2fe9de7e3cb004dc4d0b34bb3d342578af84f84325c669102)

    include(ExternalProject)
    ExternalProject_Add(luarocks
        URL                 ${LuaRocks_URL}
        URL_HASH            ${LuaRocks_URL_HASH}
        PATCH_COMMAND       "${PATCH_EXECUTABLE}" -p1 -d "<SOURCE_DIR>" -i "${CMAKE_CURRENT_SOURCE_DIR}/patches/001-luarocks-src.patch"
        CONFIGURE_COMMAND   cd "<SOURCE_DIR>"
                            COMMAND   ./configure "--prefix=<BINARY_DIR>" "--lua-version=${ABIVER}" "--with-lua=${LUA_DIR}" "--with-lua-interpreter=${LUA_INTERPRETER_NAME}"
        BUILD_COMMAND       cd "<SOURCE_DIR>" COMMAND make COMMAND make install
        INSTALL_COMMAND     cd "${CMAKE_CURRENT_SOURCE_DIR}" ${luarocks_INSTALL_COMMANDS}
        EXCLUDE_FROM_ALL    TRUE
    )
endif()

ExternalProject_Get_Property(luarocks SOURCE_DIR BINARY_DIR)

cmake_path(SET luarocks_COMMAND NORMALIZE "${CMAKE_CURRENT_SOURCE_DIR}/${luarocks_COMMAND}")

string(REPLACE "<SOURCE_DIR>" "${SOURCE_DIR}" Luarocks_EXECUTABLE "${Luarocks_EXECUTABLE}")
string(REPLACE "<BINARY_DIR>" "${BINARY_DIR}" Luarocks_EXECUTABLE "${Luarocks_EXECUTABLE}")

set(luarocks_SOURCE_DIR "${SOURCE_DIR}" PARENT_SCOPE)
set(luarocks_BINARY_DIR "${BINARY_DIR}" PARENT_SCOPE)
set(Luarocks_EXECUTABLE "${Luarocks_EXECUTABLE}" PARENT_SCOPE)
set(luarocks_COMMAND "${luarocks_COMMAND}" PARENT_SCOPE)
