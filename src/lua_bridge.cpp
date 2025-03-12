#include <registration.hpp>

namespace cvextra {
	using namespace LUA_MODULE_NAME;

	template<typename T>
	inline void Mat_newindex_at(lua_State* L, cv::Mat& self, const T& index, const Table& array) {
		const cv::Mat value = createMatFromArray(L, array, self.depth());
		Mat_newindex_at(L, self, index, value);
	}
}

namespace {
	using namespace LUA_MODULE_NAME;
	using namespace cvextra;

	auto Lua_sol_meta_function_length(lua_State* L) {
		bool is_valid;
		auto self = lua_to(L, 1, static_cast<cv::Mat*>(nullptr), is_valid);
		if (!is_valid || !self || lua_isnil(L, 1)) {
			return luaL_typeerror(L, 1, "cv::Mat");
		}
		return lua_push(L, self->size.p[0]);
	}

	auto Lua_sol_meta_function_index(lua_State* L) {
		bool is_valid;

		{
			const auto index = lua_to(L, 2, static_cast<int*>(nullptr), is_valid);
			if (is_valid) {
				auto self = lua_to(L, 1, static_cast<cv::Mat*>(nullptr), is_valid);
				if (!is_valid || !self || lua_isnil(L, 1)) {
					return luaL_typeerror(L, 1, "cv::Mat");
				}

				return Mat_index_at(L, *self, index);
			}
		}

		if (lua_istable(L, 2)) {
			using Variant = cv::util::variant<int, cv::Range>;

			auto idx_holder = lua_to(L, 2, static_cast<std::vector<Variant>*>(nullptr), is_valid);
			if (is_valid) {
				auto self = lua_to(L, 1, static_cast<cv::Mat*>(nullptr), is_valid);
				if (!is_valid || !self || lua_isnil(L, 1)) {
					return luaL_typeerror(L, 1, "cv::Mat");
				}

				decltype(auto) idx = extract_holder(idx_holder, static_cast<std::vector<Variant>*>(nullptr));

				static std::vector<int> indexes; indexes.resize(idx.size());
				static std::vector<cv::Range> ranges; ranges.resize(idx.size());

				auto i = 0;
				auto j = 0;

				for (const auto& v : idx) {
					if (cv::util::holds_alternative<int>(v)) {
						indexes[i++] = cv::util::get<int>(v);
					} else {
						ranges[j++] = cv::util::get<cv::Range>(v);
					}
				}

				if (i == idx.size()) {
					return Mat_index_at(L, *self, indexes);
				} else if (j == idx.size()) {
					return Mat_index_at(L, *self, ranges);
				} else {
					return Mat_index_at(L, *self, idx);
				}
			}
		}

		return lua_class__index<0, ::cv::Mat>(L); // fallback to the default __index method
	}

	template<typename T>
	inline int Lua_newindex_at(lua_State* L) {
		bool is_valid;

		{
			const auto index = lua_to(L, 2, static_cast<int*>(nullptr), is_valid);
			if (is_valid) {
				auto self = lua_to(L, 1, static_cast<cv::Mat*>(nullptr), is_valid);
				if (!is_valid || !self || lua_isnil(L, 1)) {
					return luaL_typeerror(L, 1, "cv::Mat");
				}

				const auto value = lua_to(L, 3, static_cast<T*>(nullptr), is_valid);
				if (!is_valid) {
					return luaL_typeerror(L, 1, internal::GetTypeName<T>());
				}

				Mat_newindex_at(L, *self, index, extract_holder(value, static_cast<T*>(nullptr)));
				return 0;
			}
		}

		if (lua_istable(L, 2)) {
			auto idx_holder = lua_to(L, 2, static_cast<std::vector<cv::util::variant<int, cv::Range>>*>(nullptr), is_valid);
			if (is_valid) {
				auto self = lua_to(L, 1, static_cast<cv::Mat*>(nullptr), is_valid);
				if (!is_valid || !self || lua_isnil(L, 1)) {
					return luaL_typeerror(L, 1, "cv::Mat");
				}

				decltype(auto) idx = extract_holder(idx_holder, static_cast<T*>(nullptr));
				const auto value = lua_to(L, 3, static_cast<T*>(nullptr), is_valid);
				if (!is_valid) {
					return luaL_typeerror(L, 1, internal::GetTypeName<T>());
				}

				static std::vector<int> indexes; indexes.resize(idx.size());
				static std::vector<cv::Range> ranges; ranges.resize(idx.size());

				auto i = 0;
				auto j = 0;

				for (const auto& v : idx) {
					if (cv::util::holds_alternative<int>(v)) {
						indexes[i++] = cv::util::get<int>(v);
					} else {
						ranges[j++] = cv::util::get<cv::Range>(v);
					}
				}

				if (i == idx.size()) {
					Mat_newindex_at(L, *self, indexes, extract_holder(value, static_cast<T*>(nullptr)));
				} else if (j == idx.size()) {
					Mat_newindex_at(L, *self, ranges, extract_holder(value, static_cast<T*>(nullptr)));
				} else {
					Mat_newindex_at(L, *self, idx, extract_holder(value, static_cast<T*>(nullptr)));
				}

				return 0;
			}
		}

		return -1;
	}

	auto Lua_sol_meta_function_newindex(lua_State* L) {
		bool is_valid;
		lua_to(L, 3, static_cast<cv::Mat*>(nullptr), is_valid);
		if (is_valid) {
			const auto result = Lua_newindex_at<cv::Mat>(L);
			if (result == 0) {
				return result;
			}
		}

		if (lua_type(L, 3) == LUA_TNUMBER) {
			const auto result = Lua_newindex_at<double>(L);
			if (result == 0) {
				return result;
			}
		}

		if (lua_istable(L, 3)) {
			const auto result = Lua_newindex_at<Table>(L);
			if (result == 0) {
				return result;
			}
		}

		return lua_class__newindex<0, ::cv::Mat>(L); // fallback to the default __newindex method
	}

	auto Lua_get(lua_State* L) {
		bool is_valid;

		auto self = lua_to(L, 1, static_cast<cv::Mat*>(nullptr), is_valid);
		if (!is_valid) {
			return luaL_typeerror(L, 1, "cv::Mat");
		}

		auto size = lua_gettop(L) - 1;

		static std::vector<cv::util::variant<int, cv::Range>> idx; idx.resize(size);
		static std::vector<int> indexes; indexes.resize(size);
		static std::vector<cv::Range> ranges; ranges.resize(size);

		bool has_int = false;
		bool has_range = false;
		bool has_both = false;

		for (int i = 0; i < size; i++) {
			indexes[i] = lua_to(L, i + 2, static_cast<int*>(nullptr), is_valid);
			if (is_valid) {
				has_int = true;
				idx[i] = indexes[i];
				continue;
			}

			ranges[i] = lua_to(L, i + 2, static_cast<cv::Range*>(nullptr), is_valid);
			if (is_valid) {
				has_range = true;
				idx[i] = ranges[i];
				continue;
			}

			return luaL_typeerror(L, i + 2, "cv::util::variant<int, cv::Range>");
		}

		if (!has_range) {
			return Mat_index_at(L, *self, indexes);
		}

		if (!has_int) {
			return Mat_index_at(L, *self, ranges);
		}

		return Mat_index_at(L, *self, idx);
	}

	const struct luaL_Reg extended_methods[] = {
		{"__len", Lua_sol_meta_function_length},
		{"__index", Lua_sol_meta_function_index},
		{"__newindex", Lua_sol_meta_function_newindex},
		{"__call", Lua_get},
		{"get", Lua_get},
		{NULL, NULL} // Sentinel
	};

	void extend_Mat(lua_State* L) {
		lua_rawget_create_if_nil(L, { "cv", "Mat" });

		lua_pushfuncs(L, extended_methods);

		lua_pop(L, 1);
	}
}


#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <dlfcn.h>
#include <cstdlib>
#include <stdlib.h>

namespace fs = std::filesystem;

#define _stringify(s) #s
#define stringify(s) _stringify(s)

namespace {
	void configure_qt_qpa() {
		Dl_info info;
		auto res = dladdr((void const*)&LUA_MODULE_LUAOPEN, &info);
		if (!res) {
			return;
		}

		fs::path shared_object = fs::absolute(info.dli_fname);
		auto rpath = shared_object.parent_path();

		auto plugins = rpath / stringify(LUA_MODULE_NAME) / "qt" / "plugins";
		if (fs::exists(plugins)) {
			setenv("QT_QPA_PLATFORM_PLUGIN_PATH", plugins.native().c_str(), 0); // does not overwrite
		}

		auto fonts = rpath / stringify(LUA_MODULE_NAME) / "qt" / "fonts";
		if (fs::exists(plugins)) {
			setenv("QT_QPA_FONTDIR", fonts.native().c_str(), 0); // does not overwrite
		}
	}
}
#endif

namespace LUA_MODULE_NAME {
	template<typename T>
	std::shared_ptr<T> exported_lua_to(lua_State* L, int index, T* ptr, bool& is_valid) {
		return lua_to(L, index, ptr, is_valid);
	}

	template<typename T>
	int exported_lua_push(lua_State* L, T* ptr) {
		return lua_push(L, ptr);
	}

	template<typename T>
	int exported_lua_push(lua_State* L, T&& obj) {
		return lua_push(L, std::move(obj));
	}

	template<typename T>
	int exported_lua_push(lua_State* L, const T& obj) {
		return lua_push(L, obj);
	}

	template
	LUA_EXPORTS std::shared_ptr<cv::Mat> exported_lua_to<cv::Mat>(lua_State* L, int index, cv::Mat* ptr, bool& is_valid);

	template
	LUA_EXPORTS int exported_lua_push<cv::Mat>(lua_State* L, cv::Mat* ptr);

	template
	LUA_EXPORTS int exported_lua_push<cv::Mat>(lua_State* L, cv::Mat&& obj);

	template
	LUA_EXPORTS int exported_lua_push<cv::Mat>(lua_State* L, const cv::Mat& obj);

	template
	LUA_EXPORTS int exported_lua_push<std::shared_ptr<cv::Mat>>(lua_State* L, const std::shared_ptr<cv::Mat>& obj);
}

namespace LUA_MODULE_NAME {
	void register_extensions(lua_State* L) {
#ifdef __linux__
		configure_qt_qpa();
#endif
		extend_Mat(L);
	}
}
