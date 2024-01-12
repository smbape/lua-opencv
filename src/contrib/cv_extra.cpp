#include "cv_extra.h"

namespace cv {
	double randu() {
		return cv::theRNG().gaussian(1.0);
	}

	void randu(int rows, int cols, cv::InputOutputArray dst, int type) {
		dst.create(rows, cols, type);
		cv::theRNG().fill(dst, cv::RNG::UNIFORM, 0, 1);
	}

	void randu(cv::Size size, cv::InputOutputArray dst, int type) {
		dst.create(size, type);
		cv::theRNG().fill(dst, cv::RNG::UNIFORM, 0, 1);
	}

	void randu(const std::vector<int>& sizes, cv::InputOutputArray dst, int type) {
		dst.create(sizes.size(), sizes.data(), type);
		cv::theRNG().fill(dst, cv::RNG::UNIFORM, 0, 1);
	}

	double randn() {
		return cv::theRNG().uniform(0.0, 1.0);
	}

	void randn(int rows, int cols, cv::InputOutputArray dst, int type) {
		dst.create(rows, cols, type);
		cv::theRNG().fill(dst, cv::RNG::NORMAL, 0, 1);
	}

	void randn(cv::Size size, cv::InputOutputArray dst, int type) {
		dst.create(size, type);
		cv::theRNG().fill(dst, cv::RNG::NORMAL, 0, 1);
	}

	void randn(const std::vector<int>& sizes, cv::InputOutputArray dst, int type) {
		dst.create(sizes.size(), sizes.data(), type);
		cv::theRNG().fill(dst, cv::RNG::NORMAL, 0, 1);
	}

	int argmax(InputArray _src, bool lastIndex) {
		const auto src = _src.getMat().reshape(_src.channels(), _src.total());
		Mat dst;
		cv::reduceArgMax(src, dst, 0, lastIndex);
		return dst.at<int>(0, 0);
	}

	std::variant<std::shared_ptr<Mat>, int> argmax(InputArray src, int axis, bool lastIndex) {
		Mat dst;
		cv::reduceArgMax(src, dst, axis, lastIndex);

		if (src.dims() <= 2) {
			return dst.at<int>(0, 0);
		}

		return std::make_shared<cv::Mat>(dst);
	}
}
