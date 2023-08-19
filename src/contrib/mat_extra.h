#pragma once
#include <lua_generated_include.hpp>
#include <luadef.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace cvextra {
	cv::Mat createMatFromVectorOfMat(const std::vector<cv::Mat>& vec, sol::state_view& lua);
	cv::Mat createMatFromArray(sol::table array, int depth, sol::state_view& lua);
    sol::table tolistMat(const cv::Mat& self, sol::state_view& lua);

	template<typename... Args>
	inline double mat_at(const cv::Mat& m, Args&&... args) {
		switch (m.depth()) {
		case CV_8U:
			return m.at<uchar>(std::forward<Args>(args)...);
		case CV_8S:
			return m.at<char>(std::forward<Args>(args)...);
		case CV_16U:
			return m.at<ushort>(std::forward<Args>(args)...);
		case CV_16S:
			return m.at<short>(std::forward<Args>(args)...);
		case CV_32S:
			return m.at<int>(std::forward<Args>(args)...);
		case CV_32F:
			return m.at<float>(std::forward<Args>(args)...);
		case CV_64F:
			return m.at<double>(std::forward<Args>(args)...);
		default:
			LUA_MODULE_THROW("Unsupported mat type");
		}
	}

	template<typename... Args>
	inline void mat_set_at(cv::Mat& m, double value, Args&&... args) {
		switch (m.depth()) {
		case CV_8U:
			m.at<uchar>(std::forward<Args>(args)...) = static_cast<uchar>(value);
			break;
		case CV_8S:
			m.at<char>(std::forward<Args>(args)...) = static_cast<char>(value);
			break;
		case CV_16U:
			m.at<ushort>(std::forward<Args>(args)...) = static_cast<ushort>(value);
			break;
		case CV_16S:
			m.at<short>(std::forward<Args>(args)...) = static_cast<short>(value);
			break;
		case CV_32S:
			m.at<int>(std::forward<Args>(args)...) = static_cast<int>(value);
			break;
		case CV_32F:
			m.at<float>(std::forward<Args>(args)...) = static_cast<float>(value);
			break;
		case CV_64F:
			m.at<double>(std::forward<Args>(args)...) = value;
			break;
		default:
			LUA_MODULE_THROW("Unsupported mat type");
		}
	}

	template<typename... Args>
	inline cv::Point2d mat_Point_at(const cv::Mat& m, Args&&... args) {
		using namespace cv;

		switch (m.depth()) {
		case CV_8U:
		{
			auto& pt = m.at<Vec2b>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_8S:
		{
			auto& pt = m.at<Vec<char, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_16U:
		{
			auto& pt = m.at<Vec2w>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_16S:
		{
			auto& pt = m.at<Vec2s>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_32S:
		{
			auto& pt = m.at<Vec2i>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_32F:
		{
			auto& pt = m.at<Vec2f>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_64F:
		{
			auto& pt = m.at<Vec2d>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		default:
			LUA_MODULE_THROW("Unsupported mat type");
		}
	}
}
