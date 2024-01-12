#include <lua_generated_pch.hpp>
#include <mutex>
#include <variant>

using MatIndexType = std::variant<std::shared_ptr<cv::Mat>, uchar, char, ushort, short, int, float, double>;

namespace {
	using namespace LUA_MODULE_NAME;

	template<typename... Args>
	inline MatIndexType mat_at(sol::this_state ts, const cv::Mat& m, Args&&... args) {
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

	MatIndexType mat_index(cv::Mat& self, int idx, sol::this_state ts) {
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

			return mat_at(ts, self, idx);
		}

		if (dims > 2) {
			auto row = self.row(idx).reshape(channels, dims - 1, size.p + 1);
			return std::make_shared<cv::Mat>(row);
		}

		// dims == 2
		if (channels != 1) {
			// treated as dims = 3
			int newsz[] = { size[1], channels };
			auto row = self.row(idx).reshape(1, 2, newsz);
			return std::make_shared<cv::Mat>(row);
		}

		// dims == 2, channels == 1

		// opencv Mat does not support 1D matrix
		// treat a matrix with 1 channel and (1 row or 1 column) as a 1D matrix
		if (size[0] == 1 || size[1] == 1) {
			return mat_at(ts, self, idx);
		}

		auto row = self.row(idx);
		return std::make_shared<cv::Mat>(row);
	}

	double mat_get(cv::Mat& self, sol::this_state ts, sol::variadic_args vargs) {
		sol::state_view lua(ts);

		auto size = vargs.size();
		if (size != self.dims) {
			luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, size);
			return 0.;
		}

		static std::vector<int> idx;
		idx.resize(size);

		int i = 0;
		for (const auto& v : vargs) {
			auto maybe_int = v.as<std::optional<int>>();
			if (!maybe_int) {
				luaL_error(lua.lua_state(), "value at index %d is not an integer", i + 1);
				return 0.;
			}
			idx[i++] = *maybe_int;
		}

		return cvextra::mat_at(self, idx.data());
	}

	void mat_set(cv::Mat& self, sol::this_state ts, sol::variadic_args vargs) {
		sol::state_view lua(ts);

		{
			auto maybe_value = vargs.get<sol::optional<double>>(0);
			if (!maybe_value) {
				goto overload1;
			}
			auto value = *maybe_value;

			auto size = vargs.size() - 1;
			if (size != self.dims) {
				luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, size);
				return;
			}

			std::vector<int> idx(size);
			int i = -1;
			for (const auto& v : vargs) {
				if (i == -1) {
					i++;
					continue;
				}

				auto maybe_int = v.as<std::optional<int>>();
				if (!maybe_int) {
					luaL_error(lua.lua_state(), "value at index %d is not an integer", i + 1);
					return;
				}
				idx[i++] = *maybe_int;
			}

			cvextra::mat_set_at(self, value, idx.data());
			return;
		}
	overload1:
		luaL_error(lua.lua_state(), "Overload resolution failed");
	}

	struct Mat_iterator_state {
		int it;
		int last;
		cv::Mat& self;

		Mat_iterator_state(cv::Mat& self)
			: self(self), it(0), last(self.size[0]) {
		}
	};

	// this gets called
	// to start the first iteration, and every
	// iteration there after
	auto Mat_next(sol::user<Mat_iterator_state&> user_it_state, sol::this_state ts) {
		// the state you passed in my_pairs is argument 1
		// the key value is argument 2, but we do not
		// care about the key value here
		Mat_iterator_state& it_state = user_it_state;
		if (it_state.it == it_state.last) {
			// return nil to signify that
			// there's nothing more to work with.
			return std::make_tuple(sol::object(sol::lua_nil),
				sol::object(sol::lua_nil));
		}

		// 2 values are returned (pushed onto the stack):
		// the key and the value
		// the state is left alone
		auto r = std::make_tuple(
			sol::object(ts, sol::in_place, it_state.it),
			sol::object(ts, sol::in_place, mat_index(it_state.self, it_state.it, ts)));

		// the iterator must be moved forward one before we return
		it_state.it++;

		return r;
	}

	// pairs expects 3 returns:
	// the "next" function on how to advance,
	// the "table" itself or some state,
	// and an initial key value (can be nil)
	auto Mat_pairs(cv::Mat& self) {
		// prepare our state
		Mat_iterator_state it_state(self);

		// sol::user is a space/time optimization over regular
		// usertypes, it's incompatible with regular usertypes and
		// stores the type T directly in lua without any pretty
		// setup saves space allocation and a single dereference
		return std::make_tuple(&Mat_next,
			sol::user<Mat_iterator_state>(std::move(it_state)),
			sol::lua_nil);
	}

	auto mat_unpack(cv::Mat& self, sol::this_state ts) {
		sol::variadic_results vres;

		auto size = self.size;

		// opencv Mat does not support 1D matrix
		// treat a matrix with 1 channel and (1 row or 1 column) as a 1D matrix
		int total = size[0];
		if (self.dims == 2 && self.channels() == 1 && (size[0] == 1 || size[1] == 1)) {
			total = self.total();
		}

		for (int idx = 0; idx < total; idx++) {
			vres.push_back(sol::object(ts, sol::in_place, mat_index(self, idx, ts)));
		}

		return vres;
	}

	double mat_index_as_table(cv::Mat& self, sol::as_table_t<std::vector<int>> idx, sol::this_state ts) {
		if (idx.value().size() == self.dims) {
			return cvextra::mat_at(self, idx.value().data());
		}

		sol::state_view lua(ts);
		luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.value().size());
		return 0.;
	}

	void mat_new_index_as_table(cv::Mat& self, sol::as_table_t<std::vector<int>> idx, double value, sol::this_state ts) {
		if (idx.value().size() == self.dims) {
			cvextra::mat_set_at(self, value, idx.value().data());
			return;
		}
		sol::state_view lua(ts);
		luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.value().size());
	}
}

namespace LUA_MODULE_NAME {
	void register_utils_extension(sol::state_view& lua, sol::table& module) {
		sol::table ns = module["cv"][sol::metatable_key];

		// https://github.com/ThePhD/sol2/issues/1405
		sol::usertype<cv::Mat> mat_type = ns["Mat"];

		mat_type[sol::meta_function::index] = sol::overload(
			&mat_index,
			&mat_index_as_table
		);

		mat_type[sol::meta_function::new_index] = sol::overload(
			[](cv::Mat& self, int idx, double value) {
				cvextra::mat_set_at(self, value, idx);
			},
			&mat_new_index_as_table
		);

		mat_type[sol::meta_function::length] = [](cv::Mat& self) {
			return self.size[0];
			};

		mat_type[sol::meta_function::pairs] = &Mat_pairs;
		mat_type.set_function("__unpack", &mat_unpack);

		mat_type.set_function("index_table", &mat_index_as_table);
		mat_type.set_function("new_index_table", &mat_new_index_as_table);

		mat_type[sol::meta_function::call] = &mat_get;
		mat_type.set_function("get", &mat_get);
		mat_type.set_function("set", &mat_set);
	}
}
