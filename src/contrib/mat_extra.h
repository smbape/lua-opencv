#pragma once
#include <lua_bridge.hdr.hpp>

// CV_EXPORTS_W : include this file in lua_generated_include

namespace cvextra {
	std::vector<int> mat_shape(const cv::Mat& self);
	std::vector<int> umat_shape(const cv::UMat& self);

	int Mat_index_at(lua_State* L, cv::Mat& self, int idx);
	int Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx);
	int Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges);
	int Mat_index_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx);

	void Mat_newindex_at(lua_State* L, cv::Mat& self, int idx, const cv::Mat& value);
	void Mat_newindex_at(lua_State* L, cv::Mat& self, int idx, const double value);
	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx, const cv::Mat& value);
	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<int>& idx, const double value);
	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges, const cv::Mat& value);
	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<cv::Range>& ranges, const double value);
	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx, const cv::Mat& value);
	void Mat_newindex_at(lua_State* L, cv::Mat& self, const std::vector<std::variant<int, cv::Range>>& idx, const double value);

	cv::Mat createMatFromVectorOfMat(lua_State* L, const std::vector<cv::Mat>& vec);
	cv::Mat createMatFromArray(lua_State* L, const LUA_MODULE_NAME::Table& array, int type);
	cv::UMat createUMatFromArray(lua_State* L, const LUA_MODULE_NAME::Table& array, int type, cv::UMatUsageFlags usageFlags);
	cv::cuda::GpuMat createGpuMatFromArray(lua_State* L, const LUA_MODULE_NAME::Table& array, int type, cv::cuda::GpuMat::Allocator* allocator);
	int pushtable_Mat(lua_State* L, const cv::Mat& self, bool nested);
	int pushtable_UMat(lua_State* L, const cv::UMat& self, bool nested);
	int pushtable_GpuMat(lua_State* L, const cv::cuda::GpuMat& self, bool nested);

	extern cv::Range Ellipsis;

	template<typename... Args>
	inline double mat_at(const cv::Mat& m, Args&&... args) {
		switch (m.depth()) {
		case CV_8U:
			return m.at<uint8_t>(std::forward<Args>(args)...);
		case CV_8S:
			return m.at<int8_t>(std::forward<Args>(args)...);
		case CV_16U:
			return m.at<uint16_t>(std::forward<Args>(args)...);
		case CV_16S:
			return m.at<int16_t>(std::forward<Args>(args)...);
		case CV_16F:
			return m.at<cv::hfloat>(std::forward<Args>(args)...);
		case CV_16BF:
			return m.at<cv::bfloat>(std::forward<Args>(args)...);
		case CV_32U:
			return m.at<uint32_t>(std::forward<Args>(args)...);
		case CV_32S:
			return m.at<int32_t>(std::forward<Args>(args)...);
		case CV_32F:
			return m.at<float>(std::forward<Args>(args)...);
		case CV_64U:
			return m.at<uint64_t>(std::forward<Args>(args)...);
		case CV_64S:
			return m.at<int64_t>(std::forward<Args>(args)...);
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
			m.at<uint8_t>(std::forward<Args>(args)...) = static_cast<uint8_t>(value);
			break;
		case CV_8S:
			m.at<int8_t>(std::forward<Args>(args)...) = static_cast<int8_t>(value);
			break;
		case CV_16U:
			m.at<uint16_t>(std::forward<Args>(args)...) = static_cast<uint16_t>(value);
			break;
		case CV_16S:
			m.at<int16_t>(std::forward<Args>(args)...) = static_cast<int16_t>(value);
			break;
		case CV_16F:
			m.at<cv::hfloat>(std::forward<Args>(args)...) = static_cast<cv::hfloat>(value);
			break;
		case CV_16BF:
			m.at<cv::bfloat>(std::forward<Args>(args)...) = static_cast<cv::bfloat>(value);
			break;
		case CV_32U:
			m.at<uint32_t>(std::forward<Args>(args)...) = static_cast<uint32_t>(value);
			break;
		case CV_32S:
			m.at<int32_t>(std::forward<Args>(args)...) = static_cast<int32_t>(value);
			break;
		case CV_32F:
			m.at<float>(std::forward<Args>(args)...) = static_cast<float>(value);
			break;
		case CV_64U:
			m.at<uint64_t>(std::forward<Args>(args)...) = static_cast<uint64_t>(value);
			break;
		case CV_64S:
			m.at<int64_t>(std::forward<Args>(args)...) = static_cast<int64_t>(value);
			break;
		case CV_64F:
			m.at<double>(std::forward<Args>(args)...) = static_cast<double>(value);
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
			auto& pt = m.at<Vec<uint8_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_8S:
		{
			auto& pt = m.at<Vec<int8_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_16U:
		{
			auto& pt = m.at<Vec<uint16_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_16S:
		{
			auto& pt = m.at<Vec<int16_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_16F:
		{
			auto& pt = m.at<Vec<cv::hfloat, 2>>(std::forward<Args>(args)...);
			return Point2d(static_cast<float>(pt[0]), static_cast<float>(pt[1]));
		}
		case CV_16BF:
		{
			auto& pt = m.at<Vec<cv::bfloat, 2>>(std::forward<Args>(args)...);
			return Point2d(static_cast<float>(pt[0]), static_cast<float>(pt[1]));
		}
		case CV_32U:
		{
			auto& pt = m.at<Vec<uint32_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_32S:
		{
			auto& pt = m.at<Vec<int32_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_32F:
		{
			auto& pt = m.at<Vec<float, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_64U:
		{
			auto& pt = m.at<Vec<uint64_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_64S:
		{
			auto& pt = m.at<Vec<int64_t, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		case CV_64F:
		{
			auto& pt = m.at<Vec<double, 2>>(std::forward<Args>(args)...);
			return Point2d(pt[0], pt[1]);
		}
		default:
			LUA_MODULE_THROW("Unsupported mat type");
		}
	}
}
