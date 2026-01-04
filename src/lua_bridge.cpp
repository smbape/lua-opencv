#include <lua_bridge.hpp>

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
		lua_push(L, self->size.p[0]);
		return 1;
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

namespace fs = std::filesystem;

#define _stringify(s) #s
#define stringify(s) _stringify(s)

#ifdef __linux__
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <dlfcn.h>
#include <cstdlib>
#include <stdlib.h>

namespace {
	fs::path get_module_filename() {
		Dl_info info;
		auto res = dladdr((void const*)&LUA_MODULE_LUAOPEN, &info);
		if (res) {
			return fs::absolute(info.dli_fname);
		}

		// Unable to get the path
		return fs::path();
	}
}
#elif defined(_WIN32)
#include <Windows.h>

namespace {
	// https://github.com/opencv/opencv/blob/4.11.0/modules/core/src/utils/datafile.cpp#L165-L187
	fs::path get_module_filename() {
		void* addr = (void*)&LUA_MODULE_LUAOPEN; // using code address, doesn't work with static linkage!
		HMODULE m = nullptr;
#if _WIN32_WINNT >= 0x0501 && (!defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))
		::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			static_cast<LPCTSTR>(addr),
			&m);
#endif
		if (m) {
			wchar_t lpFilename[4096];
			constexpr size_t nSize = sizeof(lpFilename) / sizeof(*lpFilename);

			auto sz = GetModuleFileNameW(m, lpFilename, nSize);
			if (sz > 0 && sz < nSize) {
				lpFilename[sz] = '\0';
				return fs::absolute(lpFilename);
			}
		}

		// Unable to get the path
		return fs::path();
	}
}
#endif

#ifdef __linux__
namespace {
	void configure_qt_qpa() {
		auto module_filename = get_module_filename();
		if (module_filename.empty()) {
			// unable to get module path
			return;
		}

		auto rpath = module_filename.parent_path();

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

namespace {
#ifdef _MSC_VER
	template <typename char_type>
	inline bool null_or_empty(const char_type* s) {
		return s == nullptr || *s == 0;
	}

	/**
	 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
	 *
	 * @param  codePage Code page to use in performing the conversion.
	 * @param  c_wstr   Pointer to the Unicode string to convert.
	 * @param  length   Size, in characters, of the string indicated by c_wstr parameter.
	 * @param  str      Pointer to a buffer that receives the converted string.
	 * @return          The number of bytes written to the buffer pointed to by c_str.
	 * @see             https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
	 */
	inline int wcs_to_mbs(UINT codePage, const fs::path::value_type* c_wstr, size_t length, std::string& str) {
		if (null_or_empty(c_wstr)) {
			str.clear();
			return 0;
		}

		int size = WideCharToMultiByte(codePage, 0, c_wstr, length, nullptr, 0, nullptr, nullptr);
		str.assign(size, 0);
		return WideCharToMultiByte(codePage, 0, c_wstr, length, &str[0], size + 1, nullptr, nullptr);
	}

	inline int wcs_to_utf8(const fs::path::value_type* c_wstr, size_t length, std::string& str) {
		return wcs_to_mbs(CP_UTF8, c_wstr, length, str);
	}
#endif

	inline std::string _string_type_to_string(const fs::path::string_type& match) {
#ifdef _MSC_VER
		std::string str; wcs_to_utf8(match.c_str(), match.length(), str);
		return str;
#else
		return match;
#endif
	}
}

#ifdef _WIN32

#ifndef UNICODE
#define __getenv getenv_s
#define __putenv _putenv_s
#define OPENCV_FFMPEG_DLL_DIR "OPENCV_FFMPEG_DLL_DIR"
#else
#define __getenv _wgetenv_s
#define __putenv _wputenv_s
#define OPENCV_FFMPEG_DLL_DIR L"OPENCV_FFMPEG_DLL_DIR"
#endif

namespace {
	void configure_ffmpeg_dll_dir() {
		size_t size;
		__getenv(&size, nullptr, 0, OPENCV_FFMPEG_DLL_DIR);
		if (size != 0) {
			// OPENCV_FFMPEG_DLL_DIR is already defined
			return;
		}

		auto module_filename = get_module_filename();
		if (module_filename.empty()) {
			// unable to get module path
			return;
		}

		auto base_directory = module_filename.parent_path() / stringify(LUA_MODULE_NAME) / "bin";
		if (!fs::exists(base_directory)) {
			// bin path does not exists
			return;
		}

#ifndef UNICODE
		const auto env = _string_type_to_string(base_directory.native());
#else
		const auto env = base_directory.native();
#endif

		__putenv(OPENCV_FFMPEG_DLL_DIR, env.c_str());
	}
}
#endif

namespace LUA_MODULE_NAME {
	template<typename T>
	std::shared_ptr<T> exported_lua_to(lua_State* L, int index, T* ptr, bool& is_valid) {
		return lua_to(L, index, ptr, is_valid);
	}

	template<typename T>
	void exported_lua_push(lua_State* L, T* ptr) {
		lua_push(L, ptr);
	}

	template<typename T>
	void exported_lua_push(lua_State* L, T&& obj) {
		lua_push(L, std::move(obj));
	}

	template<typename T>
	void exported_lua_push(lua_State* L, const T& obj) {
		lua_push(L, obj);
	}

	template
	LUA_EXPORTS std::shared_ptr<cv::Mat> exported_lua_to<cv::Mat>(lua_State* L, int index, cv::Mat* ptr, bool& is_valid);

	template
	LUA_EXPORTS void exported_lua_push<cv::Mat>(lua_State* L, cv::Mat* ptr);

	template
	LUA_EXPORTS void exported_lua_push<cv::Mat>(lua_State* L, cv::Mat&& obj);

	template
	LUA_EXPORTS void exported_lua_push<cv::Mat>(lua_State* L, const cv::Mat& obj);

	template
	LUA_EXPORTS void exported_lua_push<std::shared_ptr<cv::Mat>>(lua_State* L, const std::shared_ptr<cv::Mat>& obj);
}

namespace LUA_MODULE_NAME {
	void register_extensions(lua_State* L) {
#ifdef __linux__
		configure_qt_qpa();
#endif
#ifdef _WIN32
		configure_ffmpeg_dll_dir();
#endif
		extend_Mat(L);
	}
}
