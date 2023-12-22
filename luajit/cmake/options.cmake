
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  # 64 bits
  set(USE_X64 ON)
elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
  # 32 bits
  set(USE_X64 OFF)
endif()

set(DISABLE_FFI OFF CACHE BOOL
  "Permanently disable the FFI extension to reduce the size of the LuaJIT
executable. But please consider that the FFI library is compiled-in,
but NOT loaded by default. It only allocates any memory, if you actually
make use of it."
)

set(ENABLE_LUA52COMPAT OFF CACHE BOOL
  "Features from Lua 5.2 that are unlikely to break existing code are
enabled by default. Some other features that *might* break some existing
code (e.g. __pairs or os.execute() return values) can be enabled here.
Note: this does not provide full compatibility with Lua 5.2 at this time."
)

set(DISABLE_JIT OFF CACHE BOOL "Disable the JIT compiler, i.e. turn LuaJIT into a pure interpreter.")

set(NUMMODE 0 CACHE STRING
"Some architectures (e.g. PPC) can use either single-number (1) or
dual-number (2) mode.
Please see LJ_ARCH_NUMMODE in lj_arch.h for details."
)

include(CMakeDependentOption)
if ("${Luajit_VERSION}" STREQUAL "2.1.0-beta3")
  cmake_dependent_option(ENABLE_GC64 "Enable GC64 mode for x64." OFF "USE_X64" OFF)
else()
  cmake_dependent_option(DISABLE_GC64 "Disable GC64 mode for x64." OFF "USE_X64" OFF)
endif()

set(USE_SYSMALLOC OFF CACHE BOOL
  "Use the system provided memory allocator (realloc) instead of the
bundled memory allocator. This is slower, but sometimes helpful for
debugging. This option cannot be enabled on x64 without GC64, since
realloc usually doesn't return addresses in the right address range.
OTOH this option is mandatory for Valgrind's memcheck tool on x64 and
the only way to get useful results from it for all other architectures."
)

set(USE_VALGRIND OFF CACHE BOOL
  "This define is required to run LuaJIT under Valgrind. The Valgrind
header files must be installed. You should enable debug information, too.
Use --suppressions=lj.supp to avoid some false positives."
)

set(USE_GDBJIT OFF CACHE BOOL
  "This is the client for the GDB JIT API. GDB 7.0 or higher is required
to make use of it. See lj_gdbjit.c for details. Enabling this causes
a non-negligible overhead, even when not running under GDB."
)

set(USE_APICHECK OFF CACHE BOOL
  "Turn on assertions for the Lua/C API to debug problems with lua_* calls.
This is rather slow -- use only while developing C libraries/embeddings."
)

set(USE_ASSERT OFF CACHE BOOL
  "Turn on assertions for the whole LuaJIT VM. This significantly slows down
everything. Use only if you suspect a problem with LuaJIT itself."
)

if (ANDROID AND (${CMAKE_SYSTEM_VERSION} LESS 21))
  set(ENABLE_LARGEFILE OFF)
elseif (WIN32 OR CYGWIN OR MINGW)
  set(ENABLE_LARGEFILE OFF)
else()
  set(ENABLE_LARGEFILE ON)
endif()
