#include "cv_extra.h"

namespace cvextra {
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

	void randu(std::vector<int> sizes, cv::InputOutputArray dst, int type) {
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

	void randn(std::vector<int> sizes, cv::InputOutputArray dst, int type) {
		dst.create(sizes.size(), sizes.data(), type);
		cv::theRNG().fill(dst, cv::RNG::NORMAL, 0, 1);
	}
}
