#pragma once

#include <lua_bridge_common.hdr.hpp>
#include <lua_bridge.hdr.hpp>
#include <lua_bridge_arrays.hdr.hpp>

#include <lua_bridge_common.hpp>
#include <lua_bridge_arrays.hpp>

namespace LUA_MODULE_NAME {
	// ================================
	// cv::Ptr
	// ================================

	template<typename T>
	inline bool lua_is(lua_State* L, int index, cv::Ptr<T>*) {
		return lua_is(L, index, static_cast<T*>(nullptr));
	}

	template<typename T>
	inline cv::Ptr<T> lua_to(lua_State* L, int index, cv::Ptr<T>*) {
		return lua_to(L, index, static_cast<T*>(nullptr));
	}

	template<typename T>
	inline int lua_push(lua_State* L, cv::Ptr<T>& ptr) {
		return lua_push(L, std::shared_ptr<T>(ptr));
	}

	// ================================
	// cv::Point_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Point_<_Tp>*) {
		return lua_is(L, index, static_cast<std::vector<_Tp>*>(nullptr), 2);
	}

	template<typename _Tp>
	inline std::shared_ptr<cv::Point_<_Tp>> lua_to(lua_State* L, int index, cv::Point_<_Tp>*) {
		auto vec = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr));
		std::shared_ptr<cv::Point_<_Tp>> res(new cv::Point_<_Tp>());

		int i = 0;
		(*res).x = (*vec).at(i++);
		(*res).y = (*vec).at(i++);

		return res;
	}

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Point_<_Tp>& pt) {
		lua_newtable(L);
		int i = 0;
		lua_push(L, pt.x); lua_rawseti(L, -2, ++i);
		lua_push(L, pt.y); lua_rawseti(L, -2, ++i);
		return 1;
	}

	// ================================
	// cv::Point3_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Point3_<_Tp>*) {
		return lua_is(L, index, static_cast<std::vector<_Tp>*>(nullptr), 3);
	}

	template<typename _Tp>
	inline std::shared_ptr<cv::Point3_<_Tp>> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*) {
		auto vec = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr));
		std::shared_ptr<cv::Point3_<_Tp>> res(new cv::Point3_<_Tp>());

		int i = 0;
		(*res).x = (*vec).at(i++);
		(*res).y = (*vec).at(i++);
		(*res).z = (*vec).at(i++);

		return res;
	}

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Point3_<_Tp>& pt) {
		lua_newtable(L);
		int i = 0;
		lua_push(L, pt.x); lua_rawseti(L, -2, ++i);
		lua_push(L, pt.y); lua_rawseti(L, -2, ++i);
		lua_push(L, pt.z); lua_rawseti(L, -2, ++i);
		return 1;
	}

	// ================================
	// cv::Rect_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Rect_<_Tp>*) {
		return lua_is(L, index, static_cast<std::vector<_Tp>*>(nullptr), 4);
	}

	template<typename _Tp>
	inline std::shared_ptr<cv::Rect_<_Tp>> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*) {
		auto vec = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr));
		std::shared_ptr<cv::Rect_<_Tp>> res(new cv::Rect_<_Tp>());

		int i = 0;
		(*res).x = (*vec).at(i++);
		(*res).y = (*vec).at(i++);
		(*res).width = (*vec).at(i++);
		(*res).height = (*vec).at(i++);

		return res;
	}

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Rect_<_Tp>& rect) {
		lua_newtable(L);
		int i = 0;
		lua_push(L, rect.x); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.y); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.width); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.height); lua_rawseti(L, -2, ++i);
		return 1;
	}

	// ================================
	// cv::Scalar_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Scalar_<_Tp>*) {
		return lua_is(L, index, static_cast<_Tp*>(nullptr)) || lua_is(L, index, static_cast<std::vector<_Tp>*>(nullptr), 4, true);
	}

	template<typename _Tp>
	inline std::shared_ptr<cv::Scalar_<_Tp>> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*) {
		if (lua_is(L, index, static_cast<_Tp*>(nullptr))) {
			return std::make_shared<cv::Scalar_<_Tp>>(lua_to(L, index, static_cast<_Tp*>(nullptr)));
		}

		auto vec = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr));
		std::shared_ptr<cv::Scalar_<_Tp>> res(new cv::Scalar_<_Tp>());

		const auto size = vec->size();
		for (int i = 0; i < size; i++) {
			(*res)[i] = (*vec).at(i);
		}

		return res;
	}

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Scalar_<_Tp>& s) {
		lua_newtable(L);
		for (int i = 0; i < 4; i++) {
			lua_push(L, s[i]);
			lua_rawseti(L, -2, i + 1);
		}
		return 1;
	}

	// ================================
	// cv::Size_
	// ================================
	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Size_<_Tp>*) {
		return lua_isnil(L, index) || lua_is(L, index, static_cast<std::vector<_Tp>*>(nullptr), 2);
	}

	template<typename _Tp>
	inline std::shared_ptr<cv::Size_<_Tp>> lua_to(lua_State* L, int index, cv::Size_<_Tp>*) {
		if (lua_isnil(L, index)) {
			return std::make_shared<cv::Size_<_Tp>>();
		}

		auto vec = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr));
		std::shared_ptr<cv::Size_<_Tp>> res(new cv::Size_<_Tp>());

		int i = 0;
		(*res).width = (*vec).at(i++);
		(*res).height = (*vec).at(i++);

		return res;
	}

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Size_<_Tp>& rect) {
		lua_newtable(L);
		int i = 0;
		lua_push(L, rect.width); lua_rawseti(L, -2, ++i);
		lua_push(L, rect.height); lua_rawseti(L, -2, ++i);
		return 1;
	}

	// ================================
	// cv::Vec
	// ================================
	template<typename _Tp, int cn>
	inline bool lua_is(lua_State* L, int index, cv::Vec<_Tp, cn>*) {
		return lua_is(L, index, static_cast<std::vector<_Tp>*>(nullptr), cn, true);
	}

	template<typename _Tp, int cn>
	inline std::shared_ptr<cv::Vec<_Tp, cn>>  lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*) {
		auto vec = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr));
		std::shared_ptr<cv::Vec<_Tp, cn>> res(new cv::Vec<_Tp, cn>());

		const auto size = vec->size();
		for (int i = 0; i < size; i++) {
			(*res)[i] = (*vec).at(i);
		}

		return res;
	}

	template<typename _Tp, int cn>
	inline int lua_push(lua_State* L, const cv::Vec<_Tp, cn>& vec) {
		lua_newtable(L);
		for (int i = 0; i < cn; i++) {
			lua_push(L, vec[i]);
			lua_rawseti(L, -2, i + 1);
		}
		return 1;
	}

	// ================================
	// cv::util::variant
	// ================================
	template<std::size_t I = 0, typename... _Ts>
	inline bool _lua_is(lua_State* L, int index, cv::util::variant<_Ts...>* ptr) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;

		if constexpr (!std::is_same_v<T, cv::util::monostate> && !std::is_same_v<T, cv::gapi::wip::draw::FText> && !std::is_same_v<T, cv::GFrameDesc>) {
			if (lua_is(L, index, static_cast<T*>(nullptr))) {
				return true;
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			return false;
		}
		else {
			return _lua_is<I + 1, _Ts...>(L, index, ptr);
		}
	}

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, cv::util::variant<_Ts...>* ptr) {
		return _lua_is(L, index, ptr);
	}

	template<std::size_t I = 0, typename... _Ts>
	inline cv::util::variant<_Ts...> _lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;
		using Arg = typename cv::util::variant<_Ts...>;

		if constexpr (!std::is_same_v<T, cv::util::monostate> && !std::is_same_v<T, cv::gapi::wip::draw::FText> && !std::is_same_v<T, cv::GFrameDesc>) {
			if (lua_is(L, index, static_cast<T*>(nullptr))) {
				Arg res;
				auto holder = lua_to(L, index, static_cast<T*>(nullptr));
				auto value = extract_holder(holder, static_cast<T*>(nullptr));
				res = value;
				return res;
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			return Arg();
		}
		else {
			return _lua_to<I + 1, _Ts...>(L, index, ptr);
		}
	}

	template<typename... _Ts>
	inline cv::util::variant<_Ts...> lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr) {
		return _lua_to(L, index, ptr);
	}

	template<std::size_t I = 0, typename... _Ts>
	inline int _lua_push(lua_State* L, const cv::util::variant<_Ts...>& value) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;
		using Arg = typename cv::util::variant<_Ts...>;

		if constexpr (!std::is_same_v<T, cv::util::monostate> && !std::is_same_v<T, cv::gapi::wip::draw::FText> && !std::is_same_v<T, cv::GFrameDesc>) {
			if (value.index() == I) {
				return lua_push(L, cv::util::get<T>(value));
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			return 0;
		}
		else {
			return _lua_push<I + 1, _Ts...>(L, value);
		}
	}

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const cv::util::variant<_Ts...>& value) {
		return _lua_push(L, value);
	}
}

namespace LUA_MODULE_NAME {
	void register_extensions(lua_State* L);
}
