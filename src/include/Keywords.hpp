#pragma once

#include <luadef.hpp>
#include <set>

namespace LUA_MODULE_NAME {
	struct Keywords {
		static void push(lua_State* L, int index, const char* key);
		static bool has(lua_State* L, int index, const char* key);
		static int size(lua_State* L, int index);
	};

	void register_Keywords(lua_State* L);
	int lua_push(lua_State* L, Keywords* raw_ptr);

	template<>
	struct is_usertype<Keywords> : std::true_type { };

	template <>
	struct usertype_info<Keywords> {
		static int metatable;
		static const void* signature;
		static std::set<const void*> derives;
		static const struct luaL_Reg methods[];
		static const struct luaL_Reg meta_methods[];
		static const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> getters;
		static const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> setters;
        static bool lua_userdata_is(lua_State* L, int index);
        static std::shared_ptr<Keywords> lua_userdata_to(lua_State* L, int index);
	};
}
