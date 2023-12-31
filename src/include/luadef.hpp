#pragma once

// CV_EXPORTS_W : include this file in lua_generated_include

#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

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

#ifndef LUA_MODULE_LIB_NAME
#error "LUA_MODULE_LIB_NAME must be defined"
#endif

#ifndef LUA_MODULE_LIB_VERSION
#error "LUA_MODULE_LIB_VERSION must be defined"
#endif

#define _LUA_TOKEN_CONCAT(A, B) A ## B
#define LUA_TOKEN_CONCAT(A, B) _LUA_TOKEN_CONCAT(A, B)

#define LUA_MODULE_LUAOPEN LUA_TOKEN_CONCAT(luaopen_, LUA_MODULE_NAME)
#define LUA_MODULE_OPEN LUA_TOKEN_CONCAT(open_, LUA_MODULE_NAME)

#define _LUA_TOKEN_STR(A) #A
#define LUA_TOKEN_STR(A) _LUA_TOKEN_STR(A)

#define LUA_MODULE_NAME_STR LUA_TOKEN_STR(LUA_MODULE_NAME)

#ifdef Lua_Module_Func
// keep current value (through OpenCV port file)
#elif defined __GNUC__ || (defined (__cpluscplus) && (__cpluscplus >= 201103))
#define Lua_Module_Func __func__
#elif defined __clang__ && (__clang_minor__ * 100 + __clang_major__ >= 305)
#define Lua_Module_Func __func__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION >= 199901)
#define Lua_Module_Func __func__
#elif defined _MSC_VER
#define Lua_Module_Func __FUNCTION__
#elif defined(__INTEL_COMPILER) && (_INTEL_COMPILER >= 600)
#define Lua_Module_Func __FUNCTION__
#elif defined __IBMCPP__ && __IBMCPP__ >=500
#define Lua_Module_Func __FUNCTION__
#elif defined __BORLAND__ && (__BORLANDC__ >= 0x550)
#define Lua_Module_Func __FUNC__
#else
#define Lua_Module_Func "<unknown>"
#endif

#ifndef LUA_MODULE_QUOTE_STRING2
#define LUA_MODULE_QUOTE_STRING2(x) #x
#endif
#ifndef LUA_MODULE_QUOTE_STRING
#define LUA_MODULE_QUOTE_STRING(x) LUA_MODULE_QUOTE_STRING2(x)
#endif

#ifndef LUA_MODULE_INFO
#define LUA_MODULE_INFO( _message ) do { \
    std::ostringstream _out; _out << _message;  \
    fflush(stdout); fflush(stderr);         \
    fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Info: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), "", Lua_Module_Func, __FILE__, __LINE__); \
    fflush(stdout); fflush(stderr);         \
} while(0)
#endif

#ifndef LUA_MODULE_WARN
#define LUA_MODULE_WARN( _message ) do { \
    std::ostringstream _out; _out << _message;  \
    fflush(stdout); fflush(stderr);         \
    fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Warning: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), "", Lua_Module_Func, __FILE__, __LINE__); \
    fflush(stdout); fflush(stderr);         \
} while(0)
#endif

#ifndef LUA_MODULE_ERROR
#define LUA_MODULE_ERROR( _message ) do { \
    std::ostringstream _out; _out << _message;  \
    fflush(stdout); fflush(stderr);         \
    fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), "", Lua_Module_Func, __FILE__, __LINE__); \
    fflush(stdout); fflush(stderr);         \
} while(0)
#endif

#ifndef LUA_MODULE_THROW
#define LUA_MODULE_THROW( _message ) do { \
    std::ostringstream _out; _out << _message;  \
    fflush(stdout); fflush(stderr);         \
    fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), "", Lua_Module_Func, __FILE__, __LINE__); \
    fflush(stdout); fflush(stderr);           \
    throw std::runtime_error(_out.str().c_str()); \
} while(0)
#endif

#ifndef LUA_MODULE_ASSERT_THROW
#define LUA_MODULE_ASSERT_THROW( expr, _message ) do { if(!!(expr)) ; else { \
    std::ostringstream _out; _out << _message;  \
    fflush(stdout); fflush(stderr);         \
    fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), #expr, Lua_Module_Func, __FILE__, __LINE__); \
    fflush(stdout); fflush(stderr);         \
    throw std::runtime_error(_out.str().c_str());    \
}} while(0)
#endif

#ifndef LUA_MODULE_ASSERT_SET_HR
#define LUA_MODULE_ASSERT_SET_HR( expr ) do { if(!!(expr)) { hr = S_OK; } else { \
fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), #expr, Lua_Module_Func, __FILE__, __LINE__); \
hr = E_FAIL; } \
} while(0)
#endif

#ifndef LUA_MODULE_ASSERT
#define LUA_MODULE_ASSERT( expr ) do { if(!!(expr)) ; else { \
fflush(stdout); fflush(stderr); \
fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), #expr, Lua_Module_Func, __FILE__, __LINE__); \
fflush(stdout); fflush(stderr); \
return E_FAIL; } \
} while(0)
#endif
