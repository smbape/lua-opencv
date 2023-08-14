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

		// mat_type.set_function("multiply", sol::overload([] (cv::Mat& src1, cv::Mat& src2) {
		// 	double alpha = 1.0;
		// 	cv::Mat src3;
		// 	double beta = 0.0;
		// 	cv::Mat dst;
		// 	cv::gemm(src1, src2, alpha, src3, beta, dst);
		// 	return dst;
		// }, [] (cv::Mat& self, double alpha) {
		// 	cv::Mat dst = self * alpha;
		// 	return dst;
		// }));

		mat_type.set_function("multiply", [] (cv::Mat& self, sol::this_state ts, sol::variadic_args vargs) {
			sol::state_view lua(ts);
            sol::variadic_results vres;

			bool has_arg0;
			auto arg0_mat = vargs.get<sol::optional<cv::Mat>>(0); has_arg0 = static_cast<bool>(arg0_mat);
			auto arg0_ptr_mat = has_arg0 ? sol::optional<std::shared_ptr<cv::Mat>>() : vargs.get<sol::optional<std::shared_ptr<cv::Mat>>>(0); has_arg0 = static_cast<bool>(arg0_ptr_mat);
			auto arg0_double = has_arg0 ? sol::optional<double>() : vargs.get<sol::optional<double>>(0); has_arg0 = static_cast<bool>(arg0_ptr_mat);

			if (arg0_mat) {
				auto& src2 = *arg0_mat;
				double alpha = 1.0;
				cv::Mat src3;
				double beta = 0.0;
				cv::Mat dst;
				cv::gemm(self, src2, alpha, src3, beta, dst);
				vres.push_back(sol::object(ts, sol::in_place, dst));
			} else if (arg0_ptr_mat) {
				auto& src2 = *(*arg0_ptr_mat);
				double alpha = 1.0;
				cv::Mat src3;
				double beta = 0.0;
				cv::Mat dst;
				cv::gemm(self, src2, alpha, src3, beta, dst);
				vres.push_back(sol::object(ts, sol::in_place, dst));
			} else if (arg0_double) {
				cv::Mat dst = self * (*arg0_double);
				vres.push_back(sol::object(ts, sol::in_place, dst));
			} else {
				luaL_error(lua.lua_state(), "Overload resolution failed");
			}

            return vres;
        });

		// module.set_function("ones", [] (int rows, int cols, int type) {
		// 	// return cv::Mat::ones(rows, cols, type);
		// 	return std::make_shared<cv::Mat>(std::move(cv::Mat::ones(rows, cols, type)));
		// });

		module.set_function("ones", [] (sol::this_state ts, sol::variadic_args vargs) {
			sol::state_view lua(ts);
            sol::variadic_results vres;

			auto arg0_int = vargs.get<sol::optional<int>>(0);
			auto arg1_int = vargs.get<sol::optional<int>>(1);
			auto arg2_int = vargs.get<sol::optional<int>>(2);

			if (arg0_int && arg1_int && arg2_int) {
				auto& rows = *arg0_int;
				auto& cols = *arg1_int;
				auto& type = *arg2_int;
				vres.push_back(sol::object(ts, sol::in_place, std::make_shared<cv::Mat>(std::move(cv::Mat::ones(rows, cols, type)))));
			} else {
				luaL_error(lua.lua_state(), "Overload resolution failed");
			}

            return vres;
		});
	}
}
