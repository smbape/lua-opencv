#pragma once

// CV_EXPORTS_W : include this file in lua_generated_include

#include <lua.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <concepts>
#include <condition_variable>
#include <cstring>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

#if (LUA_VERSION_NUM == 501) && !(defined lua_rawlen)
#define lua_rawlen lua_objlen
#endif

inline void lua_pushfuncs(lua_State *L, const luaL_Reg *l) {
	for (; l->name; l++) {
		lua_pushstring(L, l->name);
		lua_pushcclosure(L, l->func, 0);
		lua_rawset(L, -3);
	}
}

#if LUA_VERSION_NUM < 504
extern int luaL_typeerror(lua_State* L, int arg, const char* tname);
#endif


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

#define _LUA_TOKEN_STR(A) #A
#define LUA_TOKEN_STR(A) _LUA_TOKEN_STR(A)

#define LUA_MODULE_LUAOPEN LUA_TOKEN_CONCAT(luaopen_, LUA_MODULE_NAME)
#define LUA_MODULE_LUAOPEN_STR LUA_TOKEN_STR(LUA_MODULE_LUAOPEN)
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
	fprintf(stdout, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Info: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), "", Lua_Module_Func, __FILE__, __LINE__); \
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

#ifndef LUAL_MODULE_ERROR
#define LUAL_MODULE_ERROR( L, _message ) do { \
	std::ostringstream _out; _out << _message;  \
	luaL_error(L, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), "", Lua_Module_Func, __FILE__, __LINE__); \
} while(0)
#endif

#ifndef LUAL_MODULE_ERROR_RETURN
#define LUAL_MODULE_ERROR_RETURN( L, _message ) do { \
	std::ostringstream _out; _out << _message;  \
	return luaL_error(L, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), "", Lua_Module_Func, __FILE__, __LINE__); \
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

#ifndef LUAL_MODULE_ASSERT
#define LUAL_MODULE_ASSERT( L, expr, _message ) do { if(!!(expr)) ; else { \
	std::ostringstream _out; _out << _message;  \
	luaL_error(L, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: %s (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), _out.str().c_str(), #expr, Lua_Module_Func, __FILE__, __LINE__); \
}} while(0)
#endif

#ifndef LUA_MODULE_ASSERT
#define LUA_MODULE_ASSERT( expr ) do { if(!!(expr)) ; else { \
fflush(stdout); fflush(stderr); \
fprintf(stderr, LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_NAME) "(%s) Error: (%s) in %s, file %s, line %d\n", LUA_MODULE_QUOTE_STRING(LUA_MODULE_LIB_VERSION), #expr, Lua_Module_Func, __FILE__, __LINE__); \
fflush(stdout); fflush(stderr); \
return E_FAIL; } \
} while(0)
#endif

// https://github.com/Manu343726/ctti/blob/master/include/ctti/detail/pretty_function.hpp#L10-L18
#if defined(__clang__)
	#define LUA_MODULE__PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(__GNUC__) && !defined(__clang__)
	#define LUA_MODULE__PRETTY_FUNCTION __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
	#define LUA_MODULE__PRETTY_FUNCTION __FUNCSIG__
#else
	#error "No support for this compiler."
#endif

// https://github.com/Manu343726/ctti/blob/master/include/ctti/detail/pretty_function.hpp#L42-L53
#if defined(__clang__)
	#define LUA_MODULE__TYPE_PRETTY_FUNCTION_PREFIX "const char* " LUA_MODULE_NAME_STR "::internal::GetTypeName() [T = "
	#define LUA_MODULE__TYPE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(__GNUC__)
	#define LUA_MODULE__TYPE_PRETTY_FUNCTION_PREFIX "const char* " LUA_MODULE_NAME_STR "::internal::GetTypeName() [with T = "
	#define LUA_MODULE__TYPE_PRETTY_FUNCTION_SUFFIX "]"
#elif defined(_MSC_VER)
	#define LUA_MODULE__TYPE_PRETTY_FUNCTION_PREFIX "const char *__cdecl " LUA_MODULE_NAME_STR "::internal::GetTypeName<"
	#define LUA_MODULE__TYPE_PRETTY_FUNCTION_SUFFIX ">(void)"
#else
	#error "No support for this compiler."
#endif

#define LUA_MODULE__TYPE_PRETTY_FUNCTION_LEFT (sizeof(LUA_MODULE__TYPE_PRETTY_FUNCTION_PREFIX) - 1)
#define LUA_MODULE__TYPE_PRETTY_FUNCTION_RIGHT (sizeof(LUA_MODULE__TYPE_PRETTY_FUNCTION_SUFFIX) - 1)

namespace LUA_MODULE_NAME {
	// https://blog.molecular-matters.com/2015/12/11/getting-the-type-of-a-template-argument-as-string-without-rtti/
	namespace internal {
		template <typename T>
		const char* GetTypeName(void) {
			constexpr size_t size = sizeof(LUA_MODULE__PRETTY_FUNCTION) - LUA_MODULE__TYPE_PRETTY_FUNCTION_LEFT - LUA_MODULE__TYPE_PRETTY_FUNCTION_RIGHT;
			static char typeName[size] = {};
			memcpy(typeName, LUA_MODULE__PRETTY_FUNCTION + LUA_MODULE__TYPE_PRETTY_FUNCTION_LEFT, size - 1u);
			return typeName;
			// return LUA_MODULE__PRETTY_FUNCTION;
		}


		inline std::string LuaTypeName(lua_State* L, int arg) {
			if (luaL_getmetafield(L, arg, "__name") != 0) {
				if (lua_type(L, -1) == LUA_TSTRING) {
					std::string typearg = lua_tostring(L, -1);  /* use the given type name */
					lua_pop(L, 1);
					return typearg;
				}
				lua_pop(L, 1);
			}

			if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
				return "light userdata";  /* special name for messages */
			}

			return luaL_typename(L, arg);  /* standard name */
		}

	}
}

namespace LUA_MODULE_NAME {
	// ================================
	// is_usertype generics
	// ================================

	template<typename T>
	struct is_usertype : std::false_type {};

	template<typename T>
	constexpr inline bool is_usertype_v = is_usertype<T>::value;

	template<class T>
	struct is_usertype_pointer : std::false_type {};

	template<typename T>
	constexpr inline bool is_usertype_pointer_v = is_usertype_pointer<T>::value;

	template<typename T>
	struct usertype_info;

	template<typename T>
	struct is_basetype : std::false_type {};

	template<typename T>
	constexpr inline bool is_basetype_v = is_basetype<T>::value;

	template<typename T>
	struct basetype_info;

	template<typename T>
	const char* getmetatable_name() {
		static const auto tname = std::string(LUA_MODULE_NAME_STR "::") + std::string(internal::GetTypeName<T>());
		return tname.c_str();
	}

	// Source - https://stackoverflow.com/questions/11251376/how-can-i-check-if-a-type-is-an-instantiation-of-a-given-class-template#11251408
	// Posted by Luc Touraille
	// Retrieved 2026-05-20, License - CC BY-SA 3.0

	template<template<typename...> class Template, typename T>
	struct is_instantiation_of : std::false_type {};

	template<template<typename...> class Template, typename... Types>
	struct is_instantiation_of<Template, Template<Types...>> : std::true_type {};

	template <template <typename...> class Template, typename T>
	constexpr inline bool is_instantiation_of_v = is_instantiation_of<Template, T>::value;
}

#ifndef CV_PROP_W
#define CV_PROP_W
#endif

// https://github.com/opencv/opencv/blob/4.12.0/modules/core/include/opencv2/core/cvdef.h#L471-L486
#ifndef CV_EXPORTS
# define CV_EXPORTS

/* special informative macros for wrapper generators */
#define CV_EXPORTS_W CV_EXPORTS
#define CV_EXPORTS_W_SIMPLE CV_EXPORTS
#define CV_EXPORTS_AS(synonym) CV_EXPORTS
#define CV_EXPORTS_W_MAP CV_EXPORTS
#define CV_EXPORTS_W_PARAMS CV_EXPORTS
#define CV_IN_OUT
#define CV_OUT
#define CV_PROP
#define CV_PROP_RW
#define CV_ND // Indicates that input data should be parsed into Mat without channels
#define CV_WRAP
#define CV_WRAP_AS(synonym)
#define CV_WRAP_MAPPABLE(mappable)
#define CV_WRAP_PHANTOM(phantom_header)
#define CV_WRAP_DEFAULT(val)
#endif
