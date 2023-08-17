#include <lua_utils.hpp>
#include <limits>
#include "mat_extra.h"

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

	void setMinMax(const sol::object& value, std::vector<int>& indexes, double& minVal, double& maxVal, bool& has_float, bool& has_double) {
		if (value.is<double>()) {
			has_double = true;
			auto value_t = value.as<double>();
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		LUA_MODULE_THROW("Unsupported value at index [" << join(indexes.data(), indexes.size(), "][") << "]");
	}

	int getDepth(
		const sol::table& array,
		std::vector<int>& sizes,
		std::vector<int>& indexes,
		double& minVal,
		double& maxVal,
		bool& has_float,
		bool& has_double,
		int dim = 0
	) {
		if (sizes.size() <= dim) {
			sizes.push_back(array.size());
			indexes.push_back(0);
		}
		else if (sizes[dim] != array.size()) {
			LUA_MODULE_THROW("all arrays must have the same size");
		}

		int depth = -1;

		auto i = 0;
		for (const auto& key_value_pair : array) {
			sol::object key = key_value_pair.first;
			if (!key.is<int>() || key.as<int>() != ++i) {
				LUA_MODULE_THROW("argument is not a table with only int keys");
			}

			indexes[dim] = i;

			sol::object value = key_value_pair.second;
			if (value.is<sol::table>()) {
				getDepth(value.as<sol::table>(), sizes, indexes, minVal, maxVal, has_float, has_double, dim + 1);
			}
			else {
				setMinMax(value, indexes, minVal, maxVal, has_float, has_double);
			}
		}

		// reset index before processing another dimension
		indexes[dim] = 1;

		if (has_double) {
			return CV_64F;
		}

		if (has_float) {
			return CV_32F;
		}

		if (minVal >= std::numeric_limits<int8_t>::min() && maxVal <= std::numeric_limits<int8_t>::max()) {
			return CV_8S;
		}

		if (minVal >= std::numeric_limits<uint8_t>::min() && maxVal <= std::numeric_limits<uint8_t>::max()) {
			return CV_8U;
		}

		if (minVal >= std::numeric_limits<int16_t>::min() && maxVal <= std::numeric_limits<int16_t>::max()) {
			return CV_16S;
		}

		if (minVal >= std::numeric_limits<int32_t>::min() && maxVal <= std::numeric_limits<int32_t>::max()) {
			return CV_32S;
		}

		return CV_32F;
	}

	template<typename _Tp>
	inline const void _createMatFromArray(sol::table& array, int depth, std::vector<int>& sizes, cv::Mat& result) {
		uint64_t total = 1;
		auto dims = sizes.size();
		std::vector<size_t> steps(dims);
		std::vector<sol::table> tables(dims);
		std::vector<int> indexes(dims, 0);

		sol::table current = array;

		for (int i = 0; i < dims; i++) {
			auto count = sizes[i];
			steps[i] = total;
			total *= count;

			tables[i] = current;
			if (i + 1 < dims) {
				current = current[1];
			}
		}

		result = cv::Mat::zeros(sizes.size(), sizes.data(), depth);
		_Tp* data = result.ptr<_Tp>();

		for (uint64_t i = 0, j = 0; i < total; i++) {
			sol::object value = current[j + 1];
			data[i] = static_cast<_Tp>(value.as<double>());

			if (i + 1 == total) {
				continue;
			}

			j = dims - 1;
			indexes[j]++;

			for (; j > 0 && indexes[j] == sizes[j]; j--) {
				indexes[j] = 0;
				indexes[j - 1]++;
			}

			current = tables[j++];
			for (; j < dims; j++) {
				current = current[indexes[j - 1] + 1];
				tables[j] = current;
			}

			j = indexes[dims - 1];
		}
	}

	template<typename _Tp>
	inline void _tolistMat(const cv::Mat& self, sol::table& res, sol::state_view& lua) {
		auto channels = self.channels();
		auto total = self.total() * channels;
		auto dims = self.dims;

		if (self.cols == 1) {
			dims--;
		}

		if (channels != 1) {
			dims++;
		}

		std::vector<sol::table> tables(dims);
		std::vector<int> sizes(dims);
		std::vector<size_t> steps(dims);
		std::vector<int> indexes(dims, 0);

		const uchar* data = self.ptr();

		auto current = res;
		for (int i = 0; i < dims; i++) {
			auto j = i == 0 || self.cols != 1 ? i : i + 1;

			tables[i] = current;
			sizes[i] = j < self.dims ? self.size[j] : channels;
			steps[i] = j < self.dims ? self.step[j] : sizeof(_Tp);

			if (i + 1 < dims) {
				current[1] = lua.create_table();
				current = current[1];
			}
		}

		std::vector<int> ssizes(dims, 1);
		for (int i = dims - 2; i >= 0; i--) {
			ssizes[i] = ssizes[i + 1] * sizes[i + 1];
		}

		for (uint64_t i = 0, j = 0; i < total; i++) {
			size_t offset = 0;
			size_t index = i;
			size_t nexti = i;

			for (int k = 0; k < dims; k++) {
				index = nexti / ssizes[k];
				offset += index * steps[k];
				nexti %= ssizes[k];
			}

			current[index + 1] = *reinterpret_cast<const _Tp*>(data + offset);

			if (i + 1 == total) {
				continue;
			}

			j = dims - 1;
			indexes[j]++;

			for (; j > 0 && indexes[j] == sizes[j]; j--) {
				indexes[j] = 0;
				indexes[j - 1]++;
			}

			current = tables[j++];
			for (; j < dims; j++) {
				auto k = indexes[j - 1] + 1;
				if (current[k] == sol::lua_nil) {
					current[k] = lua.create_table();
				}
				current = current[k];
				tables[j] = current;
			}

			j = indexes[dims - 1];
		}
	}
}

namespace cvextra {
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

	cv::Mat createMatFromArray(sol::table array, int depth, sol::state_view& lua) {
		std::vector<int> sizes;
		std::vector<int> indexes;
		double minVal = 0;
		double maxVal = 0;
		bool has_float = false;
		bool has_double = false;
		auto idepth = getDepth(array, sizes, indexes, minVal, maxVal, has_float, has_double);

		if (depth == -1) {
			depth = idepth;
		}

		cv::Mat result;

		switch (depth) {
		case CV_8U:
			_createMatFromArray<byte>(array, depth, sizes, result);
			break;
		case CV_8S:
			_createMatFromArray<char>(array, depth, sizes, result);
			break;
		case CV_16U:
			_createMatFromArray<ushort>(array, depth, sizes, result);
			break;
		case CV_16S:
			_createMatFromArray<short>(array, depth, sizes, result);
			break;
		case CV_32S:
			_createMatFromArray<int>(array, depth, sizes, result);
			break;
		case CV_32F:
			_createMatFromArray<float>(array, depth, sizes, result);
			break;
		case CV_64F:
			_createMatFromArray<double>(array, depth, sizes, result);
			break;
		default:
			luaL_error(lua.lua_state(), "depth must be one of CV_8U CV_8S CV_16U CV_16S CV_32S CV_32F CV_64F");
		}

		return result;
	}

	sol::table tolistMat(const cv::Mat& self, sol::state_view& lua) {
		sol::table res = lua.create_table();

		switch (self.depth()) {
		case CV_8U:
			_tolistMat<byte>(self, res, lua);
			break;
		case CV_8S:
			_tolistMat<char>(self, res, lua);
			break;
		case CV_16U:
			_tolistMat<ushort>(self, res, lua);
			break;
		case CV_16S:
			_tolistMat<short>(self, res, lua);
			break;
		case CV_32S:
			_tolistMat<int>(self, res, lua);
			break;
		case CV_32F:
			_tolistMat<float>(self, res, lua);
			break;
		case CV_64F:
			_tolistMat<double>(self, res, lua);
			break;
		default:
			luaL_error(lua.lua_state(), "depth must be one of CV_8U CV_8S CV_16U CV_16S CV_32S CV_32F CV_64F");
		}

		return res;
	}
}
