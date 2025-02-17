#pragma once

#include <luadef.hpp>
#include <opencv2/core.hpp>
#include <opencv2/gapi/util/variant.hpp>

namespace LUA_MODULE_NAME {
	inline bool lua_is(lua_State* L, int index, cv::Range*);

	template<typename T>
	inline bool lua_is(lua_State* L, int index, cv::Ptr<T>*);

	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Point_<_Tp>*);

	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Point3_<_Tp>*);

	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Rect_<_Tp>*);

	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Scalar_<_Tp>*);

	template<typename _Tp>
	inline bool lua_is(lua_State* L, int index, cv::Size_<_Tp>*);

	template<typename _Tp, int cn>
	inline bool lua_is(lua_State* L, int index, cv::Vec<_Tp, cn>*);

	template<typename... _Ts>
	inline bool lua_is(lua_State* L, int index, cv::util::variant<_Ts...>* ptr);


	inline void lua_to(lua_State* L, int index, cv::Range& range);

	inline cv::Range lua_to(lua_State* L, int index, cv::Range*);

	inline void lua_to(lua_State* L, int index, std::vector<cv::Range>& out);

	template<typename T>
	inline cv::Ptr<T> lua_to(lua_State* L, int index, cv::Ptr<T>*);

	template<typename _Tp>
	inline cv::Point_<_Tp> lua_to(lua_State* L, int index, cv::Point_<_Tp>*);

	template<typename _Tp>
	inline cv::Point3_<_Tp> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*);

	template<typename _Tp>
	inline cv::Rect_<_Tp> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*);

	template<typename _Tp>
	inline cv::Scalar_<_Tp> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*);

	template<typename _Tp>
	inline cv::Size_<_Tp> lua_to(lua_State* L, int index, cv::Size_<_Tp>*);

	template<typename _Tp, int cn>
	inline cv::Vec<_Tp, cn> lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*);

	template<typename... _Ts>
	inline cv::util::variant<_Ts...> lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr);


	inline int lua_push(lua_State* L, const cv::Range& range);

	template<typename T>
	inline int lua_push(lua_State* L, cv::Ptr<T>& ptr);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Point_<_Tp>& pt);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Point3_<_Tp>& pt);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Rect_<_Tp>& rect);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Scalar_<_Tp>& s);

	template<typename _Tp>
	inline int lua_push(lua_State* L, const cv::Size_<_Tp>& rect);

	template<typename _Tp, int cn>
	inline int lua_push(lua_State* L, const cv::Vec<_Tp, cn>& vec);

	template<typename... _Ts>
	inline int lua_push(lua_State* L, const cv::util::variant<_Ts...>& value);
}
