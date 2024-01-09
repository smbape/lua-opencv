#pragma once
#include <luadef.hpp>
#include <opencv2/core.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace cvextra {
	double randu();
	void randu(int rows, int cols, cv::InputOutputArray dst, int type);
	void randu(cv::Size size, cv::InputOutputArray dst, int type);
	void randu(std::vector<int> sizes, cv::InputOutputArray dst, int type);

	double randn();
	void randn(int rows, int cols, cv::InputOutputArray dst, int type);
	void randn(cv::Size size, cv::InputOutputArray dst, int type);
	void randn(std::vector<int> sizes, cv::InputOutputArray dst, int type);
}
