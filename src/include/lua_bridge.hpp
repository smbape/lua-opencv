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
	inline bool lua_is(lua_State* L, int index, cv::dnn::DictValue*) {
		return lua_isnil(L, index)
			|| lua_is(L, index, static_cast<int64*>(nullptr))
			|| lua_is(L, index, static_cast<double*>(nullptr))
			|| lua_is(L, index, static_cast<std::string*>(nullptr));
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L7-L37
	inline cv::dnn::DictValue lua_to(lua_State* L, int index, cv::dnn::DictValue*) {
		if (lua_isnil(L, index)) {
			return cv::dnn::DictValue();
		}

		if (lua_is(L, index, static_cast<int64*>(nullptr))) {
			return cv::dnn::DictValue(lua_to(L, index, static_cast<int64*>(nullptr)));
		}

		if (lua_is(L, index, static_cast<double*>(nullptr))) {
			return cv::dnn::DictValue(lua_to(L, index, static_cast<double*>(nullptr)));
		}

		if (lua_is(L, index, static_cast<std::string*>(nullptr))) {
			return cv::dnn::DictValue(lua_to(L, index, static_cast<std::string*>(nullptr)));
		}

		luaL_error(L, "Unsupported value type");
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
	inline bool lua_is(lua_State* L, int index, cv::dnn::LayerParams*) {
		return lua_is(L, index, static_cast<std::map<std::string, cv::dnn::DictValue>*>(nullptr));
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L75-L88
	inline void lua_to(lua_State* L, int index, cv::dnn::LayerParams& lp) {
		using K = std::string;
		using V = cv::dnn::DictValue;
		using Map = cv::dnn::LayerParams;

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		// Push another reference to the table on top of the stack (so we know
		// where it is, and this function can work for negative, positive and
		// pseudo indices
		lua_pushvalue(L, index);
		// stack now contains: -1 => table

		lua_pushnil(L);
		// stack now contains: -1 => nil; -2 => table

		while (lua_next(L, -2)) {
			// stack now contains: -1 => value; -2 => key; -3 => table
			const auto key = lua_to(L, -2, static_cast<K*>(nullptr));
			auto value_holder = lua_to(L, -1, static_cast<V*>(nullptr));
			decltype(auto) value = extract_holder(value_holder, static_cast<V*>(nullptr));

			lp.set(key, value);

			// pop value
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => table
		}

		// stack now contains: -1 => table (when lua_next returns 0 it pops the key
		// but does not push anything.)
		// Pop table
		lua_pop(L, 1);
		// Stack is now the same as it was on entry to this function
	}

	inline cv::dnn::LayerParams lua_to(lua_State* L, int index, cv::dnn::LayerParams*) {
		cv::dnn::LayerParams lp;
		lua_to(L, index, lp);
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
	inline bool lua_is(lua_State* L, int index, cv::flann::IndexParams*) {
		return lua_is(L, index, static_cast<std::map<std::string, std::variant<bool, int, double>>*>(nullptr));
	}

	// https://github.com/opencv/opencv/blob/4.11.0/modules/dnn/misc/python/pyopencv_dnn.hpp#L18-L103
	inline void lua_to(lua_State* L, int index, cv::flann::IndexParams& p) {
		using K = std::string;
		using V = std::variant<bool, int, double>;
		using Map = cv::flann::IndexParams;

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		// Push another reference to the table on top of the stack (so we know
		// where it is, and this function can work for negative, positive and
		// pseudo indices
		lua_pushvalue(L, index);
		// stack now contains: -1 => table

		lua_pushnil(L);
		// stack now contains: -1 => nil; -2 => table

		while (lua_next(L, -2)) {
			// stack now contains: -1 => value; -2 => key; -3 => table
			const auto key = lua_to(L, -2, static_cast<K*>(nullptr));
			auto value_holder = lua_to(L, -1, static_cast<V*>(nullptr));
			decltype(auto) value = extract_holder(value_holder, static_cast<V*>(nullptr));

			if (std::holds_alternative<bool>(value)) {
				p.setBool(key, std::get<bool>(value));
			} else if (std::holds_alternative<int>(value)) {
				if (key == "algorithm") {
					p.setAlgorithm(std::get<int>(value));
				} else {
					p.setInt(key, std::get<int>(value));
				}
			} else if (std::holds_alternative<double>(value)) {
				if (key == "eps") {
					p.setFloat(key, static_cast<float>(std::get<double>(value)));
				} else {
					p.setDouble(key, std::get<double>(value));
				}
			}

			// pop value
			lua_pop(L, 1);
			// stack now contains: -1 => key; -2 => table
		}

		// stack now contains: -1 => table (when lua_next returns 0 it pops the key
		// but does not push anything.)
		// Pop table
		lua_pop(L, 1);
		// Stack is now the same as it was on entry to this function
	}

	inline std::shared_ptr<cv::flann::IndexParams> lua_to(lua_State* L, int index, cv::flann::IndexParams*) {
		auto p = std::make_shared<cv::flann::IndexParams>();
		lua_to(L, index, *p);
		return p;
	}

	inline bool lua_is(lua_State* L, int index, cv::flann::SearchParams*) {
		return lua_is(L, index, static_cast<cv::flann::IndexParams*>(nullptr));
	}

	inline void lua_to(lua_State* L, int index, cv::flann::SearchParams& value) {
		lua_to(L, index, static_cast<cv::flann::IndexParams&>(value));
	}

	inline std::shared_ptr<cv::flann::SearchParams> lua_to(lua_State* L, int index, cv::flann::SearchParams*) {
		auto value = std::make_shared<cv::flann::SearchParams>();
		lua_to(L, index, *value);
		return value;
	}
#endif

#ifdef HAVE_OPENCV_GAPI
	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L179-L199
	inline bool lua_is(lua_State* L, int index, cv::gapi::wip::draw::Prim*) {
		return lua_is(L, index, static_cast<std::variant<
			cv::gapi::wip::draw::Rect,
			cv::gapi::wip::draw::Text,
			cv::gapi::wip::draw::Circle,
			cv::gapi::wip::draw::Line,
			cv::gapi::wip::draw::Mosaic,
			cv::gapi::wip::draw::Image,
			cv::gapi::wip::draw::Poly
		>*>(nullptr));
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L179-L199
	inline cv::gapi::wip::draw::Prim lua_to(lua_State* L, int index, cv::gapi::wip::draw::Prim*) {
		cv::gapi::wip::draw::Prim prim;

#define TRY_EXTRACT(Primitive)                                                                           \
		if (lua_is(L, index, static_cast<cv::gapi::wip::draw::Primitive*>(nullptr))) {                   \
			auto value_holder = lua_to(L, index, static_cast<cv::gapi::wip::draw::Primitive*>(nullptr)); \
			prim = extract_holder(value_holder, static_cast<cv::gapi::wip::draw::Primitive*>(nullptr));  \
			return prim;                                                                                 \
		}                                                                                                \

		TRY_EXTRACT(Rect)
		TRY_EXTRACT(Text)
		TRY_EXTRACT(Circle)
		TRY_EXTRACT(Line)
		TRY_EXTRACT(Mosaic)
		TRY_EXTRACT(Image)
		TRY_EXTRACT(Poly)
#undef TRY_EXTRACT

		luaL_error(L, "Unsupported primitive type");
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
	inline bool lua_is(lua_State* L, int index, cv::GMetaArg*) {
		return lua_is(L, index, static_cast<std::variant<
			cv::GMatDesc,
			cv::GScalarDesc,
			cv::GArrayDesc,
			cv::GOpaqueDesc
		>*>(nullptr));
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L208-L226
	inline cv::GMetaArg lua_to(lua_State* L, int index, cv::GMetaArg*) {
		cv::GMetaArg arg;

#define TRY_EXTRACT(Meta)                                                      \
		if (lua_is(L, index, static_cast<Meta*>(nullptr))) {                   \
			auto value_holder = lua_to(L, index, static_cast<Meta*>(nullptr)); \
			arg = extract_holder(value_holder, static_cast<Meta*>(nullptr));   \
			return arg;                                                        \
		}                                                                      \

		TRY_EXTRACT(cv::GMatDesc)
		TRY_EXTRACT(cv::GScalarDesc)
		TRY_EXTRACT(cv::GArrayDesc)
		TRY_EXTRACT(cv::GOpaqueDesc)
#undef TRY_EXTRACT

		luaL_error(L, "Unsupported cv::GMetaArg type");
		return arg;
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L270-L274
	inline bool lua_is(lua_State* L, int index, cv::GArg*) {
		LUA_MODULE_WARN("Setting a cv::GArg is not yet supported");
		return false;
	}

	// https://github.com/opencv/opencv/tree/4.11.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L270-L274
	inline std::shared_ptr<cv::GArg> lua_to(lua_State* L, int index, cv::GArg*) {
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
	inline cv::GRunArg lua_to(lua_State* L, int index, cv::GRunArg*) {
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

	inline int lua_push(lua_State* L, const cv::Range& range) {
		lua_newtable(L);
		int i = 0;
		lua_push(L, range.start); lua_rawseti(L, -2, ++i);
		lua_push(L, range.end); lua_rawseti(L, -2, ++i);
		return 1;
	}

	inline void lua_to(lua_State* L, int index, std::vector<cv::Range>& ranges) {
		if (lua_isuserdata(L, index)) {
			ranges = *lua_userdata_to(L, index, static_cast<std::vector<cv::Range>*>(nullptr));
			return;
		}

		if (index < 0) {
			index += lua_gettop(L) + 1;
		}

		auto size = lua_rawlen(L, index);
		ranges.resize(size);

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index); // push range table

			lua_pushnumber(L, 1);
			lua_rawget(L, -2); // push start
			ranges[i - 1].start = lua_to(L, -1, static_cast<int*>(nullptr));
			lua_pop(L, 1); // pop start

			lua_pushnumber(L, 2);
			lua_rawget(L, -2); // push end
			ranges[i - 1].end = lua_to(L, -1, static_cast<int*>(nullptr));
			lua_pop(L, 1); // pop end

			lua_pop(L, 1); // pop range table
		}
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
