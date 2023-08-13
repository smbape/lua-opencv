#include <lua_generated_pch.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	auto mat_at(cv::Mat& self, int idx, sol::this_state& ts) {
		sol::state_view lua(ts);

		switch (self.depth()) {
		case CV_8U:
			return sol::object(ts, sol::in_place, self.at<uchar>(idx));
		case CV_8S:
			return sol::object(ts, sol::in_place, self.at<char>(idx));
		case CV_16U:
			return sol::object(ts, sol::in_place, self.at<ushort>(idx));
		case CV_16S:
			return sol::object(ts, sol::in_place, self.at<short>(idx));
		case CV_32S:
			return sol::object(ts, sol::in_place, self.at<int>(idx));
		case CV_32F:
			return sol::object(ts, sol::in_place, self.at<float>(idx));
		case CV_64F:
			return sol::object(ts, sol::in_place, self.at<double>(idx));
		default:
			luaL_error(lua.lua_state(), "Overload resolution failed");
			// LUA_MODULE_THROW("Overload resolution failed");
			return sol::object(sol::lua_nil);
		}
	}

	sol::object mat_index(cv::Mat& self, int idx, sol::this_state ts) {
		const auto& size = self.size;
		const auto dims = size.dims();
		const auto channels = self.channels();

		if (dims == 1) {
			if (channels != 1) {
				// treated as dims == 2
				int newsz[] = { size[0], channels };
				auto row = self.reshape(1, 2, newsz);
				return mat_index(row, idx, ts);
			}

			return mat_at(self, idx, ts);
		}

		if (dims > 2) {
			auto row = self.row(idx).reshape(channels, dims - 1, size.p + 1);
			return sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(row));
		}

		// dims == 2
		if (channels != 1) {
			// treated as dims = 3
			int newsz[] = { size[1], channels };
			auto row = self.row(idx).reshape(1, 2, newsz);
			return sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(row));
		}

		// dims == 2, channels == 1

		if (size[0] == 1 || size[1] == 1) {
			return mat_at(self, idx, ts);
		}

		auto row = self.row(idx);
		return sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(row));
	}

	void mat_set_at(cv::Mat& self, int idx, double value, sol::this_state ts) {
		sol::state_view lua(ts);

		const auto& size = self.size;
		const auto dims = size.dims();
		const auto channels = self.channels();

		const bool can_set = dims == 1 && channels == 1 || dims == 2 && (channels == 1 || size[0] == 1 || size[1] == 1);
		if (!can_set) {
			luaL_error(lua.lua_state(), "matrix must have dims == 1 && channels == 1 || dims == 2 && (channels == 1 || rows == 1 || cols == 1)");
			return;
		}

		switch (self.depth()) {
		case CV_8U:
			self.at<uchar>(idx) = static_cast<uchar>(value);
			break;
		case CV_8S:
			self.at<char>(idx) = static_cast<char>(value);
			break;
		case CV_16U:
			self.at<ushort>(idx) = static_cast<ushort>(value);
			break;
		case CV_16S:
			self.at<short>(idx) = static_cast<short>(value);
			break;
		case CV_32S:
			self.at<int>(idx) = static_cast<int>(value);
			break;
		case CV_32F:
			self.at<float>(idx) = static_cast<float>(value);
			break;
		case CV_64F:
			self.at<double>(idx) = static_cast<double>(value);
			break;
		default:
			luaL_error(lua.lua_state(), "Unsupported mat type %d", self.depth());
			// LUA_MODULE_THROW("Unsupported mat type " << self.depth());
		}
	}
}

namespace LUA_MODULE_NAME {
	void register_extension(sol::state_view& lua, sol::table& module) {
		// https://github.com/ThePhD/sol2/issues/1405
		sol::usertype<cv::Mat> mat_type = module["cv"]["Mat"];

		mat_type.set_function(sol::meta_function::index, &mat_index);
		mat_type.set_function(sol::meta_function::new_index, &mat_set_at);
	}
}
