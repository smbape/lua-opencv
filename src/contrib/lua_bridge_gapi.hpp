#pragma once

#ifdef HAVE_OPENCV_GAPI

#include <lua_bridge_gapi.hdr.hpp>
#include <lua_generated_include.hpp>
#include <register_all.hpp>
#include <lua_bridge_common.hpp>

namespace LUA_MODULE_NAME {

	// ================================
	// cv::util::variant
	// ================================

	template<std::size_t I = 0, typename... _Ts>
	inline cv::util::variant<_Ts...> _lua_to(lua_State* L, int index, cv::util::variant<_Ts...>* ptr, bool& is_valid) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;
		using Variant = typename cv::util::variant<_Ts...>;

		if constexpr (
			!std::is_same_v<T, cv::util::monostate>
#ifdef HAVE_OPENCV_GAPI
			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/gmat.hpp#L103
			&& !std::is_same_v<T, cv::GMatP>

			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/garray.hpp#L70
			&& !std::is_same_v<T, cv::detail::GArrayU>

			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/gopaque.hpp#L67
			&& !std::is_same_v<T, cv::detail::GOpaqueU>

			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/gframe.hpp#L96
			&& !std::is_same_v<T, cv::GFrameDesc>
#endif
		) {
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
	inline void _lua_push(lua_State* L, const cv::util::variant<_Ts...>& value) {
		using _Tuple = typename std::tuple<_Ts...>;
		using T = typename std::tuple_element<I, _Tuple>::type;
		using Arg = typename cv::util::variant<_Ts...>;

		if constexpr (
			!std::is_same_v<T, cv::util::monostate>
#ifdef HAVE_OPENCV_GAPI
			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/gmat.hpp#L103
			&& !std::is_same_v<T, cv::GMatP>

			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/garray.hpp#L70
			&& !std::is_same_v<T, cv::detail::GArrayU>

			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/gopaque.hpp#L67
			&& !std::is_same_v<T, cv::detail::GOpaqueU>

			// There is no python api : https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/gframe.hpp#L96
			&& !std::is_same_v<T, cv::GFrameDesc>
#endif
		) {
			if (value.index() == I) {
				lua_push(L, cv::util::get<T>(value));
				return;
			}
		}

		if constexpr (I + 1 != sizeof...(_Ts)) {
			_lua_push<I + 1, _Ts...>(L, value);
		}
		else {
			lua_pushnil(L);
		}
	}

	template<typename... _Ts>
	inline void lua_push(lua_State* L, const cv::util::variant<_Ts...>& value) {
		_lua_push(L, value);
	}


	// ================================
	// cv::optional
	// ================================

	template<typename T>
	inline void lua_push(lua_State* L, const cv::optional<T>& opt) {
		if (opt.has_value()) {
			lua_push(L, *opt);
		}
		else {
			lua_pushnil(L);
		}
	}

	// ================================


	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L149-L170
	inline void lua_push(lua_State* L, const cv::gapi::wip::draw::Prim& prim) {
		switch (prim.index()) {
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Rect>():
				lua_push(L, cv::util::get<cv::gapi::wip::draw::Rect>(prim));
				break;
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Text>():
				lua_push(L, cv::util::get<cv::gapi::wip::draw::Text>(prim));
				break;
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Circle>():
				lua_push(L, cv::util::get<cv::gapi::wip::draw::Circle>(prim));
				break;
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Line>():
				lua_push(L, cv::util::get<cv::gapi::wip::draw::Line>(prim));
				break;
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Poly>():
				lua_push(L, cv::util::get<cv::gapi::wip::draw::Poly>(prim));
				break;
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Mosaic>():
				lua_push(L, cv::util::get<cv::gapi::wip::draw::Mosaic>(prim));
				break;
			case cv::gapi::wip::draw::Prim::index_of<cv::gapi::wip::draw::Image>():
				lua_push(L, cv::util::get<cv::gapi::wip::draw::Image>(prim));
				break;
			default:
				luaL_error(L, "Unsupported draw primitive type");
		}

	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L179-L199
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

			luaL_error(L, "Unsupported primitive type");
		return prim;
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L208-L226
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

			luaL_error(L, "Unsupported cv::GMetaArg type");
		return arg;
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L236-L267
	inline void lua_push(lua_State* L, const cv::GArg& value) {
		GAPI_Assert(value.kind != cv::detail::ArgKind::GOBJREF);
#define HANDLE_CASE(T, O) case cv::detail::OpaqueKind::CV_##T:  \
		{                                                           \
			lua_push(L, value.get<O>()); break;                     \
		}

#define UNSUPPORTED(T) case cv::detail::OpaqueKind::CV_##T: break
		switch (value.opaque_kind)
		{
			HANDLE_CASE(BOOL, bool);
			HANDLE_CASE(INT, int);
			HANDLE_CASE(INT64, int64_t);
			HANDLE_CASE(UINT64, uint64_t);
			HANDLE_CASE(DOUBLE, double);
			HANDLE_CASE(FLOAT, float);
			HANDLE_CASE(STRING, std::string);
			HANDLE_CASE(POINT, cv::Point);
			HANDLE_CASE(POINT2F, cv::Point2f);
			HANDLE_CASE(POINT3F, cv::Point3f);
			HANDLE_CASE(SIZE, cv::Size);
			HANDLE_CASE(RECT, cv::Rect);
			HANDLE_CASE(SCALAR, cv::Scalar);
			HANDLE_CASE(MAT, cv::Mat);
			// HANDLE_CASE(UNKNOWN,   cv::detail::PyObjectHolder); // TODO
			HANDLE_CASE(DRAW_PRIM, cv::gapi::wip::draw::Prim);
#undef HANDLE_CASE
#undef UNSUPPORTED
		default:
			luaL_error(L, "Unsupported kernel input type");
		}
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L270-L274
	inline std::shared_ptr<cv::GArg> lua_to(lua_State* L, int index, cv::GArg*, bool& is_valid) {
		is_valid = false;
		// value = cv::GArg(cv::detail::PyObjectHolder(obj));
		luaL_error(L, "Setting a cv::GArg is not yet supported");
		return std::shared_ptr<cv::GArg>();
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L301-L326
	inline void lua_push(lua_State* L, const cv::detail::OpaqueRef& o) {
		switch (o.getKind()) {
			case cv::detail::OpaqueKind::CV_BOOL: lua_push(L, o.rref<bool>()); break;
			case cv::detail::OpaqueKind::CV_INT: lua_push(L, o.rref<int>()); break;
			case cv::detail::OpaqueKind::CV_INT64: lua_push(L, o.rref<int64_t>()); break;
			case cv::detail::OpaqueKind::CV_UINT64: lua_push(L, o.rref<uint64_t>()); break;
			case cv::detail::OpaqueKind::CV_DOUBLE: lua_push(L, o.rref<double>()); break;
			case cv::detail::OpaqueKind::CV_FLOAT: lua_push(L, o.rref<float>()); break;
			case cv::detail::OpaqueKind::CV_STRING: lua_push(L, o.rref<std::string>()); break;
			case cv::detail::OpaqueKind::CV_POINT: lua_push(L, o.rref<cv::Point>()); break;
			case cv::detail::OpaqueKind::CV_POINT2F: lua_push(L, o.rref<cv::Point2f>()); break;
			case cv::detail::OpaqueKind::CV_POINT3F: lua_push(L, o.rref<cv::Point3f>()); break;
			case cv::detail::OpaqueKind::CV_SIZE: lua_push(L, o.rref<cv::Size>()); break;
			case cv::detail::OpaqueKind::CV_RECT: lua_push(L, o.rref<cv::Rect>()); break;
			case cv::detail::OpaqueKind::CV_UNKNOWN: lua_push(L, o.rref<cv::GArg>()); break;
			case cv::detail::OpaqueKind::CV_DRAW_PRIM: lua_push(L, o.rref<cv::gapi::wip::draw::Prim>()); break;
			case cv::detail::OpaqueKind::CV_SCALAR: break;
			case cv::detail::OpaqueKind::CV_MAT: break;
			default: luaL_error(L, "Unsupported GOpaque type");;
		}
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L328-L352
	inline void lua_push(lua_State* L, const cv::detail::VectorRef& v) {
		switch (v.getKind()) {
			case cv::detail::OpaqueKind::CV_BOOL: lua_push(L, v.rref<bool>()); break;
			case cv::detail::OpaqueKind::CV_INT: lua_push(L, v.rref<int>()); break;
			case cv::detail::OpaqueKind::CV_INT64: lua_push(L, v.rref<int64_t>()); break;
			case cv::detail::OpaqueKind::CV_UINT64: lua_push(L, v.rref<uint64_t>()); break;
			case cv::detail::OpaqueKind::CV_DOUBLE: lua_push(L, v.rref<double>()); break;
			case cv::detail::OpaqueKind::CV_FLOAT: lua_push(L, v.rref<float>()); break;
			case cv::detail::OpaqueKind::CV_STRING: lua_push(L, v.rref<std::string>()); break;
			case cv::detail::OpaqueKind::CV_POINT: lua_push(L, v.rref<cv::Point>()); break;
			case cv::detail::OpaqueKind::CV_POINT2F: lua_push(L, v.rref<cv::Point2f>()); break;
			case cv::detail::OpaqueKind::CV_POINT3F: lua_push(L, v.rref<cv::Point3f>()); break;
			case cv::detail::OpaqueKind::CV_SIZE: lua_push(L, v.rref<cv::Size>()); break;
			case cv::detail::OpaqueKind::CV_RECT: lua_push(L, v.rref<cv::Rect>()); break;
			case cv::detail::OpaqueKind::CV_SCALAR: lua_push(L, v.rref<cv::Scalar>()); break;
			case cv::detail::OpaqueKind::CV_MAT: lua_push(L, v.rref<cv::Mat>()); break;
			case cv::detail::OpaqueKind::CV_UNKNOWN: lua_push(L, v.rref<cv::GArg>()); break;
			case cv::detail::OpaqueKind::CV_DRAW_PRIM: lua_push(L, v.rref<cv::gapi::wip::draw::Prim>()); break;
			default: luaL_error(L, "Unsupported GArray type");
		}
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L355-L374
	inline void lua_push(lua_State* L, const cv::GRunArg& v) {
		switch (v.index()) {
			case cv::GRunArg::index_of<cv::Mat>():
				lua_push(L, cv::util::get<cv::Mat>(v));
				break;

			case cv::GRunArg::index_of<cv::Scalar>():
				lua_push(L, cv::util::get<cv::Scalar>(v));
				break;

			case cv::GRunArg::index_of<cv::detail::VectorRef>():
				lua_push(L, cv::util::get<cv::detail::VectorRef>(v));
				break;

			case cv::GRunArg::index_of<cv::detail::OpaqueRef>():
				lua_push(L, cv::util::get<cv::detail::OpaqueRef>(v));
				break;

			default:
				luaL_error(L, "Failed to unpack GRunArgs. Index of variant is unknown");
		}
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/include/opencv2/gapi/garg.hpp#L121
	inline std::shared_ptr<cv::GRunArg> lua_to(lua_State* L, int index, cv::GRunArg*, bool& is_valid) {
		is_valid = false;
		// luaL_error(L, "Setting a cv::GRunArg is not allowed");
		return std::shared_ptr<cv::GRunArg>();
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L387-L406
	inline void lua_push(lua_State* L, const cv::GOptRunArg& v) {
		switch (v.index()) {
			case cv::GOptRunArg::index_of<cv::optional<cv::Mat>>():
				lua_push(L, cv::util::get<cv::optional<cv::Mat>>(v));
				break;

			case cv::GOptRunArg::index_of<cv::optional<cv::Scalar>>():
				lua_push(L, cv::util::get<cv::optional<cv::Scalar>>(v));
				break;

			case cv::GOptRunArg::index_of<cv::optional<cv::detail::VectorRef>>():
				lua_push(L, cv::util::get<cv::optional<cv::detail::VectorRef>>(v));
				break;

			case cv::GOptRunArg::index_of<cv::optional<cv::detail::OpaqueRef>>():
				lua_push(L, cv::util::get<cv::optional<cv::detail::OpaqueRef>>(v));
				break;

			default:
				luaL_error(L, "Failed to unpack GOptRunArg. Index of variant is unknown");
		}

	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L493-L524
	inline cv::detail::OpaqueRef extract_opaque_ref(lua_State* L, int index, cv::detail::OpaqueKind kind, int i)
	{
	#define HANDLE_CASE(T, O) case cv::detail::OpaqueKind::CV_##T:  \
	{                                                               \
		bool is_valid;                                                                                  \
		auto holder_value = lua_to(L, index, static_cast<O*>(nullptr), is_valid);                       \
		if (is_valid) {                                                                                 \
			decltype(auto) value = extract_holder(holder_value, static_cast<O*>(nullptr));              \
			return cv::detail::OpaqueRef{value};                                                        \
		}                                                                                               \
		luaL_error(L, "index %d: Expecting " #O, i);                                                    \
	}
	#define UNSUPPORTED(T) case cv::detail::OpaqueKind::CV_##T: break
		switch (kind)
		{
			HANDLE_CASE(BOOL,    bool);
			HANDLE_CASE(INT,     int);
			HANDLE_CASE(INT64,   int64_t);
			HANDLE_CASE(UINT64,  uint64_t);
			HANDLE_CASE(DOUBLE,  double);
			HANDLE_CASE(FLOAT,   float);
			HANDLE_CASE(STRING,  std::string);
			HANDLE_CASE(POINT,   cv::Point);
			HANDLE_CASE(POINT2F, cv::Point2f);
			HANDLE_CASE(POINT3F, cv::Point3f);
			HANDLE_CASE(SIZE,    cv::Size);
			HANDLE_CASE(RECT,    cv::Rect);
			HANDLE_CASE(UNKNOWN, cv::GArg);
			UNSUPPORTED(SCALAR);
			UNSUPPORTED(MAT);
			UNSUPPORTED(DRAW_PRIM);
	#undef HANDLE_CASE
	#undef UNSUPPORTED
		}
		luaL_error(L, "index %d: Unsupported type for GOpaqueT", i);
		cv::util::throw_error(std::logic_error("Unsupported type for GOpaqueT"));
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L526-L557
	inline cv::detail::VectorRef extract_vector_ref(lua_State* L, int index, cv::detail::OpaqueKind kind, int i)
	{
	#define HANDLE_CASE(T, O) case cv::detail::OpaqueKind::CV_##T:                                      \
	{                                                                                                   \
		bool is_valid;                                                                                  \
		auto holder_value = lua_to(L, index, static_cast<std::vector<O>*>(nullptr), is_valid);          \
		if (is_valid) {                                                                                 \
			decltype(auto) value = extract_holder(holder_value, static_cast<std::vector<O>*>(nullptr)); \
			return cv::detail::VectorRef{value};                                                        \
		}                                                                                               \
		luaL_error(L, "index %d: Expecting std::vector<" #O ">", i);                                    \
	}
	#define UNSUPPORTED(T) case cv::detail::OpaqueKind::CV_##T: break
		switch (kind)
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
			HANDLE_CASE(UNKNOWN,   cv::GArg);
			HANDLE_CASE(DRAW_PRIM, cv::gapi::wip::draw::Prim);
	#undef HANDLE_CASE
	#undef UNSUPPORTED
		}
		luaL_error(L, "index %d: Unsupported type for GArrayT", i);
		cv::util::throw_error(std::logic_error("Unsupported type for GArrayT"));
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L559-L599
	inline cv::GRunArg extract_run_arg(lua_State* L, const cv::GTypeInfo& info, int index, int i) {
		switch (info.shape) {
			case cv::GShape::GMAT:
			{
				// NB: In case streaming it can be IStreamSource or cv::Mat
				{
					bool is_valid;
					auto holder_value = lua_to(L, index, static_cast<cv::gapi::wip::IStreamSource*>(nullptr), is_valid);
					if (is_valid) {
						return holder_value;
					}
				}

				{
					bool is_valid;
					auto holder_value = lua_to(L, index, static_cast<cv::Mat*>(nullptr), is_valid);
					if (is_valid) {
						decltype(auto) value = extract_holder(holder_value, static_cast<cv::Mat*>(nullptr));
						return value;
					}

				}

				luaL_error(L, "index %d: Expecting IStreamSource or cv::Mat", i);
			}
			case cv::GShape::GSCALAR:
			{
				bool is_valid;
				auto holder_value = lua_to(L, index, static_cast<cv::Scalar*>(nullptr), is_valid);
				if (is_valid) {
					decltype(auto) value = extract_holder(holder_value, static_cast<cv::Scalar*>(nullptr));
					return value;
				}
				luaL_error(L, "index %d: Expecting cv::Scalar", i);
			}
			case cv::GShape::GOPAQUE:
			{
				return extract_opaque_ref(L, index, info.kind, i);
			}
			case cv::GShape::GARRAY:
			{
				return extract_vector_ref(L, index, info.kind, i);
			}
			case cv::GShape::GFRAME:
			{
				// NB: Isn't supported yet.
				break;
			}
		}

		luaL_error(L, "index %d: Unsupported output shape", i);
		cv::util::throw_error(std::logic_error("Unsupported output shape"));
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L601-L615
	inline cv::GRunArgs extract_run_args(lua_State* L, const cv::GTypesInfo& info, const Object& lua_args) {
		lua_push(L, lua_args);
		auto index = lua_gettop(L);

		cv::GRunArgs args;
		auto size = lua_rawlen(L, index);
		args.reserve(size);

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			args.push_back(extract_run_arg(L, info[i], -1, i));
			lua_pop(L, 1);
		}

		lua_pop(L, 1);

		return args;
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L617-L648
	inline cv::GMetaArg extract_meta_arg(lua_State* L, const cv::GTypeInfo& info, int index, int i) {
		switch (info.shape)
		{
			case cv::GShape::GMAT:
			{
				bool is_valid;
				auto holder_value = lua_to(L, index, static_cast<cv::Mat*>(nullptr), is_valid);
				if (is_valid) {
					decltype(auto) value = extract_holder(holder_value, static_cast<cv::Mat*>(nullptr));
					return cv::GMetaArg{cv::descr_of(value)};
				}
				luaL_error(L, "index %d: Expecting cv::Mat", i);
			}
			case cv::GShape::GSCALAR:
			{
				bool is_valid;
				auto holder_value = lua_to(L, index, static_cast<cv::Scalar*>(nullptr), is_valid);
				if (is_valid) {
					decltype(auto) value = extract_holder(holder_value, static_cast<cv::Scalar*>(nullptr));
					return cv::GMetaArg{cv::descr_of(value)};
				}
				luaL_error(L, "index %d: Expecting cv::Scalar", i);
			}
			case cv::GShape::GARRAY:
			{
				return cv::GMetaArg{cv::empty_array_desc()};
			}
			case cv::GShape::GOPAQUE:
			{
				return cv::GMetaArg{cv::empty_gopaque_desc()};
			}
			case cv::GShape::GFRAME:
			{
				// NB: Isn't supported yet.
				break;
			}
		}
		luaL_error(L, "index %d: Unsupported output shape", i);
		cv::util::throw_error(std::logic_error("Unsupported output shape"));
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L650-L664
	inline cv::GMetaArgs extract_meta_args(lua_State* L, const cv::GTypesInfo& info, const Object& lua_args) {
		lua_push(L, lua_args);
		auto index = lua_gettop(L);

		cv::GMetaArgs metas;
		auto size = lua_rawlen(L, index);
		metas.reserve(size);

		for (auto i = 1; i <= size; ++i) {
			lua_pushnumber(L, i);
			lua_rawget(L, index);
			metas.push_back(extract_meta_arg(L, info[i], -1, i));
			lua_pop(L, 1);
		}

		lua_pop(L, 1);

		return metas;
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L1033-L1056
	inline cv::detail::ExtractArgsCallback lua_to(lua_State* L, int index, cv::detail::ExtractArgsCallback*, bool& is_valid) {
		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return {};
		}

		Object holder(L, index, is_valid);

		auto creator_gil_mutex = get_gil_mutex(L);
		auto creator_thread_id = std::this_thread::get_id();
		auto creator_gil = get_thread_gil(L, creator_gil_mutex);

		return { [=](const cv::GTypesInfo& info) {
			GilLock lock(L, creator_gil_mutex, creator_thread_id, creator_gil);
			return extract_run_args(L, info, holder);
		} };
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L1058-L1081
	inline cv::detail::ExtractMetaCallback lua_to(lua_State* L, int index, cv::detail::ExtractMetaCallback*, bool& is_valid) {
		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return {};
		}

		Object holder(L, index, is_valid);

		auto creator_gil_mutex = get_gil_mutex(L);
		auto creator_thread_id = std::this_thread::get_id();
		auto creator_gil = get_thread_gil(L, creator_gil_mutex);

		return { [=](const cv::GTypesInfo& info) {
			GilLock lock(L, creator_gil_mutex, creator_thread_id, creator_gil);
			return extract_meta_args(L, info, holder);
		} };
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L457-L491
	template <typename T>
	inline std::shared_ptr<T> extract_proto_args(lua_State* L, int index, bool& is_valid) {
		auto args_holder = lua_to(L, index, static_cast<cv::GProtoArgs*>(nullptr), is_valid);
		if (!is_valid) {
			return std::shared_ptr<T>();
		}

		cv::GProtoArgs args = extract_holder(args_holder, static_cast<cv::GProtoArgs*>(nullptr));
		return std::make_shared<T>(std::move(args));
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L1135-L1148
	inline std::shared_ptr<cv::GProtoInputArgs> lua_to(lua_State* L, int index, cv::GProtoInputArgs*, bool& is_valid) {
		return extract_proto_args<cv::GProtoInputArgs>(L, index, is_valid);
	}

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L1150-L1163
	inline std::shared_ptr<cv::GProtoOutputArgs> lua_to(lua_State* L, int index, cv::GProtoOutputArgs*, bool& is_valid) {
		return extract_proto_args<cv::GProtoOutputArgs>(L, index, is_valid);
	}

}

namespace cv::gapi::wip {
	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L1169-L1299

	/**
	 * @class LuaCustomStreamSource
	 * @brief Wraps a Lua-defined frame source as an IStreamSource for G-API.
	 *
	 * This class allows a G-API pipeline to pull frames from a Lua object. The Lua object
	 * must implement at a `pull()` method which must return a `numpy.ndarray` containing
	 * the next frame, or `None` to signal end-of-stream. It can also implement a `descr_of()`
	 * method which must return a `numpy.ndarray` that describes the format
	 * (data type, number of channels, height, width) of the frames produced.
	 */

	class LuaCustomStreamSource : public IStreamSource
	{
	public:
		LuaCustomStreamSource(lua_State* L, int index, bool& is_valid) {
			using namespace LUA_MODULE_NAME;

			is_valid = !lua_isnil(L, index);
			if (!is_valid) {
				return;
			}

			auto _obj = lua_to(L, index, static_cast<Object*>(nullptr), is_valid);
			if (is_valid) {
				obj.assign(L, _obj);
				creator_gil_mutex = get_gil_mutex(L);
				creator_thread_id = std::this_thread::get_id();
				creator_gil = get_thread_gil(L, creator_gil_mutex);
			}
		}

		bool pull(cv::gapi::wip::Data& data) CV_OVERRIDE
		{
			using namespace LUA_MODULE_NAME;

			lua_State* L = obj.L;
			GilLock lock(L, creator_gil_mutex, creator_thread_id, creator_gil);

			auto top = lua_gettop(L);

			lua_push(L, obj);
			lua_pushliteral(L, "pull");
			lua_gettable(L, -2);

			if (!lua_isfunction(L, -1)) {
				luaL_error(L, "LuaCustomStreamSource::pull(): .pull is not a function");
			}

			lua_push(L, obj);
			lua_call(L, 1, 1);

			bool is_valid;
			auto holder_value = lua_to(L, -1, static_cast<cv::Mat*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_error(L, "LuaCustomStreamSource::pull(): .pull() did not return a cv::Mat");
			}

			decltype(auto) value = extract_holder(holder_value, static_cast<cv::Mat*>(nullptr));
			data = value;

			lua_settop(L, top);
			return true;
		}

		GMetaArg descr_of() const CV_OVERRIDE
		{
			using namespace LUA_MODULE_NAME;

			lua_State* L = obj.L;
			GilLock lock(L, creator_gil_mutex, creator_thread_id, creator_gil);

			if (obj.isnil()) {
				return cv::GMetaArg(cv::GFrameDesc{ cv::MediaFormat::BGR, cv::Size(640, 480) });
			}

			auto top = lua_gettop(L);

			lua_push(L, obj);
			lua_pushliteral(L, "descr_of");
			lua_gettable(L, -2);

			if (!lua_isfunction(L, -1)) {
				luaL_error(L, "LuaCustomStreamSource::pull(): .descr_of is not a function");
			}

			lua_push(L, obj);
			lua_call(L, 1, 1);

			bool is_valid;
			auto holder_value = lua_to(L, -1, static_cast<cv::Mat*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_error(L, "LuaCustomStreamSource::descr_of(): .descr_of() did not return a cv::Mat");
			}

			decltype(auto) value = extract_holder(holder_value, static_cast<cv::Mat*>(nullptr));
			cv::GMatDesc mdesc = cv::descr_of(value);

			return cv::GMetaArg(mdesc);
		}

	private:
		LUA_MODULE_NAME::Object obj;
		std::shared_ptr<std::mutex> creator_gil_mutex;
		std::thread::id creator_thread_id;
		std::shared_ptr<std::unique_lock<std::mutex>> creator_gil;
	};

}

namespace LUA_MODULE_NAME {

	// https://github.com/opencv/opencv_contrib/blob/5.0.0/modules/gapi/misc/python/pyopencv_gapi.hpp#L1311-L1318
	inline std::shared_ptr<cv::gapi::wip::IStreamSource> lua_to(lua_State* L, int index, cv::gapi::wip::IStreamSource*, bool& is_valid) {
		return std::make_shared<cv::gapi::wip::LuaCustomStreamSource>(L, index, is_valid);
	}
}

#endif
