#include <lua_generated_pch.hpp>
#include <mutex>

#define BRET(b) return (int32_t) (b)

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

	double mat_get(cv::Mat& self, sol::this_state ts, sol::variadic_args vargs) {
		sol::state_view lua(ts);

		auto size = vargs.size();
		if (size != self.dims) {
			luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, size);
			return 0.;
		}

		std::vector<int> idx(size);
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

	double mat_index_as_vector(cv::Mat& self, std::vector<int> idx, sol::this_state ts) {
		if (idx.size() == self.dims) {
			return cvextra::mat_at(self, idx.data());
		}

		sol::state_view lua(ts);
		luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.size());
		return 0.;
	}

	void mat_new_index_as_vector(cv::Mat& self, std::vector<int> idx, double value, sol::this_state ts) {
		if (idx.size() == self.dims) {
			cvextra::mat_set_at(self, value, idx.data());
			return;
		}
		sol::state_view lua(ts);
		luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.size());
	}

	double mat_index_maybe(cv::Mat& self, sol::stack_object obj, sol::this_state ts) {
		sol::state_view lua(ts);

		auto maybe_idx = maybe<std::vector<int>>(obj);
		if (!maybe_idx) {
			luaL_error(lua.lua_state(), "Overload resolution failed");
			return 0.;
		}

		auto& idx = *maybe_idx;

		if (idx.size() == self.dims) {
			return cvextra::mat_at(self, idx.data());
		}

		luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.size());
		return 0.;
	}

	void mat_new_index_maybe(cv::Mat& self, sol::stack_object obj, double value, sol::this_state ts) {
		sol::state_view lua(ts);

		auto maybe_idx = maybe<std::vector<int>>(obj);
		if (!maybe_idx) {
			luaL_error(lua.lua_state(), "Overload resolution failed");
			return;
		}

		auto& idx = *maybe_idx;

		if (idx.size() == self.dims) {
			cvextra::mat_set_at(self, value, idx.data());
			return;
		}

		luaL_error(lua.lua_state(), "matrix has %d dimensions, but given index has %d dimensions", self.dims, idx.size());
	}

	auto tobit(int b) {
		BRET(b);
	}

	auto bnot(int b) {
		BRET(~b);
	}

	auto band(int b, sol::this_state ts, sol::variadic_args vargs) {
		sol::state_view lua(ts);

		int i = 1;
		for (const auto& v : vargs) {
			auto maybe_int = v.as<std::optional<int>>();
			if (!maybe_int) {
				luaL_error(lua.lua_state(), "value at index %d is not an integer", i + 1);
				return 0;
			}
			b &= *maybe_int;
		}

		BRET(b);
	}

	auto bor(int b, sol::this_state ts, sol::variadic_args vargs) {
		sol::state_view lua(ts);

		int i = 1;
		for (const auto& v : vargs) {
			auto maybe_int = v.as<std::optional<int>>();
			if (!maybe_int) {
				luaL_error(lua.lua_state(), "value at index %d is not an integer", i + 1);
				return 0;
			}
			b |= *maybe_int;
		}

		BRET(b);
	}

	auto bxor(int b, sol::this_state ts, sol::variadic_args vargs) {
		sol::state_view lua(ts);

		int i = 1;
		for (const auto& v : vargs) {
			auto maybe_int = v.as<std::optional<int>>();
			if (!maybe_int) {
				luaL_error(lua.lua_state(), "value at index %d is not an integer", i + 1);
				return 0;
			}
			b ^= *maybe_int;
		}

		BRET(b);
	}

	auto lshift(int b, int n) {
		return b << n;
	}

	auto rshift(int b, int n) {
		BRET(b >> n);
	}

	auto arshift(int b, int n) {
		BRET((int32_t)b >> n);
	}

	auto rol(int b, int n) {
		BRET((b << n) | (b >> (32 - n)));
	}

	auto ror(int b, int n) {
		BRET((b << (32 - n)) | (b >> n));
	}

	auto bwsap(int32_t b) {
		b = (b >> 24) | ((b >> 8) & 0xff00) | ((b & 0xff00) << 8) | (b << 24);
		BRET(b);
	}

	auto tohex(int32_t b, int32_t n) {
		const char* hexdigits = "0123456789abcdef";
		char buf[8];
		if (n < 0) {
			n = -n;
			hexdigits = "0123456789ABCDEF";
		}
		if (n > 8) {
			n = 8;
		}
		for (int i = (int)n; --i >= 0; ) {
			buf[i] = hexdigits[b & 15];
			b >>= 4;
		}
		return std::string(buf, n);
	}
}

namespace LUA_MODULE_NAME {
	void register_extension(sol::state_view& lua, sol::table& module) {
		sol::table bit = lua.create_table();
		module["bit"] = bit;

		bit.set_function("tobit", tobit);
		bit.set_function("bnot", bnot);
		bit.set_function("band", band);
		bit.set_function("bor", bor);
		bit.set_function("bxor", bxor);
		bit.set_function("lshift", lshift);
		bit.set_function("rshift", rshift);
		bit.set_function("arshift", arshift);
		bit.set_function("rol", rol);
		bit.set_function("ror", ror);
		bit.set_function("tohex", sol::overload([](int32_t b) {
			return tohex(b, 8);
			}, tohex));

		sol::table ns = module["cv"][sol::metatable_key];

		// https://github.com/ThePhD/sol2/issues/1405
		sol::usertype<cv::Mat> mat_type = module["cv"]["Mat"];

		mat_type.set_function(sol::meta_function::index, sol::overload(
			&mat_index,
			&mat_index_as_table
		));

		mat_type.set_function(sol::meta_function::new_index, sol::overload(
			[](cv::Mat& self, int idx, double value) {
				cvextra::mat_set_at(self, value, idx);
			},
			&mat_new_index_as_table
		));

		mat_type.set_function(sol::meta_function::length, [](cv::Mat& self) {
			return self.size[0];
			});

		mat_type.set_function("index_table", &mat_index_as_table);
		mat_type.set_function("new_index_table", &mat_new_index_as_table);

		mat_type.set_function("index_vector", &mat_index_as_vector);
		mat_type.set_function("new_index_vector", &mat_new_index_as_vector);

		mat_type.set_function("index_maybe", &mat_index_maybe);
		mat_type.set_function("new_index_maybe", &mat_new_index_maybe);

		mat_type.set_function(sol::meta_function::call, &mat_get);
		mat_type.set_function("get", &mat_get);
		mat_type.set_function("set", &mat_set);
	}
}
