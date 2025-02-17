#include <registration.hpp>

#if LUA_VERSION_NUM < 504
int luaL_typeerror(lua_State* L, int arg, const char* tname) {
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
