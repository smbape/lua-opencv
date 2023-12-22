
if(MSVC)
  add_compile_options(/MP)
endif()

if(NOT WIN32)
  add_compile_options(-Wall)
endif()

if(MSVC)
  add_compile_definitions(_CRT_SECURE_NO_DEPRECATE)
  add_compile_definitions("_CRT_STDIO_INLINE=__declspec(dllexport)__inline")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  # 32 bits
  if(MSVC)
    add_compile_options(/arch:SSE2)
  else()
    add_compile_options(-march=i686 -msse -msse2 -mfpmath=sse)
  endif()
endif()

foreach(feature
  DISABLE_FFI
  ENABLE_LUA52COMPAT
  DISABLE_JIT
  ENABLE_GC64
  DISABLE_GC64
  USE_SYSMALLOC
  USE_VALGRIND
  USE_GDBJIT
)
  if (${feature})
    add_compile_definitions(LUAJIT_${feature})
  endif()
endforeach()

foreach(feature
  USE_APICHECK
  USE_ASSERT
)
  if (${feature})
    add_compile_definitions(LUA_${feature})
  endif()
endforeach()

if ((NUMMODE EQUAL 1) OR (NUMMODE EQUAL 2))
  add_compile_definitions(LUAJIT_NUMMODE=${NUMMODE})
endif()
