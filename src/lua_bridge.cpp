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
		::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
		if (!self) {
			return luaL_typeerror(L, 1, "cv::Mat");
		}
		return lua_push(L, self->size.p[0]);
	}

	auto Lua_sol_meta_function_index(lua_State* L) {
		if (lua_is(L, 2, static_cast<int*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			const auto& index = lua_to(L, 2, static_cast<int*>(nullptr));
			return Mat_index_at(L, *self, index);
		}

		if (lua_istable(L, 2) && lua_is(L, 2, static_cast<std::vector<int>*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			static std::vector<int> index; lua_to(L, 2, index);
			return Mat_index_at(L, *self, index);
		}

		if (lua_istable(L, 2) && lua_is(L, 2, static_cast<std::vector<cv::Range>*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			static std::vector<cv::Range> index; lua_to(L, 2, index);
			return Mat_index_at(L, *self, index);
		}

		if (lua_istable(L, 2) && lua_is(L, 2, static_cast<std::vector<std::variant<int, cv::Range>>*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			static std::vector<std::variant<int, cv::Range>> index; lua_to(L, 2, index);
			return Mat_index_at(L, *self, index);
		}

		return lua_class__index<0, ::cv::Mat>(L); // fallback to the default __index method
	}

	template<typename T>
	inline int Lua_newindex_at(lua_State* L) {
		if (lua_is(L, 2, static_cast<int*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			const auto& index = lua_to(L, 2, static_cast<int*>(nullptr));
			const auto value = lua_to(L, 3, static_cast<T*>(nullptr));
			Mat_newindex_at(L, *self, index, extract_holder(value, static_cast<T*>(nullptr)));
			return 0;
		}

		if (lua_istable(L, 2) && lua_is(L, 2, static_cast<std::vector<int>*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			static std::vector<int> index; lua_to(L, 2, index);
			const auto value = lua_to(L, 3, static_cast<T*>(nullptr));
			Mat_newindex_at(L, *self, index, extract_holder(value, static_cast<T*>(nullptr)));
			return 0;
		}

		if (lua_istable(L, 2) && lua_is(L, 2, static_cast<std::vector<cv::Range>*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			static std::vector<cv::Range> index; lua_to(L, 2, index);
			const auto value = lua_to(L, 3, static_cast<T*>(nullptr));
			Mat_newindex_at(L, *self, index, extract_holder(value, static_cast<T*>(nullptr)));
			return 0;
		}

		if (lua_istable(L, 2) && lua_is(L, 2, static_cast<std::vector<std::variant<int, cv::Range>>*>(nullptr))) {
			::cv::Mat* self = lua_is(L, 1, static_cast<cv::Mat*>(nullptr)) ? lua_to(L, 1, static_cast<cv::Mat*>(nullptr)).get() : nullptr;
			if (!self) {
				return luaL_typeerror(L, 1, "cv::Mat");
			}

			static std::vector<std::variant<int, cv::Range>> index; lua_to(L, 2, index);
			const auto value = lua_to(L, 3, static_cast<T*>(nullptr));
			Mat_newindex_at(L, *self, index, extract_holder(value, static_cast<T*>(nullptr)));
			return 0;
		}

		return -1;
	}

	auto Lua_sol_meta_function_newindex(lua_State* L) {
		if (lua_is(L, 3, static_cast<cv::Mat*>(nullptr))) {
			const auto result = Lua_newindex_at<cv::Mat>(L);
			if (result == 0) {
				return result;
			}
		}

		if (lua_is(L, 3, static_cast<double*>(nullptr))) {
			const auto result = Lua_newindex_at<double>(L);
			if (result == 0) {
				return result;
			}
		}

		if (lua_is(L, 3, static_cast<Table*>(nullptr))) {
			const auto result = Lua_newindex_at<Table>(L);
			if (result == 0) {
				return result;
			}
		}

		return lua_class__newindex<0, ::cv::Mat>(L); // fallback to the default __newindex method
	}

	auto Lua_get(lua_State* L) {
		if (!lua_is(L, 1, static_cast<cv::Mat*>(nullptr))) {
			return luaL_typeerror(L, 1, "cv::Mat");
		}

		auto self = lua_to(L, 1, static_cast<cv::Mat*>(nullptr));
		auto size = lua_gettop(L) - 1;

		static std::vector<std::variant<int, cv::Range>> idx; idx.resize(size);
		static std::vector<int> indexes; indexes.resize(size);
		static std::vector<cv::Range> ranges; ranges.resize(size);

		bool has_int = false;
		bool has_range = false;
		bool has_both = false;

		for (int i = 0; i < size; i++) {
			has_int = lua_is(L, i + 2, static_cast<int*>(nullptr)) || has_int;
			has_range = lua_is(L, i + 2, static_cast<cv::Range*>(nullptr)) || has_range;
			if (!has_int && !has_range) {
				return luaL_typeerror(L, i + 2, "std::variant<int, cv::Range>");
			}

			if (has_int && has_range && !has_both) {
				has_both = true;
				if (lua_is(L, i + 2, static_cast<int*>(nullptr))) {
					for (int j = 0; j < i; j++) {
						idx[j] = ranges[j];
					}
				}
				else {
					for (int j = 0; j < i; j++) {
						idx[j] = indexes[j];
					}
				}
			}

			if (!has_range) {
				indexes[i] = lua_to(L, i + 2, static_cast<int*>(nullptr));
			}
			else if (!has_int) {
				ranges[i] = lua_to(L, i + 2, static_cast<cv::Range*>(nullptr));
			}
			else {
				idx[i] = lua_is(L, i + 2, static_cast<std::variant<int, cv::Range>*>(nullptr));
			}
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
	LUA_EXPORTS bool exported_lua_is(lua_State* L, int index, T* ptr) {
		return lua_is(L, index, ptr);
	}

	template<typename T>
	LUA_EXPORTS std::shared_ptr<T> exported_lua_to(lua_State* L, int index, T* ptr) {
		return lua_to(L, index, ptr);
	}

	template<typename T>
	LUA_EXPORTS int exported_lua_push(lua_State* L, T* ptr) {
		return lua_push(L, ptr);
	}

	template<typename T>
	LUA_EXPORTS int exported_lua_push(lua_State* L, T&& obj) {
		return lua_push(L, std::move(obj));
	}

	template<typename T>
	LUA_EXPORTS int exported_lua_push(lua_State* L, const T& obj) {
		return lua_push(L, obj);
	}

	template
	LUA_EXPORTS bool exported_lua_is<cv::Mat>(lua_State* L, int index, cv::Mat* ptr);

	template
	LUA_EXPORTS std::shared_ptr<cv::Mat> exported_lua_to<cv::Mat>(lua_State* L, int index, cv::Mat* ptr);

	template
	LUA_EXPORTS int exported_lua_push<cv::Mat>(lua_State* L, cv::Mat* ptr);

	template
	LUA_EXPORTS int exported_lua_push<cv::Mat>(lua_State* L, cv::Mat&& obj);

	template
	LUA_EXPORTS int exported_lua_push<cv::Mat>(lua_State* L, const cv::Mat& obj);
}

namespace LUA_MODULE_NAME {
	void register_extensions(lua_State* L) {
#ifdef __linux__
		configure_qt_qpa();
#endif
		extend_Mat(L);
	}
}
