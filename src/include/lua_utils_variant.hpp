#pragma once

// CV_EXPORTS_W : include this file in lua_generated_include

#include <lua_utils.hpp>
#include <lua_generated_include.hpp>

namespace LUA_MODULE_NAME {
    template<typename _Tp>
    inline auto return_as_impl(std::vector<std::tuple<cv::Point_<_Tp>, double>>& vec, sol::state_view& lua) {
        sol::table res = lua.create_table();
        int i = 0;
        for (const auto& obj : vec) {
            sol::table elem = lua.create_table();
            elem[1] = std::get<0>(obj);
            elem[2] = std::get<1>(obj);
            res[++i] = elem;
        }
        return res;
    }

    template<typename _Tp>
    inline auto return_as_impl(const std::vector<std::tuple<cv::Point_<_Tp>, double>>& vec, sol::state_view& lua) {
        return return_as_impl(const_cast<std::vector<std::tuple<cv::Point_<_Tp>, double>>&>(vec), lua);
    }
}
