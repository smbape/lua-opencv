#pragma once

#if defined _WIN32
#  define LUA_CDECL __cdecl
#  define LUA_STDCALL __stdcall
#else
#  define LUA_CDECL
#  define LUA_STDCALL
#endif

#ifndef LUA_EXTERN_C
#  ifdef __cplusplus
#    define LUA_EXTERN_C extern "C"
#  else
#    define LUA_EXTERN_C
#  endif
#endif

#ifndef LUA_EXPORTS
# if (defined _WIN32 || defined WINCE || defined __CYGWIN__) && defined(LUAAPI_EXPORTS)
#   define LUA_EXPORTS __declspec(dllexport)
# elif defined __GNUC__ && __GNUC__ >= 4 && (defined(LUAAPI_EXPORTS) || defined(__APPLE__))
#   define LUA_EXPORTS __attribute__ ((visibility ("default")))
# elif defined __clang__ 
#   define LUA_EXPORTS __attribute__ ((visibility ("default")))
# else
#   define LUA_EXPORTS
# endif
#endif

#ifndef LUAAPI
#  define LUAAPI(rettype) LUA_EXTERN_C LUA_EXPORTS rettype LUA_CDECL
#endif

#ifndef LUA_MODULE_NAME
#error "LUA_MODULE_NAME must be defined"
#endif

#define _LUA_TOKEN_CONCAT(A, B) A ## B
#define LUA_TOKEN_CONCAT(A, B) _LUA_TOKEN_CONCAT(A, B)

#define LUA_MODULE_LUAOPEN LUA_TOKEN_CONCAT(luaopen_, LUA_MODULE_NAME)
#define LUA_MODULE_OPEN LUA_TOKEN_CONCAT(open_, LUA_MODULE_NAME)

#define _LUA_TOKEN_STR(A) #A
#define LUA_TOKEN_STR(A) _LUA_TOKEN_STR(A)

#define LUA_MODULE_NAME_STR LUA_TOKEN_STR(LUA_MODULE_NAME)
