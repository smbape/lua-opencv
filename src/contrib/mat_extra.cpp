#include "mat_extra.h"

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
}
