#include <lua_utils.hpp>
#include <limits>
#include <type_traits>
#include "mat_extra.h"

#if (LUA_VERSION_NUM == 501 && !defined(lua_rawlen))
#define lua_rawlen lua_objlen
#endif

namespace {
	using namespace LUA_MODULE_NAME;
	using byte = unsigned char;

	template<typename _Tp>
	inline const std::string join(_Tp* elements, size_t size, const std::string& separator = ",") {
		std::ostringstream o;
		for (size_t i = 0; i < size; i++) {
			if (i != 0) {
				o << separator;
			}
			o << elements[i];
		}
		return o.str();
	}

	void setMinMax(lua_Number value, std::vector<double>& values, std::vector<int>& indexes) {
		double value_t = value;
		values.push_back(value_t);
	}

	void traverse_matrix(
		lua_State* L,
		std::vector<double>& values,
		std::vector<int>& sizes,
		std::vector<int>& indexes,
		int dim = 0
	) {
		auto index = lua_gettop(L);
		auto size = lua_rawlen(L, index);

		if (sizes.size() <= dim) {
			sizes.push_back(size);
			indexes.push_back(0);
		}
		else if (sizes[dim] != size) {
			LUA_MODULE_THROW("all arrays must have the same size");
		}

		for (int i = 0; i < size; i++) {
			indexes[dim] = i;

			lua_rawgeti(L, index, i + 1);

			if (lua_type(L, -1) == LUA_TNUMBER) {
				setMinMax(lua_tonumber(L, -1), values, indexes);
			}
			else {
				traverse_matrix(L, values, sizes, indexes, dim + 1);
			}

			lua_pop(L, 1);
		}

		// reset index before processing another dimension
		indexes[dim] = 1;
	}

	template<typename _Tm>
	inline void _createFromArray(sol::table array, int type, sol::state_view& lua, _Tm& result) {
		std::vector<double> values;
		std::vector<int> sizes;
		std::vector<int> indexes;

		lua_State* L = lua.lua_state();
		sol::stack::push(L, array);
		traverse_matrix(L, values, sizes, indexes);
		lua_pop(L, 1);

		if (type == -1) {
			type = CV_64F;
		}

		auto flags = CV_MAT_TYPE(type);
		auto cn = CV_MAT_CN(flags);
		auto depth = CV_MAT_DEPTH(flags);

		cv::Mat data(sizes, CV_64F, reinterpret_cast<void*>(values.data()));

		data.convertTo(result, depth);

		if (cn != 1) {
			auto dims = sizes.size();
			if (cn == sizes.back()) {
				result = result.reshape(cn, dims - 1, result.size.p);
			}
			else {
				luaL_error(lua.lua_state(), "the given array has %i channels, while given type has %i channels", sizes.back(), cn);
			}
		}
	}

	template<typename _Tp, typename _Tm>
	inline void _table(const _Tm& _self, bool nested, sol::table& res, sol::state_view& lua) {
		const cv::Mat self = [&]{
			if constexpr (std::is_same_v<_Tm, cv::UMat>) {
				return _self.getMat(cv::ACCESS_READ);
			} else {
				return _self;
			}
		}();

		const auto channels = self.channels();
		const auto total = self.total() * channels;
		auto dims = self.dims;

		if (self.cols == 1) {
			dims--;
		}

		if (channels != 1) {
			dims++;
		}

		std::vector<int> sizes(dims);
		std::vector<size_t> steps(dims);
		std::vector<int> indexes(dims, 0);

		for (int i = 0; i < dims; i++) {
			auto j = i == 0 || self.cols != 1 ? i : i + 1;
			sizes[i] = j < self.dims ? self.size[j] : channels;
			steps[i] = j < self.dims ? self.step[j] : sizeof(_Tp);
		}

		std::vector<int> ssizes(dims, 1);
		for (int i = dims - 2; i >= 0; i--) {
			ssizes[i] = ssizes[i + 1] * sizes[i + 1];
		}

		const uchar* data = self.ptr();
		lua_State* L = lua.lua_state();

		sol::stack::push(L, res);

		for (uint64_t i = 0, j = 0; i < total; i++) {
			while (indexes[j] == sizes[j]) {
				if (nested) {
					lua_pop(L, 1);
				}
				indexes[j--] = 0;
				indexes[j]++;
			}

			if (j != dims - 1) {
				if (nested) {
					int index = 1 + indexes[j];
					lua_newtable(L);
					lua_rawseti(L, -2, index);

					// lua_rawseti will pop the table from the stack
					// added it back onto the stack
					// because the top of the stack is the table we are filling
					lua_rawgeti(L, -1, index);
				}

				i--;
				j++;
				continue;
			}

			size_t offset = 0;
			size_t index = i;
			size_t nexti = i;

			for (int k = 0; k < dims; k++) {
				index = nexti / ssizes[k];
				offset += index * steps[k];
				nexti %= ssizes[k];
			}

			if constexpr (LUA_VERSION_NUM > 502 && std::is_integral_v<_Tp>) {
				lua_pushinteger(L, *reinterpret_cast<const _Tp*>(data + offset));
			} else {
				lua_pushnumber(L, *reinterpret_cast<const _Tp*>(data + offset));
			}

			if (nested) {
				lua_rawseti(L, -2, 1 + indexes[j]);
			} else {
				lua_rawseti(L, -2, 1 + i);
			}

			indexes[j]++;
		}

		if (nested) {
			// for j = 0; j < dims - 1; j++ lua_newtable
			// +1 = sol::stack::push(L, res);
			// there were dims tables pushed onto the stack
			// remove them from the stack
			lua_pop(L, dims);
		} else {
			// remove sol::stack::push(L, res);
			lua_pop(L, 1);
		}
	}

	template<typename _Tm>
	inline sol::table _table(const _Tm& self, bool nested, sol::state_view& lua) {
		sol::table res = lua.create_table();

		switch (self.depth()) {
		case CV_8U:
			_table<byte, _Tm>(self, nested, res, lua);
			break;
		case CV_8S:
			_table<char, _Tm>(self, nested, res, lua);
			break;
		case CV_16U:
			_table<ushort, _Tm>(self, nested, res, lua);
			break;
		case CV_16S:
			_table<short, _Tm>(self, nested, res, lua);
			break;
		case CV_32S:
			_table<int, _Tm>(self, nested, res, lua);
			break;
		case CV_32F:
			_table<float, _Tm>(self, nested, res, lua);
			break;
		case CV_64F:
			_table<double, _Tm>(self, nested, res, lua);
			break;
		default:
			luaL_error(lua.lua_state(), "depth must be one of CV_8U CV_8S CV_16U CV_16S CV_32S CV_32F CV_64F");
		}

		return res;
	}
}

namespace cvextra {
	std::vector<int> mat_shape(const cv::Mat& self) {
		const auto dims = self.size.dims();
		std::vector<int> shape(self.size.p, self.size.p + dims);
		const auto channels = self.channels();
		if (channels != 1) {
			shape.push_back(channels);
		}
		return shape;
	}

	std::vector<int> umat_shape(const cv::UMat& self) {
		const auto dims = self.size.dims();
		std::vector<int> shape(self.size.p, self.size.p + dims);
		const auto channels = self.channels();
		if (channels != 1) {
			shape.push_back(channels);
		}
		return shape;
	}

	cv::Mat createMatFromVectorOfMat(const std::vector<cv::Mat>& vec, sol::state_view& lua) {
		using namespace cv;

		Mat result;

		if (vec.empty()) {
			return result;
		}

		std::vector<int> isizes = std::vector<int>(vec[0].size.p, vec[0].size.p + vec[0].dims);

		std::vector<int> sizes{ static_cast<int>(vec.size()) };
		sizes.insert(std::end(sizes), std::begin(isizes), std::end(isizes));

		auto dims = vec[0].dims;
		auto depth = vec[0].depth();
		auto cn = vec[0].channels();
		auto type = CV_MAKE_TYPE(depth, cn);

		result = Mat::zeros(sizes.size(), sizes.data(), type);
		uchar* data = result.ptr();

		size_t i = 0;
		for (const auto& mat : vec) {
			LUA_MODULE_ASSERT_THROW(mat.depth() == depth,
				"Mat at index " << i << " doesn't have the same depth as Mat at index 0");
			LUA_MODULE_ASSERT_THROW(mat.channels() == cn,
				"Mat at index " << i << " doesn't have the same number of channels as Mat at index 0");
			LUA_MODULE_ASSERT_THROW(mat.dims == dims,
				"Mat at index " << i << " doesn't have the same dims as Mat at index 0");
			LUA_MODULE_ASSERT_THROW(std::equal(isizes.begin(), isizes.end(), mat.size.p),
				"Mat at index " << i << " doesn't have the same size as Mat at index 0");

			mat.copyTo(Mat(isizes, type, data));

			data += result.step.p[0];
			i++;
		}

		return result;
	}

	cv::Mat createMatFromArray(sol::table array, int type, sol::state_view& lua) {
		cv::Mat _result;
		_createFromArray(array, type, lua, _result);
		return _result;
	}

	sol::table tableMat(const cv::Mat& self, bool nested, sol::state_view& lua) {
		return _table(self, nested, lua);
	}

	cv::UMat createUMatFromArray(sol::table array, int type, cv::UMatUsageFlags usageFlags, sol::state_view& lua) {
		cv::UMat _result(usageFlags);
		_createFromArray(array, type, lua, _result);
		return _result;
	}

	sol::table tableUMat(const cv::UMat& self, bool nested, sol::state_view& lua) {
		return _table(self, nested, lua);
	}
}
