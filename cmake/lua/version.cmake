set(Lua_VERSION_54 5.4.6)
set(Lua_URL_HASH_546 7d5ea1b9cb6aa0b59ca3dde1c6adcb57ef83a1ba8e5432c0ecd06bf439b3ad88)

set(Lua_VERSION_53 5.3.6)
set(Lua_URL_HASH_536 fc5fd69bb8736323f026672b1b7235da613d7177e72558893a0bdcd320466d60)

set(Lua_VERSION_52 5.2.4)
set(Lua_URL_HASH_524 b9e2e4aad6789b3b63a056d442f7b39f0ecfca3ae0f1fc0ae4e9614401b69f4b)

set(Lua_VERSION_51 5.1.5)
set(Lua_URL_HASH_515 2640fc56a795f29d28ef15e13c34a47e223960b0240e8cb0a82d9b0738695333)

if ((NOT DEFINED Lua_VERSION) OR ("${Lua_VERSION} " STREQUAL " "))
    set(Lua_VERSION 5.4.6)
    set(Lua_VERSION "${Lua_VERSION}" CACHE STRING "Choose the Lua version." FORCE)
else()
    set(Lua_VERSION "${Lua_VERSION}" CACHE STRING "Choose the Lua version." FORCE)

    if ("${Lua_VERSION}" STREQUAL "5.4")
        set(Lua_VERSION "${Lua_VERSION_54}")
    elseif("${Lua_VERSION}" STREQUAL "5.3")
        set(Lua_VERSION "${Lua_VERSION_53}")
    elseif("${Lua_VERSION}" STREQUAL "5.2")
        set(Lua_VERSION "${Lua_VERSION_52}")
    elseif("${Lua_VERSION}" STREQUAL "5.1")
        set(Lua_VERSION "${Lua_VERSION_51}")
    endif()
endif()

set_property(CACHE Lua_VERSION PROPERTY STRINGS "luajit-2.1" "${Lua_VERSION_54}" "${Lua_VERSION_53}" "${Lua_VERSION_52}" "${Lua_VERSION_51}")
