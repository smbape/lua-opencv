#pragma once

#include <lua_bridge_common.hdr.hpp>
#include <lua_bridge.hdr.hpp>
#include <lua_bridge_arrays.hdr.hpp>

#include <lua_bridge_common.hpp>
#include <lua_generated_include.hpp>
#include <register_all.hpp>

#include <lua_bridge_arrays.hpp>

namespace LUA_MODULE_NAME {
#ifdef HAVE_OPENCV_DNN
	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L7-L37
	inline cv::dnn::DictValue lua_to(lua_State* L, int index, cv::dnn::DictValue*, bool& is_valid) {
		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return cv::dnn::DictValue();
		}

		{
			auto value = lua_to(L, index, static_cast<int64*>(nullptr), is_valid);
			if (is_valid) {
				return cv::dnn::DictValue(value);
			}
		}

		{
			auto value = lua_to(L, index, static_cast<double*>(nullptr), is_valid);
			if (is_valid) {
				return cv::dnn::DictValue(value);
			}
		}

		{
			auto value = lua_to(L, index, static_cast<std::string*>(nullptr), is_valid);
			if (is_valid) {
				return cv::dnn::DictValue(value);
			}
		}

		return cv::dnn::DictValue();
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L40-L51
	template<typename T>
	int lua_push_dict_value(lua_State* L, const cv::dnn::DictValue& dv) {
		if (dv.size() > 1) {
			std::vector<T> vec(dv.size());
			for (int i = 0; i < dv.size(); ++i) {
				vec[i] = dv.get<T>(i);
			}
			return lua_push(L, vec);
		}

		return lua_push(L, dv.get<T>());
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L54-L61
	inline int lua_push(lua_State* L, const cv::dnn::DictValue& dv) {
		if (dv.isInt()) return lua_push_dict_value<int>(L, dv);
		if (dv.isReal()) return lua_push_dict_value<float>(L, dv);
		if (dv.isString()) return lua_push_dict_value<cv::String>(L, dv);
		return luaL_error(L, "Unknown value type");
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L75-L88
	inline void lua_to(lua_State* L, int index, cv::dnn::LayerParams& lp, bool& is_valid) {
		using K = std::string;
		using V = cv::dnn::DictValue;
		using Map = std::map<K, V>;

		auto kv_holder = lua_to(L, index, static_cast<Map*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) kv = extract_holder(kv_holder, static_cast<Map*>(nullptr));

		for (const auto& [k, v] : kv) {
			lp.set(k, v);
		}
	}

	inline cv::dnn::LayerParams lua_to(lua_State* L, int index, cv::dnn::LayerParams*, bool& is_valid) {
		cv::dnn::LayerParams lp;
		lua_to(L, index, lp, is_valid);
		return lp;
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L65-L72
	inline int lua_push(lua_State* L, const cv::dnn::LayerParams& lp) {
		lua_newtable(L);
		int index = lua_gettop(L);
		int i = 0;
		for (auto it = lp.begin(); it != lp.end(); ++it) {
			lua_push(L, it->first);
			lua_push(L, it->second);
			lua_rawset(L, index);
		}
		return 1;
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L91-L94
	inline int lua_push(lua_State* L, const std::vector<cv::dnn::Target>& t) {
		return lua_push(L, std::vector<int>(t.begin(), t.end()));
	}
#endif

#ifdef HAVE_OPENCV_FLANN
	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L18-L103
	inline void lua_to(lua_State* L, int index, cv::flann::IndexParams& p, bool& is_valid) {
		using K = std::string;
		using V = cv::util::variant<bool, int, double>;
		using Map = std::map<K, V>;

		auto kv_holder = lua_to(L, index, static_cast<Map*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) kv = extract_holder(kv_holder, static_cast<Map*>(nullptr));

		for (const auto& [k, v] : kv) {
			if (cv::util::holds_alternative<bool>(v)) {
				p.setBool(k, cv::util::get<bool>(v));
			} else if (cv::util::holds_alternative<int>(v)) {
				if (k == "algorithm") {
					p.setAlgorithm(cv::util::get<int>(v));
				} else {
					p.setInt(k, cv::util::get<int>(v));
				}
			} else if (cv::util::holds_alternative<double>(v)) {
				if (k == "eps") {
					p.setFloat(k, static_cast<float>(cv::util::get<double>(v)));
				} else {
					p.setDouble(k, cv::util::get<double>(v));
				}
			}
		}
	}

	inline std::shared_ptr<cv::flann::IndexParams> lua_to(lua_State* L, int index, cv::flann::IndexParams*, bool& is_valid) {
		auto p = std::make_shared<cv::flann::IndexParams>();
		lua_to(L, index, *p, is_valid);
		return p;
	}

	inline void lua_to(lua_State* L, int index, cv::flann::SearchParams& value, bool& is_valid) {
		lua_to(L, index, static_cast<cv::flann::IndexParams&>(value), is_valid);
	}

	inline std::shared_ptr<cv::flann::SearchParams> lua_to(lua_State* L, int index, cv::flann::SearchParams*, bool& is_valid) {
		auto value = std::make_shared<cv::flann::SearchParams>();
		lua_to(L, index, *value, is_valid);
		return value;
	}
#endif

#ifdef HAVE_OPENCV_GAPI
	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L179-L199
	inline cv::gapi::wip::draw::Prim lua_to(lua_State* L, int index, cv::gapi::wip::draw::Prim*, bool& is_valid) {
		cv::gapi::wip::draw::Prim prim;

#define TRY_EXTRACT(Primitive)                                                                                     \
		{                                                                                                          \
			auto value_holder = lua_to(L, index, static_cast<cv::gapi::wip::draw::Primitive*>(nullptr), is_valid); \
			if (is_valid) {                                                                                        \
				prim = extract_holder(value_holder, static_cast<cv::gapi::wip::draw::Primitive*>(nullptr));        \
				return prim;                                                                                       \
			}                                                                                                      \
		}                                                                                                          \

		TRY_EXTRACT(Rect)
		TRY_EXTRACT(Text)
		TRY_EXTRACT(Circle)
		TRY_EXTRACT(Line)
		TRY_EXTRACT(Mosaic)
		TRY_EXTRACT(Image)
		TRY_EXTRACT(Poly)
#undef TRY_EXTRACT

		return prim;
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L149-L170
	inline int lua_push(lua_State* L, const cv::gapi::wip::draw::Prim& prim) {
		switch (prim.index())
		{
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Rect>():
				return lua_push(L, cv::util::get<cv::gapi::wip::draw::Rect>(prim));
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Text>():
				return lua_push(L, cv::util::get<cv::gapi::wip::draw::Text>(prim));
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Circle>():
				return lua_push(L, cv::util::get<cv::gapi::wip::draw::Circle>(prim));
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Line>():
				return lua_push(L, cv::util::get<cv::gapi::wip::draw::Line>(prim));
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Poly>():
				return lua_push(L, cv::util::get<cv::gapi::wip::draw::Poly>(prim));
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Mosaic>():
				return lua_push(L, cv::util::get<cv::gapi::wip::draw::Mosaic>(prim));
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Image>():
				return lua_push(L, cv::util::get<cv::gapi::wip::draw::Image>(prim));
		}

		return luaL_error(L, "Unsupported draw primitive type");
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L208-L226
	inline cv::GMetaArg lua_to(lua_State* L, int index, cv::GMetaArg*, bool& is_valid) {
		cv::GMetaArg arg;

#define TRY_EXTRACT(Meta)                                                                \
		{                                                                                \
			auto value_holder = lua_to(L, index, static_cast<Meta*>(nullptr), is_valid); \
			if (is_valid) {                                                              \
				arg = extract_holder(value_holder, static_cast<Meta*>(nullptr));         \
				return arg;                                                              \
			}                                                                            \
		}                                                                                \

		TRY_EXTRACT(cv::GMatDesc)
		TRY_EXTRACT(cv::GScalarDesc)
		TRY_EXTRACT(cv::GArrayDesc)
		TRY_EXTRACT(cv::GOpaqueDesc)
#undef TRY_EXTRACT

		return arg;
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L270-L274
	inline std::shared_ptr<cv::GArg> lua_to(lua_State* L, int index, cv::GArg*, bool& is_valid) {
		is_valid = false;
		// value = cv::GArg(cv::detail::PyObjectHolder(obj));
		luaL_error(L, "Setting a cv::GArg is not yet supported");
		return std::shared_ptr<cv::GArg>();
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L236-L267
	int lua_push(lua_State* L, const cv::GArg& value) {
		GAPI_Assert(value.kind != cv::detail::ArgKind::GOBJREF);
	#define HANDLE_CASE(T, O) case cv::detail::OpaqueKind::CV_##T:  \
		{                                                           \
			return lua_push(L, value.get<O>());                   \
		}

	#define UNSUPPORTED(T) case cv::detail::OpaqueKind::CV_##T: break
		switch (value.opaque_kind)
		{
			HANDLE_CASE(BOOL,      bool);
			HANDLE_CASE(INT,       int);
			HANDLE_CASE(INT64,     int64_t);
			HANDLE_CASE(UINT64,    uint64_t);
			HANDLE_CASE(DOUBLE,    double);
			HANDLE_CASE(FLOAT,     float);
			HANDLE_CASE(STRING,    std::string);
			HANDLE_CASE(POINT,     cv::Point);
			HANDLE_CASE(POINT2F,   cv::Point2f);
			HANDLE_CASE(POINT3F,   cv::Point3f);
			HANDLE_CASE(SIZE,      cv::Size);
			HANDLE_CASE(RECT,      cv::Rect);
			HANDLE_CASE(SCALAR,    cv::Scalar);
			HANDLE_CASE(MAT,       cv::Mat);
			// HANDLE_CASE(UNKNOWN,   cv::detail::PyObjectHolder); // TODO
			HANDLE_CASE(DRAW_PRIM, cv::gapi::wip::draw::Prim);
	#undef HANDLE_CASE
	#undef UNSUPPORTED
		}
		return luaL_error(L, "Unsupported kernel input type");
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L301-L326
	inline int lua_push(lua_State* L, const cv::detail::OpaqueRef& o) {
		switch (o.getKind())
		{
			case cv::detail::OpaqueKind::CV_BOOL      : return lua_push(L, o.rref<bool>());
			case cv::detail::OpaqueKind::CV_INT       : return lua_push(L, o.rref<int>());
			case cv::detail::OpaqueKind::CV_INT64     : return lua_push(L, o.rref<int64_t>());
			case cv::detail::OpaqueKind::CV_UINT64    : return lua_push(L, o.rref<uint64_t>());
			case cv::detail::OpaqueKind::CV_DOUBLE    : return lua_push(L, o.rref<double>());
			case cv::detail::OpaqueKind::CV_FLOAT     : return lua_push(L, o.rref<float>());
			case cv::detail::OpaqueKind::CV_STRING    : return lua_push(L, o.rref<std::string>());
			case cv::detail::OpaqueKind::CV_POINT     : return lua_push(L, o.rref<cv::Point>());
			case cv::detail::OpaqueKind::CV_POINT2F   : return lua_push(L, o.rref<cv::Point2f>());
			case cv::detail::OpaqueKind::CV_POINT3F   : return lua_push(L, o.rref<cv::Point3f>());
			case cv::detail::OpaqueKind::CV_SIZE      : return lua_push(L, o.rref<cv::Size>());
			case cv::detail::OpaqueKind::CV_RECT      : return lua_push(L, o.rref<cv::Rect>());
			case cv::detail::OpaqueKind::CV_UNKNOWN   : return lua_push(L, o.rref<cv::GArg>());
			case cv::detail::OpaqueKind::CV_DRAW_PRIM : return lua_push(L, o.rref<cv::gapi::wip::draw::Prim>());
			case cv::detail::OpaqueKind::CV_SCALAR    : break;
			case cv::detail::OpaqueKind::CV_MAT       : break;
		}

		return luaL_error(L, "Unsupported GOpaque type");
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L328-L352
	inline int lua_push(lua_State* L, const cv::detail::VectorRef& v) {
		switch (v.getKind())
		{
			case cv::detail::OpaqueKind::CV_BOOL      : return lua_push(L, v.rref<bool>());
			case cv::detail::OpaqueKind::CV_INT       : return lua_push(L, v.rref<int>());
			case cv::detail::OpaqueKind::CV_INT64     : return lua_push(L, v.rref<int64_t>());
			case cv::detail::OpaqueKind::CV_UINT64    : return lua_push(L, v.rref<uint64_t>());
			case cv::detail::OpaqueKind::CV_DOUBLE    : return lua_push(L, v.rref<double>());
			case cv::detail::OpaqueKind::CV_FLOAT     : return lua_push(L, v.rref<float>());
			case cv::detail::OpaqueKind::CV_STRING    : return lua_push(L, v.rref<std::string>());
			case cv::detail::OpaqueKind::CV_POINT     : return lua_push(L, v.rref<cv::Point>());
			case cv::detail::OpaqueKind::CV_POINT2F   : return lua_push(L, v.rref<cv::Point2f>());
			case cv::detail::OpaqueKind::CV_POINT3F   : return lua_push(L, v.rref<cv::Point3f>());
			case cv::detail::OpaqueKind::CV_SIZE      : return lua_push(L, v.rref<cv::Size>());
			case cv::detail::OpaqueKind::CV_RECT      : return lua_push(L, v.rref<cv::Rect>());
			case cv::detail::OpaqueKind::CV_SCALAR    : return lua_push(L, v.rref<cv::Scalar>());
			case cv::detail::OpaqueKind::CV_MAT       : return lua_push(L, v.rref<cv::Mat>());
			case cv::detail::OpaqueKind::CV_UNKNOWN   : return lua_push(L, v.rref<cv::GArg>());
			case cv::detail::OpaqueKind::CV_DRAW_PRIM : return lua_push(L, v.rref<cv::gapi::wip::draw::Prim>());
		}

		return luaL_error(L, "Unsupported GArray type");
	}

	// modules/gapi/include/opencv2/gapi/garg.hpp#L98-L109
	inline cv::GRunArg lua_to(lua_State* L, int index, cv::GRunArg*, bool& is_valid) {
		is_valid = false;
		luaL_error(L, "Setting a cv::GRunArg is not allowed");
		return cv::GRunArg();
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L355-L374
	inline int lua_push(lua_State* L, const cv::GRunArg& v) {
		switch (v.index())
		{
			case cv::GRunArg::index_of<cv::Mat>():
				return lua_push(L, cv::util::get<cv::Mat>(v));

			case cv::GRunArg::index_of<cv::Scalar>():
				return lua_push(L, cv::util::get<cv::Scalar>(v));

			case cv::GRunArg::index_of<cv::detail::VectorRef>():
				return lua_push(L, cv::util::get<cv::detail::VectorRef>(v));

			case cv::GRunArg::index_of<cv::detail::OpaqueRef>():
				return lua_push(L, cv::util::get<cv::detail::OpaqueRef>(v));
		}

		return luaL_error(L, "Failed to unpack GRunArgs. Index of variant is unknown");
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L387-L406
	inline int lua_push(lua_State* L, const cv::GOptRunArg& v) {
		switch (v.index())
		{
			case cv::GOptRunArg::index_of<cv::optional<cv::Mat>>():
				return lua_push(L, cv::util::get<cv::optional<cv::Mat>>(v));

			case cv::GOptRunArg::index_of<cv::optional<cv::Scalar>>():
				return lua_push(L, cv::util::get<cv::optional<cv::Scalar>>(v));

			case cv::GOptRunArg::index_of<cv::optional<cv::detail::VectorRef>>():
				return lua_push(L, cv::util::get<cv::optional<cv::detail::VectorRef>>(v));

			case cv::GOptRunArg::index_of<cv::optional<cv::detail::OpaqueRef>>():
				return lua_push(L, cv::util::get<cv::optional<cv::detail::OpaqueRef>>(v));
		}

		return luaL_error(L, "Failed to unpack GOptRunArg. Index of variant is unknown");
	}

#endif

	// ================================
	// cv::Range
	// ================================

	inline void lua_to(lua_State* L, int index, cv::Range& range, bool& is_valid) {
		auto vec_holder = lua_to(L, index, static_cast<std::pair<int, int>*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::pair<int, int>*>(nullptr));
		range.start = vec.first;
		range.end = vec.second;
	}

	inline cv::Range lua_to(lua_State* L, int index, cv::Range*, bool& is_valid) {
		cv::Range out;
		lua_to(L, index, out, is_valid);
		return out;
	}

	inline int lua_push(lua_State* L, const cv::Range& range) {
		lua_newtable(L);
		int i = 0;
		lua_push(L, range.start); lua_rawseti(L, -2, ++i);
		lua_push(L, range.end); lua_rawseti(L, -2, ++i);
		return 1;
	}

	inline void lua_to(lua_State* L, int index, std::vector<cv::Range>& ranges, bool& is_valid) {
		if (lua_isuserdata(L, index)) {
			ranges = *lua_userdata_to(L, index, static_cast<std::vector<cv::Range>*>(nullptr), is_valid);
			return;
		}

		auto vec_holder = lua_to(L, index, static_cast<std::vector<std::pair<int, int>>*>(nullptr), is_valid);
		if (!is_valid) {
			return;
		}
		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::vector<std::pair<int, int>>*>(nullptr));

		ranges.resize(vec.size());

		auto i = 0;
		for (const auto& [start, end] : vec) {
			ranges[i].start = start;
			ranges[i].end = end;
			i++;
		}
	}


	// ================================
	// cv::Ptr
	// ================================

	template<typename T>
	inline cv::Ptr<T> lua_to(lua_State* L, int index, cv::Ptr<T>*, bool& is_valid) {
		return lua_to(L, index, static_cast<T*>(nullptr), is_valid);
	}

	template<typename T>
	inline int lua_push(lua_State* L, cv::Ptr<T>& ptr) {
		return lua_push(L, std::shared_ptr<T>(ptr));
	}


	// ================================
	// cv::Point_
	// ================================

	template<typename _Tp>
	inline cv::Point_<_Tp> lua_to(lua_State* L, int index, cv::Point_<_Tp>*, bool& is_valid) {
		auto pt_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Point_<_Tp>();
		}
		decltype(auto) pt = extract_holder(pt_holder, static_cast<std::tuple<_Tp, _Tp>*>(nullptr));

		return cv::Point_<_Tp>(std::get<0>(pt), std::get<1>(pt));
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
	inline cv::Point3_<_Tp> lua_to(lua_State* L, int index, cv::Point3_<_Tp>*, bool& is_valid) {
		auto pt_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Point3_<_Tp>();
		}
		decltype(auto) pt = extract_holder(pt_holder, static_cast<std::tuple<_Tp, _Tp, _Tp>*>(nullptr));

		return cv::Point3_<_Tp>(std::get<0>(pt), std::get<1>(pt), std::get<2>(pt));
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
	inline cv::Rect_<_Tp> lua_to(lua_State* L, int index, cv::Rect_<_Tp>*, bool& is_valid) {
		auto rect_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp, _Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Rect_<_Tp>();
		}
		decltype(auto) rect = extract_holder(rect_holder, static_cast<std::tuple<_Tp, _Tp, _Tp, _Tp>*>(nullptr));

		return cv::Rect_<_Tp>(std::get<0>(rect), std::get<1>(rect), std::get<2>(rect), std::get<3>(rect));
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
	inline cv::Scalar_<_Tp> lua_to(lua_State* L, int index, cv::Scalar_<_Tp>*, bool& is_valid) {
		auto v = lua_to(L, index, static_cast<_Tp*>(nullptr), is_valid);
		if (is_valid) {
			return cv::Scalar_<_Tp>(v);
		}

		auto vec_holder = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr), is_valid, 4, true);
		if (!is_valid) {
			return cv::Scalar_<_Tp>();
		}
		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::vector<_Tp>*>(nullptr));

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
	inline cv::Size_<_Tp> lua_to(lua_State* L, int index, cv::Size_<_Tp>*, bool& is_valid) {
		if (lua_isnil(L, index)) {
			return cv::Size_<_Tp>();
		}

		auto sz_holder = lua_to(L, index, static_cast<std::tuple<_Tp, _Tp>*>(nullptr), is_valid);
		if (!is_valid) {
			return cv::Size_<_Tp>();
		}

		decltype(auto) sz = extract_holder(sz_holder, static_cast<std::tuple<_Tp, _Tp>*>(nullptr));
		return cv::Size_<_Tp>(std::get<0>(sz), std::get<1>(sz));
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
	inline cv::Vec<_Tp, cn>  lua_to(lua_State* L, int index, cv::Vec<_Tp, cn>*, bool& is_valid) {
		cv::Vec<_Tp, cn> out;

		auto vec_holder = lua_to(L, index, static_cast<std::vector<_Tp>*>(nullptr), is_valid, cn, true);
		if (!is_valid) {
			return out;
		}

		decltype(auto) vec = extract_holder(vec_holder, static_cast<std::vector<_Tp>*>(nullptr));

		const auto size = vec.size();

		for (int i = 0; i < size; i++) {
			out[i] = vec.at(i);
		}

		for (int i = size; i < cn; i++) {
			out[i] = static_cast<_Tp>(0);
		}

		return out;
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
	inline cv::util::variant<_Ts...> _lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr, bool& is_valid) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;
		using Variant = typename cv::util::variant<_Ts...>;

		if constexpr (!std::is_same_v<T, cv::util::monostate> && !std::is_same_v<T, cv::gapi::wip::draw::FText> && !std::is_same_v<T, cv::GFrameDesc>) {
			Variant out;
			auto holder = lua_to(L, index, static_cast<T*>(nullptr), is_valid);
			if (is_valid) {
				out = extract_holder(holder, static_cast<T*>(nullptr));
				return out;
			}
		}

		if constexpr (I == sizeof...(_Ts) - 1) {
			is_valid = false;
			return Variant();
		}
		else {
			return _lua_to<I + 1, _Ts...>(L, index, ptr, is_valid);
		}
	}

	template<typename... _Ts>
	inline cv::util::variant<_Ts...> lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr, bool& is_valid) {
		return _lua_to(L, index, ptr, is_valid);
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


	// ================================
	// cv::optional
	// ================================

	template<typename T>
	inline int lua_push(lua_State* L, const cv::optional<T>& opt) {
		if (!opt.has_value()) {
			lua_pushnil(L);
			return 1;
		}
		return lua_push(L, *opt);
	}
}

namespace LUA_MODULE_NAME {
	void register_extensions(lua_State* L);
}
