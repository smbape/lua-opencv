option(BUILD_LUA "Build Lua from source" ON)

if (BUILD_LUA AND (NOT WITH_LUA_ROCKS))
  set(Lua_VERSION 5.4.6 CACHE STRING "Choose the Lua version.")
  set_property(CACHE Lua_VERSION PROPERTY STRINGS "luajit-2.1" "5.4.6" "5.3.6" "5.2.4" "5.1.5")

  if (Lua_VERSION MATCHES "^luajit-")
    string(SUBSTRING "${Lua_VERSION}" 7 -1 Luajit_VERSION)
    set(LUA_SUFFIX "jit")
  else()
    set(LUA_SUFFIX "")
    unset(Luajit_VERSION CACHE)
  endif()

  add_subdirectory(lua${LUA_SUFFIX})

  set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" PROPERTY VS_STARTUP_PROJECT lua${LUA_SUFFIX})
  file(TO_NATIVE_PATH "${${CMAKE_CURRENT_SOURCE_DIR}/test}" VS_DEBUGGER_WORKING_DIRECTORY)
  set_target_properties(lua${LUA_SUFFIX} PROPERTIES
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
    string(SUBSTRING "${Lua_VERSION}" 7 -1 Luajit_VERSION)
    set(Lua_VERSION 5.1)
    set(LUA_SUFFIX "jit")
  else()
    set(LUA_SUFFIX "")
    unset(Luajit_VERSION CACHE)
  endif()

  list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")

  if (Lua_VERSION)
    find_package(Lua ${Lua_VERSION} EXACT REQUIRED)
  else()
    find_package(Lua REQUIRED)
  endif()

  unset(_lua_interpreter_names)

  if (Lua_INTERPRETER_NAME)
    set(_lua_interpreter_names ${Lua_INTERPRETER_NAME})
  else()
    if (LUA_VERSION_STRING)
      set(_lua_interpreter_names
        luajit-${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua${LUA_VERSION_MAJOR}${LUA_VERSION_MINOR}
        lua${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua-${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
        lua.${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
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
    REQUIRED
  )

  unset(_lua_interpreter_names)
endif()

message(STATUS "LUA_VERSION_STRING = ${LUA_VERSION_STRING}")
message(STATUS "LUA_LIBRARIES = ${LUA_LIBRARIES}")
message(STATUS "LUA_INCLUDE_DIR = ${LUA_INCLUDE_DIR}")
message(STATUS "LUA_INTERPRETER = ${LUA_INTERPRETER}")
