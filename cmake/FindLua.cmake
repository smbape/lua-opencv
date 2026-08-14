option(BUILD_LUA "Build Lua from source" ON)

function(update_lua_multi_config)
    unset(_Luajit_VERSION)
    unset(_Lua_VERSION)

    if (Luajit_VERSION)
        set(_Luajit_VERSION "${Luajit_VERSION}")
    elseif (Lua_VERSION MATCHES "^luajit-")
        string(SUBSTRING "${Lua_VERSION}" 7 -1 _Luajit_VERSION)
    elseif (Lua_VERSION)
        set(_Lua_VERSION "${Lua_VERSION}")
    endif()

    if (_Luajit_VERSION)
        string(REPLACE "." ";" Luajit_VERSION_PARTS "${Luajit_VERSION}")
        list(GET Luajit_VERSION_PARTS 0 Luajit_VERSION_MAJOR)
        list(GET Luajit_VERSION_PARTS 1 Luajit_VERSION_MINOR)
        set(LUA_MULTI_CONFIG_SUFFIX "/luajit-${Luajit_VERSION_MAJOR}.${Luajit_VERSION_MINOR}" PARENT_SCOPE)
    elseif(_Lua_VERSION)
        string(REPLACE "." ";" Lua_VERSION_PARTS "${_Lua_VERSION}")
        list(GET Lua_VERSION_PARTS 0 Lua_VERSION_MAJOR)
        list(GET Lua_VERSION_PARTS 1 Lua_VERSION_MINOR)
        set(LUA_MULTI_CONFIG_SUFFIX "/${Lua_VERSION_MAJOR}.${Lua_VERSION_MINOR}" PARENT_SCOPE)
    endif()
endfunction()

foreach(var_name BINDIR DIR INCDIR)
    if (DEFINED LUA_${var_name} AND "${LUA_${var_name}} " STREQUAL " ")
        unset(LUA_${var_name} CACHE)
    endif()

    if (DEFINED LUA_${var_name})
        set(ENV{LUA_${var_name}} "${LUA_${var_name}}")
    endif()
endforeach()

if(DEFINED ENV{LUA_MULTI_CONFIG} AND NOT DEFINED LUA_MULTI_CONFIG)
    set(LUA_MULTI_CONFIG "$ENV{LUA_MULTI_CONFIG}")
endif()

if (BUILD_LUA AND (NOT WITH_LUA_ROCKS))
    include("${CMAKE_SOURCE_DIR}/cmake/lua/version.cmake")

    # Hack to ensure that Lua is built with BUILD_SHARED_LIBS ON
    set(BUILD_SHARED_LIBS_BACKUP ${BUILD_SHARED_LIBS})
    set(BUILD_SHARED_LIBS ON)

    if (Lua_VERSION MATCHES "^luajit-")
        string(SUBSTRING "${Lua_VERSION}" 7 -1 Luajit_VERSION)
        add_subdirectory(luajit)
        set(lua_PROJECT_SUFFIX "jit")
    else()
        unset(Luajit_VERSION CACHE)
        update_lua_multi_config()
        add_subdirectory(lua "lua${LUA_MULTI_CONFIG_SUFFIX}")
        unset(lua_PROJECT_SUFFIX)
    endif()

    set(BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS_BACKUP})
    unset(BUILD_SHARED_LIBS_BACKUP)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT lua${lua_PROJECT_SUFFIX})
    file(TO_NATIVE_PATH "${${CMAKE_CURRENT_SOURCE_DIR}/test}" VS_DEBUGGER_WORKING_DIRECTORY)
    set_target_properties(lua${lua_PROJECT_SUFFIX} PROPERTIES
        VS_DEBUGGER_COMMAND_ARGUMENTS test.lua
        VS_DEBUGGER_WORKING_DIRECTORY "${VS_DEBUGGER_WORKING_DIRECTORY}"
    )
endif()

if (WITH_LUA_ROCKS OR (DEFINED ENV{LUA_DIR}))
    unset(LUA_INCLUDE_DIR)
    unset(LUA_INCLUDE_DIR CACHE)
    unset(LUA_INTERPRETER)
    unset(LUA_INTERPRETER CACHE)
    unset(LUA_LIBRARY)
    unset(LUA_LIBRARY CACHE)
    unset(LUA_LIBRARIES)
    unset(LUA_LIBRARIES CACHE)
endif()

if (DEFINED ENV{LUA_DIR})
    message(STATUS "LUA_BINDIR=${LUA_BINDIR}")
    message(STATUS "LUA_DIR=${LUA_DIR}")
    message(STATUS "LUA_INCDIR=${LUA_INCDIR}")

    if (Lua_VERSION MATCHES "^luajit-")
        set(Lua_SUFFIX "jit")
        string(SUBSTRING "${Lua_VERSION}" 7 -1 Luajit_VERSION)
        set(Lua_VERSION 5.1)
    else()
        unset(Lua_SUFFIX)
        unset(Luajit_VERSION CACHE)
    endif()

    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/lua/Modules")

    if (Lua_VERSION)
        find_package(Lua ${Lua_VERSION} EXACT REQUIRED)
    else()
        find_package(Lua REQUIRED)
    endif()

    unset(_lua_interpreter_names)

    if (Lua_INTERPRETER_NAME)
        set(_lua_interpreter_names ${Lua_INTERPRETER_NAME})
    else()
        if (Lua_VERSION)
            set(_lua_interpreter_names
                luajit-${Lua_VERSION_MAJOR}.${Lua_VERSION_MINOR}
                lua${Lua_VERSION_MAJOR}${Lua_VERSION_MINOR}
                lua${Lua_VERSION_MAJOR}.${Lua_VERSION_MINOR}
                lua-${Lua_VERSION_MAJOR}.${Lua_VERSION_MINOR}
                lua.${Lua_VERSION_MAJOR}.${Lua_VERSION_MINOR}
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
        DOC "Lua Interpreter"
        REQUIRED
    )

    unset(_lua_interpreter_names)
endif()

update_lua_multi_config()

if (Luajit_VERSION)
    message(STATUS "Luajit_VERSION = ${Luajit_VERSION}")
endif()

message(STATUS "Lua_VERSION = ${Lua_VERSION}")
message(STATUS "LUA_LIBRARIES = ${LUA_LIBRARIES}")
message(STATUS "LUA_INCLUDE_DIR = ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_INTERPRETER = ${LUA_INTERPRETER}")
