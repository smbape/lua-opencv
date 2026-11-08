#pragma once

#include <lua.hpp>
#include <cmath>
#include <cstddef>

#if (LUA_VERSION_NUM == 501) && !(defined lua_rawlen)
#define lua_rawlen lua_objlen
#endif

#if LUA_VERSION_NUM < 502
/*
 ** Convert an acceptable stack index into an absolute index
*/
inline int lua_absindex(lua_State* L, int idx) {
	return idx > 0 || idx <= LUA_REGISTRYINDEX ? idx : idx + lua_gettop(L) + 1;
}

/*
** Pushes onto the stack the value t[k],
** where t is the table at the given index and
** k is the pointer p represented as a light userdata.
*/
inline void lua_rawgetp(lua_State* L, int idx, const void* p) {
	idx = lua_absindex(L, idx);
	lua_pushlightuserdata(L, const_cast<void*>(p));
	lua_rawget(L, idx);
}

/*
 ** Does the equivalent of t[k] = v,
 ** where t is the table at the given index,
 ** k is the pointer p represented as a light userdata,
 ** and v is the value at the top of the stack.
*/
inline void lua_rawsetp(lua_State *L, int idx, const void *p) {
	idx = lua_absindex(L, idx);
	lua_pushlightuserdata(L, const_cast<void*>(p));
	lua_insert(L, -2);
	lua_rawset(L, idx);
}

/*
** set functions from list 'l' into table at top - 'nup'; each
** function gets the 'nup' elements at the top as upvalues.
** Returns with only the table at the stack.
*/
inline void luaL_setfuncs(lua_State* L, const luaL_Reg* l, int nup) {
	luaL_checkstack(L, nup, "too many upvalues");
	for (; l->name != nullptr; ++l) {  /* fill the table with given functions */
		for (int i = 0; i < nup; ++i) {  /* copy upvalues to the top */
			lua_pushvalue(L, -nup);
		}
		lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
		lua_setfield(L, -(nup + 2), l->name);
	}
	lua_pop(L, nup);  /* remove upvalues */
}
#endif

#if LUA_VERSION_NUM < 504
/*
 ** Raises a type error for the argument arg of the C function that called it,
 ** using a standard message; tname is a "name" for the expected type
*/
inline int luaL_typeerror(lua_State* L, int arg, const char* tname) {
	const char* msg;
	const char* typearg;  /* name for the type of the actual argument */
	if (luaL_getmetafield(L, arg, "__name") == LUA_TSTRING) {
		typearg = lua_tostring(L, -1);  /* use the given type name */
	}
	else if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
		typearg = "light userdata";  /* special name for messages */
	}
	else {
		typearg = luaL_typename(L, arg);  /* standard name */
	}
	msg = lua_pushfstring(L, "%s expected, got %s", tname, typearg);
	return luaL_argerror(L, arg, msg);
}
#endif

inline void* lua_compat_newuserdata(lua_State* L, std::size_t size) {
#if LUA_VERSION_NUM >= 504
	return lua_newuserdatauv(L, size, 0);
#else
	return lua_newuserdata(L, size);
#endif
}
