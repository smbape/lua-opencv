#include <lua_generated_pch.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

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

			return sol::object(ts, sol::in_place, cvextra::mat_at(self, idx));
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
			return sol::object(ts, sol::in_place, cvextra::mat_at(self, idx));
		}

		auto row = self.row(idx);
		return sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(row));
	}
}

namespace LUA_MODULE_NAME {
	void register_extension(sol::state_view& lua, sol::table& module) {
		// https://github.com/ThePhD/sol2/issues/1405
		sol::usertype<cv::Mat> mat_type = module["cv"]["Mat"];

		mat_type.set_function(sol::meta_function::index, sol::overload(
			&mat_index,
			[](cv::Mat& self, sol::as_table_t<std::vector<int>> idx, sol::this_state ts) {
				if (idx.value().size() == self.dims) {
					return cvextra::mat_at(self, idx.value().data());
				}
				sol::state_view lua(ts);
				luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.value().size());
				return 0.;
			}
		));

		mat_type.set_function(sol::meta_function::new_index, sol::overload(
			[](cv::Mat& self, int idx, double value) {
				cvextra::mat_set_at(self, value, idx);
			},
			[](cv::Mat& self, sol::as_table_t<std::vector<int>> idx, double value, sol::this_state ts) {
				if (idx.value().size() == self.dims) {
					cvextra::mat_set_at(self, value, idx.value().data());
					return;
				}
				sol::state_view lua(ts);
				luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.value().size());
			}
		));
	}
}
