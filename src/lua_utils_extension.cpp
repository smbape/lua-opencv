#include <lua_generated_pch.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	auto mat_at(cv::Mat& self, int i, sol::this_state& ts) {
		sol::state_view lua(ts);

		switch (self.depth()) {
		case CV_8U:
			return sol::object(ts, sol::in_place, self.at<uchar>(i));
		case CV_8S:
			return sol::object(ts, sol::in_place, self.at<char>(i));
		case CV_16U:
			return sol::object(ts, sol::in_place, self.at<ushort>(i));
		case CV_16S:
			return sol::object(ts, sol::in_place, self.at<short>(i));
		case CV_32S:
			return sol::object(ts, sol::in_place, self.at<int>(i));
		case CV_32F:
			return sol::object(ts, sol::in_place, self.at<float>(i));
		case CV_64F:
			return sol::object(ts, sol::in_place, self.at<double>(i));
		default:
			luaL_error(lua.lua_state(), "Overload resolution failed");
			// LUA_MODULE_THROW("Overload resolution failed");
			return sol::object(sol::lua_nil);
		}
	}

	sol::object mat_index(cv::Mat& self, int i, sol::this_state ts) {
		const auto& size = self.size;
		const auto dims = size.dims();
		const auto channels = self.channels();

		if (dims == 1) {
			if (channels != 1) {
				// treated as dims == 2
				int newsz[] = { size[0], channels };
				auto row = self.reshape(1, 2, newsz);
				return mat_index(row, i, ts);
			}

			return mat_at(self, i, ts);
		}

		if (dims > 2) {
			auto row = self.row(i).reshape(channels, dims - 1, size.p + 1);
			return sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(row));
		}

		// dims == 2
		if (channels != 1) {
			// treated as dims = 3
			int newsz[] = { size[1], channels };
			auto row = self.row(i).reshape(1, 2, newsz);
			return sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(row));
		}

		// dims == 2, channels == 1

		if (size[0] == 1 || size[1] == 1) {
			return mat_at(self, i, ts);
		}

		auto row = self.row(i);
		return sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(row));
	}
}

namespace LUA_MODULE_NAME {
	void register_extension(sol::state_view& lua, sol::table& module) {
		// https://github.com/ThePhD/sol2/issues/1405
		sol::usertype<cv::Mat> mat_type = module["cv"]["Mat"];

		mat_type.set_function(sol::meta_function::index, &mat_index);
	}
}
