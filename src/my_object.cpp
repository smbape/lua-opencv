#include <my_object/my_object.hpp>

namespace LUA_MODULE_NAME {

    sol::table LUA_MODULE_OPEN(sol::this_state L) {
        sol::state_view lua(L);
        sol::table module = lua.create_table();
        auto test_type = module.new_usertype<test>("test", sol::constructors<test(), test(int)>());
        test_type["value"] = &test::value;
        test_type["getBuildInformation"] = &test::getBuildInformation;
        return module;
    }

} // namespace LUA_MODULE_NAME

int LUA_MODULE_LUAOPEN(lua_State* L) {
    // pass the lua_State,
    // the index to start grabbing arguments from,
    // and the function itself
    // optionally, you can pass extra arguments to the function
    // if that's necessary, but that's advanced usage and is
    // generally reserved for internals only
    return sol::stack::call_lua(
         L, 1, LUA_MODULE_NAME::LUA_MODULE_OPEN);
}
