#include <lua_utils.hpp>
#include <limits>
#include "mat_extra.h"

namespace {
	using namespace LUA_MODULE_NAME;
	using byte = unsigned char;

	template<typename _Tp>
	const std::string join(_Tp* elements, size_t size, const std::string& separator = ",") {
		std::ostringstream o;
		for (size_t i = 0; i < size; i++) {
			if (i != 0) {
				o << separator;
			}
			o << elements[i];
		}
		return o.str();
	}

	void setMinMax(const sol::object& value, std::vector<int>& index, double& minVal, double& maxVal, bool& has_float, bool& has_double) {
		if (object_is_impl(value, static_cast<int8_t*>(nullptr))) {
			auto value_t = (double)object_as_impl(value, static_cast<int8_t*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		if (object_is_impl(value, static_cast<uint8_t*>(nullptr))) {
			auto value_t = (double)object_as_impl(value, static_cast<uint8_t*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		if (object_is_impl(value, static_cast<int16_t*>(nullptr))) {
			auto value_t = (double)object_as_impl(value, static_cast<int16_t*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		if (object_is_impl(value, static_cast<uint16_t*>(nullptr))) {
			auto value_t = (double)object_as_impl(value, static_cast<uint16_t*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		if (object_is_impl(value, static_cast<int32_t*>(nullptr))) {
			auto value_t = (double)object_as_impl(value, static_cast<int32_t*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		if (object_is_impl(value, static_cast<uint32_t*>(nullptr))) {
			auto value_t = (double)object_as_impl(value, static_cast<uint32_t*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		if (object_is_impl(value, static_cast<float*>(nullptr))) {
			has_float = true;
			auto value_t = (double)object_as_impl(value, static_cast<float*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		if (object_is_impl(value, static_cast<double*>(nullptr))) {
			has_double = true;
			auto value_t = (double)object_as_impl(value, static_cast<double*>(nullptr));
			minVal = std::min(minVal, value_t);
			maxVal = std::max(maxVal, value_t);
			return;
		}

		LUA_MODULE_THROW("Unsupported value at index [" << join(index.data(), index.size(), "][") << "]");
	}

	int getDepth(
		const sol::table& array,
		std::vector<int>& sizes,
		std::vector<int>& index,
		double& minVal,
		double& maxVal,
		bool& has_float,
		bool& has_double,
		int dim = 0
	) {
		if (sizes.size() <= dim) {
			sizes.push_back(array.size());
			index.push_back(0);
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

			index[dim] = i;

			sol::object value = key_value_pair.second;
			if (value.is<sol::table>()) {
				getDepth(value.as<sol::table>(), sizes, index, minVal, maxVal, has_float, has_double, dim + 1);
			}
			else {
				setMinMax(value, index, minVal, maxVal, has_float, has_double);
			}
		}

		// reset index before processing another dimension
		index[dim] = 1;

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
}

namespace cvextra {
	cv::Mat createFromVectorOfMat(const std::vector<cv::Mat>& vec) {
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

	template<typename _Tp>
	const void _createFromArray(sol::table& array, int depth, std::vector<int>& sizes, cv::Mat& result) {
		uint64_t total = 1;
		auto dims = sizes.size();
		std::vector<int> steps(dims);
		std::vector<sol::table> tables(dims);
		std::vector<int> index(dims, 0);

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

			if (object_is_impl(value, static_cast<int8_t*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<int8_t*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}
			else if (object_is_impl(value, static_cast<uint8_t*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<uint8_t*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}
			else if (object_is_impl(value, static_cast<int16_t*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<int16_t*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}
			else if (object_is_impl(value, static_cast<uint16_t*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<uint16_t*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}
			else if (object_is_impl(value, static_cast<int32_t*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<int32_t*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}
			else if (object_is_impl(value, static_cast<uint32_t*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<uint32_t*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}
			else if (object_is_impl(value, static_cast<float*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<float*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}
			else if (object_is_impl(value, static_cast<double*>(nullptr))) {
				auto value_t = object_as_impl(value, static_cast<double*>(nullptr));
				data[i] = static_cast<_Tp>(value_t);
			}

			if (i + 1 < total) {
				j = dims - 1;

				index[j]++;
				for (; j > 0 && index[j] == sizes[j]; j--) {
					index[j] = 0;
					index[j - 1]++;
				}

				current = tables[j++];
				for (; j < dims; j++) {
					current = current[index[j - 1] + 1];
					tables[j] = current;
				}

				j = index[dims - 1];
			}
		}
	}

	cv::Mat createFromArray(sol::table array, int depth) {
		std::vector<int> sizes;
		std::vector<int> index;
		double minVal = 0;
		double maxVal = 0;
		bool has_float = false;
		bool has_double = false;
		auto idepth = getDepth(array, sizes, index, minVal, maxVal, has_float, has_double);

		if (depth == -1) {
			depth = idepth;
		}

		cv::Mat result;

		switch (depth) {
		case CV_8U:
			_createFromArray<byte>(array, depth, sizes, result);
			break;
		case CV_8S:
			_createFromArray<char>(array, depth, sizes, result);
			break;
		case CV_16U:
			_createFromArray<ushort>(array, depth, sizes, result);
			break;
		case CV_16S:
			_createFromArray<short>(array, depth, sizes, result);
			break;
		case CV_32S:
			_createFromArray<int>(array, depth, sizes, result);
			break;
		case CV_32F:
			_createFromArray<float>(array, depth, sizes, result);
			break;
		case CV_64F:
			_createFromArray<double>(array, depth, sizes, result);
			break;
		default:
			LUA_MODULE_THROW("depth must be one of CV_8U CV_8S CV_16U CV_16S CV_32S CV_32F CV_64F");
		}

		return result;
	}
}
