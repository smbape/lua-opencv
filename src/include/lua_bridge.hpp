#pragma once

#include <lua_bridge_common.hdr.hpp>
#include <lua_bridge.hdr.hpp>
#include <lua_bridge_arrays.hdr.hpp>

#include <lua_bridge_common.hpp>
#include <lua_bridge_arrays.hpp>

namespace LUA_MODULE_NAME {
	// ================================
	// cv::Range
	// ================================
	
	inline bool lua_is(lua_State* L, int index, cv::Range*) {
		return lua_is(L, index, static_cast<std::vector<int>*>(nullptr), 2);
	}

	inline void lua_to(lua_State* L, int index, cv::Range& range) {
		static std::vector<int> vec;
		lua_to(L, index, vec);
		range.start = vec.at(0);
		range.end = vec.at(1);
	}

	inline cv::Range lua_to(lua_State* L, int index, cv::Range*) {
		static std::vector<int> vec;
		lua_to(L, index, vec);
		return cv::Range(vec.at(0), vec.at(1));
	}

	template<>
	inline void lua_to<cv::Range>(lua_State* L, int index, std::vector<cv::Range>& out) {
		if (lua_isuserdata(L, index)) {
			out = *lua_userdata_to(L, index, static_cast<std::vector<cv::Range>*>(nullptr));
			return;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		auto size = lua_rawlen(L, index);
		out.resize(size);

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index); // push range table

			lua_pushnumber(L, 1);
			lua_rawget(L, -2); // push start
			out[i - 1].start = lua_to(L, -1, static_cast<int*>(nullptr));
			lua_pop(L, 1); // pop start

			lua_pushnumber(L, 2);
			lua_rawget(L, -2); // push end
			out[i - 1].end = lua_to(L, -1, static_cast<int*>(nullptr));
			lua_pop(L, 1); // pop end

			lua_pop(L, 1); // pop range table
		}
	}

	inline int lua_push(lua_State* L, const cv::Range& range) {
		lua_newtable(L);
		int i = 0;
		lua_push(L, range.start); lua_rawseti(L, -2, ++i);
		lua_push(L, range.end); lua_rawseti(L, -2, ++i);
		return 1;
	}

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
	inline cv::Point_<_Tp> lua_to(lua_State* L, int index, cv::Point_<_Tp>*) {
		static std::vector<_Tp> vec;
		lua_to(L, index, vec);
		return cv::Point_<_Tp>(vec.at(0), vec.at(1));
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
	inline cv::Point3_<_Tp> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*) {
		static std::vector<_Tp> vec;
		lua_to(L, index, vec);
		return cv::Point3_<_Tp>(vec.at(0), vec.at(1), vec.at(2));
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
	inline cv::Rect_<_Tp> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*) {
		static std::vector<_Tp> vec;
		lua_to(L, index, vec);
		return cv::Rect_<_Tp>(vec.at(0), vec.at(1), vec.at(2), vec.at(3));
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
	inline cv::Scalar_<_Tp> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*) {
		if (lua_is(L, index, static_cast<_Tp*>(nullptr))) {
			return cv::Scalar_<_Tp>(lua_to(L, index, static_cast<_Tp*>(nullptr)));
		}

		static std::vector<_Tp> vec;
		lua_to(L, index, vec);

		switch (vec.size()) {
		case 1:
			return cv::Scalar_<_Tp>(vec.at(0));
		case 2:
			return cv::Scalar_<_Tp>(vec.at(0), vec.at(1));
		case 3:
			return cv::Scalar_<_Tp>(vec.at(0), vec.at(1), vec.at(2));
		case 4:
			return cv::Scalar_<_Tp>(vec.at(0), vec.at(1), vec.at(2), vec.at(3));
		default:
			return cv::Scalar_<_Tp>();
		}
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
	inline cv::Size_<_Tp> lua_to(lua_State* L, int index, cv::Size_<_Tp>*) {
		if (lua_isnil(L, index)) {
			return cv::Size_<_Tp>();
		}
		static std::vector<_Tp> vec;
		lua_to(L, index, vec);
		return cv::Size_<_Tp>(vec.at(0), vec.at(1));
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
	inline cv::Vec<_Tp, cn>  lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*) {
		static std::vector<_Tp> vec;
		lua_to(L, index, vec);

		static cv::Vec<_Tp, cn> res;

		const auto size = vec.size();

		for (int i = 0; i < size; i++) {
			res[i] = vec.at(i);
		}

		for (int i = size; i < cn; i++) {
			res[i] = _Tp(0);
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
