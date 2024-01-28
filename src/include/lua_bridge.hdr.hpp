#pragma once

#include <luadef.hpp>
#include <opencv2/core.hpp>

namespace LUA_MODULE_NAME {
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


	template<typename T>
	inline cv::Ptr<T> lua_to(lua_State* L, int index, cv::Ptr<T>*);

	template<typename _Tp>
	inline std::shared_ptr<cv::Point_<_Tp>> lua_to(lua_State* L, int index, cv::Point_<_Tp>*);

	template<typename _Tp>
	inline std::shared_ptr<cv::Point3_<_Tp>> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*);

	template<typename _Tp>
	inline std::shared_ptr<cv::Rect_<_Tp>> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*);

	template<typename _Tp>
	inline std::shared_ptr<cv::Scalar_<_Tp>> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*);

	template<typename _Tp>
	inline std::shared_ptr<cv::Size_<_Tp>> lua_to(lua_State* L, int index, cv::Size_<_Tp>*);

	template<typename _Tp, int cn>
	inline std::shared_ptr<cv::Vec<_Tp, cn>>  lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*);

	template<typename... _Ts>
	inline cv::util::variant<_Ts...> lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr);


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
