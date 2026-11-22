#pragma once

#include <luadef.hpp>

#ifdef _MSC_VER
#pragma push_macro("NOMINMAX")
#pragma push_macro("STRICT")
#pragma push_macro("RELATIVE")
#pragma push_macro("ABSOLUTE")
#define NOMINMAX
#define STRICT

// curl/lib/setup-win32.h includes winsock2.h before windows.h.
// Windows.h includes winsock.h,
// which conflicts with winsock2.h if included before winsock2.h.
// To fix the issue, include winsock2.h before windows.h like in curl/lib/setup-win32.h
#define USE_WINSOCK 2
#include <winsock2.h>

#include <Windows.h>
#pragma pop_macro("NOMINMAX")
#pragma pop_macro("STRICT")
#pragma pop_macro("RELATIVE")
#pragma pop_macro("ABSOLUTE")
#endif

namespace LUA_MODULE_NAME {
#ifdef _MSC_VER
	namespace wide_char {
		template <typename char_type>
		inline bool null_or_empty(const char_type* s) {
			return s == nullptr || *s == 0;
		}

		/**
		 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
		 *
		 * @param codePage    [in]  Code page to use in performing the conversion.
		 * @param c_str       [in]  Pointer to the character string to convert.
		 * @param cbMultiByte [in]  Size, in bytes, of the string indicated by the c_str parameter. Alternatively, this parameter can be set to -1 if the string is null-terminated.
		 * @param wstr        [out] Pointer to a buffer that receives the converted string.
		 * @return 	          The number of characters written to the buffer pointed to by wstr.
		 * @see               https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
		 *                    https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		 */
		inline int mbs_to_wcs(UINT codePage, const char* c_str, int cbMultiByte, std::wstring& wstr) {
			if (null_or_empty(c_str)) {
				wstr.clear();
				return 0;
			}

			int size = MultiByteToWideChar(codePage, 0, c_str, cbMultiByte, nullptr, 0);
			wstr.assign(size, 0);
			return MultiByteToWideChar(codePage, 0, c_str, cbMultiByte, &wstr[0], size + 1);
		}

		/**
		 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
		 *
		 * @param codePage  [in]  Code page to use in performing the conversion.
		 * @param  str      [in]  The string to convert.
		 * @param  wstr     [out] Pointer to a buffer that receives the converted string.
		 * @see             https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
		 *                  https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		 */
		inline int mbs_to_wcs(UINT codePage, const std::string& str, std::wstring& wstr) {
			return mbs_to_wcs(codePage, str.c_str(), str.length(), wstr);
		}

		/**
		 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
		 *
		 * @param  codePage    Code page to use in performing the conversion.
		 * @param  c_wstr      Pointer to the Unicode string to convert.
		 * @param  cchWideChar Size, in characters, of the string indicated by c_wstr parameter.
		 * @param  str         Pointer to a buffer that receives the converted string.
		 * @return             The number of bytes written to the buffer pointed to by c_str.
		 * @see                https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		 */
		inline int wcs_to_mbs(UINT codePage, const WCHAR* c_wstr, int cchWideChar, std::string& str) {
			if (null_or_empty(c_wstr)) {
				str.clear();
				return 0;
			}

			int size = WideCharToMultiByte(codePage, 0, c_wstr, cchWideChar, nullptr, 0, nullptr, nullptr);
			str.assign(size, 0);
			return WideCharToMultiByte(codePage, 0, c_wstr, cchWideChar, &str[0], size + 1, nullptr, nullptr);
		}

		/**
		 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
		 *
		 * @param  codePage Code page to use in performing the conversion.
		 * @param  wstr     Pointer to the Unicode string to convert.
		 * @param  str      Pointer to a buffer that receives the converted string.
		 * @return          The number of bytes written to the buffer pointed to by str.
		 * @see             https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		 */
		inline int wcs_to_mbs(UINT codePage, const std::wstring& wstr, std::string& str) {
			return wcs_to_mbs(codePage, wstr.c_str(), wstr.length(), str);
		}

		/**
		 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
		 *
		 * @param c_str     [in]  Pointer to the character string to convert.
		 * @param length    [in]  Size, in bytes, of the string indicated by the c_str parameter. Alternatively, this parameter can be set to -1 if the string is null-terminated.
		 * @param wstr      [out] Pointer to a buffer that receives the converted string.
		 * @return 	        The number of characters written
		 * @see             https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
		 *                  https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		 */
		inline int utf8_to_wcs(const char* c_str, int length, std::wstring& wstr) {
			return mbs_to_wcs(CP_UTF8, c_str, length, wstr);
		}

		/**
		 * Maps a character string to a UTF-16 (wide character) string. The character string is not necessarily from a multibyte character set.
		 *
		 * @param  str      [in]  The string to convert.
		 * @param  wstr     [out] Pointer to a buffer that receives the converted string.
		 * @see             https://stackoverflow.com/questions/6693010/how-do-i-use-multibytetowidechar/59617138#59617138
		 *                  https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar
		 */
		inline int utf8_to_wcs(const std::string& str, std::wstring& wstr) {
			return mbs_to_wcs(CP_UTF8, str, wstr);
		}

		/**
		 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
		 *
		 * @param  c_wstr      Pointer to the Unicode string to convert.
		 * @param  cchWideChar Size, in characters, of the string indicated by c_wstr parameter.
		 * @param  str         Pointer to a buffer that receives the converted string.
		 * @return             The number of bytes written to the buffer pointed to by c_str.
		 * @see                https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		 */
		inline int wcs_to_utf8(const WCHAR* c_wstr, int cchWideChar, std::string& str) {
			return wcs_to_mbs(CP_UTF8, c_wstr, cchWideChar, str);
		}

		/**
		 * Maps a UTF-16 (wide character) string to a new character string. The new character string is not necessarily from a multibyte character set.
		 *
		 * @param  wstr     Pointer to the Unicode string to convert.
		 * @param  str      Pointer to a buffer that receives the converted string.
		 * @return          The number of bytes written to the buffer pointed to by str.
		 * @see             https://learn.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-widechartomultibyte
		 */
		inline int wcs_to_utf8(const std::wstring& wstr, std::string& str) {
			return wcs_to_mbs(CP_UTF8, wstr, str);
		}
	}
#endif

	void register_Common(lua_State* L);

	class Gil {
	public:
		int lock_unsafe(std::unique_lock<std::mutex>& lk, int locks);
		int lock(int locks = 1);

		void unlock_unsafe(int locks);
		int unlock(int locks = 1);

		int release(int& id);
		void acquire(int id, int locks);

		void release_if_owned_by(std::thread::id id);
		void close();

		std::unique_lock<std::mutex> create_lock();

	private:
		std::thread::id owner_;
		std::mutex m_;
		std::condition_variable cv_;
		int locks_ = 0;
		int ids_ = 0;
		bool closed_ = false;
	};

	using GilPtr = std::shared_ptr<Gil>;

	GilPtr get_gil_of(lua_State* L);

	class GilGuard {
	public:
		GilGuard(lua_State* L);
		GilGuard(GilPtr gil);
		~GilGuard();

	private:
		GilPtr gil_;
	};

	class GilYieldGuard {
	public:
		GilYieldGuard(lua_State* L);
		GilYieldGuard(lua_State* L, long timeout_ms);
		GilYieldGuard(GilPtr gil, long timeout_ms);
		~GilYieldGuard();

	private:
		GilPtr gil_;
		long timeout_ms_;
		int rlocks_;
		int rid_;
	};

	class GilUnlockGuard {
	public:
		GilUnlockGuard(lua_State* L);
		GilUnlockGuard(GilPtr gil);
		~GilUnlockGuard();
		std::unique_lock<std::mutex>& get_lock();

	private:
		GilPtr gil_;
		std::unique_lock<std::mutex> lk_;
	};

	int yield(lua_State* L);

	int __call_constructor(lua_State* L);


	// ================================
	// LuaStateContext
	// ================================

	class LuaStateContext {
	public:
		struct MetatableInfo {
			const void* ptr = nullptr;
			int ref = LUA_REFNIL;
		};

		template<typename T>
		void register_class(const void* ptr, int ref) {
			get_info<T>() = { ptr, ref };
		}

		template<typename T>
		const void* get_metatable_pointer() {
			return get_info<T>().ptr;
		}

		template<typename T>
		void push_metatable(lua_State* L) {
			lua_rawgeti(L, LUA_REGISTRYINDEX, get_info<T>().ref);
		}

		template<typename T>
		bool is_registered() {
			return get_info<T>().ref != LUA_REFNIL;
		}

		static constexpr int kMaxDependencies = 256;

		struct SwitchUpValueGuard {
			/**
			 * Change and restore lua_upvalueindex(n) with the value at the top of the stack.
			 * Pops the value from the top of the stack
			 */
			SwitchUpValueGuard(lua_State* L, int n);
			~SwitchUpValueGuard();

			// The compiler-generated copy constructor just copies the members, so it wouldn't call the constructor,
			// therefore making the destructor operations invalid.
			// More over, this prevents the compiler from generating a move constructor or move assignment.
			// Move will still call the destructor without calling the constructor, therefore making the destructor operations invalid.
			// The compiler-generated copy assignment has the same problem, it just copies the members, so it wouldn't call the constructor.
			SwitchUpValueGuard(const SwitchUpValueGuard&) = delete;
			SwitchUpValueGuard& operator=(const SwitchUpValueGuard&) = delete;

			lua_State* L_;
			int n_;
			int ref_ = LUA_REFNIL;
		};

		/**
		 * Add the value at the top of the stack as a dependency and pop the value from the top the stack.
		 * @param  L the lua_State
		 * @return   the dependency id
		 */
		int add_dependecy(lua_State* L);

		/**
		 * Switch value at upvalue index n with the dependency id
		 * @param  L  the lua_State
		 * @param  n  the upvalue index
		 * @param  id the dependency id
		 * @return    The guard object. When destroyed, restores the origin upvalue
		 */
		SwitchUpValueGuard switch_dependency(lua_State* L, int n, int id);

	private:
		template<typename T>
		MetatableInfo& get_info() {
			static auto class_id = [this] () {
				std::unique_lock lock(mutex);
				return g_class_id++;
			} ();

			if (class_id >= classes.size()) {
				classes.resize(class_id + 1);
			}

			return classes.at(class_id);
		}

		static std::mutex mutex;
		static unsigned short g_class_id;

		// pre allocate 512 placeholders for classes
		std::vector<MetatableInfo> classes{512};

		int g_dep_id = 0;
		int dependencies[kMaxDependencies] = {};
	};

	void lua_module_push_context(lua_State* L);
	std::shared_ptr<LuaStateContext> get_context_from_registry(lua_State* L);
	std::shared_ptr<LuaStateContext> get_context_from_upvalue(lua_State* L);


	// ================================
	// CallInfo
	// ================================

	// Pushes message, prefixed with the location of the Lua caller ("file:line: ").
	void lua_module_push_error(lua_State* L, const char* message);

	template <int (*fn)(lua_State*)>
	int lua_mocule_pcall_cfunction(lua_State* L) {
		auto top = lua_gettop(L);
		try {
			return fn(L);
		} catch (const std::exception& e) {
			lua_settop(L, top);
			lua_module_push_error(L, e.what());
		}
		return -1;
	}

	template <int (*fn)(lua_State*)>
	int lua_mocule_call_cfunction(lua_State* L) {
		const int n = lua_mocule_pcall_cfunction<fn>(L);
		return n >= 0 ? n : lua_error(L);
	}


	// ================================
	// type traits
	// ================================
	// Source - https://stackoverflow.com/questions/9851594/standard-c11-way-to-remove-all-pointers-of-a-type
	// Posted by klaus triendl
	// Retrieved 05/11/2025, License - CC-BY-SA 4.0
	template<typename T>
	struct remove_all_pointers : std::conditional_t<
		std::is_pointer_v<T>,
		remove_all_pointers<
		std::remove_pointer_t<T>
		>,
		std::type_identity<T>
	> {};

	template<typename T>
	using remove_all_pointers_t = typename remove_all_pointers<T>::type;

	template<typename T>
	using remove_cvref_all_pointers_t = std::remove_cvref_t<remove_all_pointers_t<T>>;

	template<bool B, typename T, typename V>
	struct enable_if_else { using type = V; };

	template<typename T, typename V>
	struct enable_if_else<true, T, V> { using type = T; };

	template< bool B, typename T, typename V >
	using enable_if_else_t = typename enable_if_else<B, T, V>::type;

	template<typename, typename = void>
	constexpr bool is_type_complete_v = false;

	template<typename T>
	constexpr bool is_type_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;

	template<typename T, typename = void>
	struct lua_to_custom_bridge {
		static constexpr bool value = false;
		static T lua_to(lua_State* L, int index, bool& is_valid);
	};

	// https://devblogs.microsoft.com/oldnewthing/20190710-00/?p=102678
	// Detecting in C++ whether a type is defined, part 3: SFINAE and incomplete types
	// Posted by Raymond Chen
	template<typename, typename = void>
	constexpr bool has_lua_to_custom_bridge_v = false;

	template<typename T>
	constexpr bool has_lua_to_custom_bridge_v<T, void> = lua_to_custom_bridge<T>::value;


	// ================================
	// reference_internal generics
	// ================================

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(_Tp* element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, element);
	}

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(const _Tp* element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, const_cast<_Tp*>(element));
	}

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(_Tp& element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, &element);
	}

	template<typename _Tp, typename shared_ptr = std::shared_ptr<_Tp>>
	inline decltype(auto) reference_internal(const _Tp& element, shared_ptr* ptr = static_cast<shared_ptr*>(nullptr)) {
		return shared_ptr(shared_ptr{}, const_cast<_Tp*>(&element));
	}


	// ================================
	// lua_reference_push
	// ================================

	template<typename T>
	std::enable_if_t<is_usertype_v<T>> lua_reference_push(lua_State* L, T& value);

	template<typename T>
	std::enable_if_t<!is_usertype_v<T>&& is_instantiation_of_v<std::optional, T>> lua_reference_push(lua_State* L, T& value);

	template<typename T>
	std::enable_if_t<!is_usertype_v<T> && !is_instantiation_of_v<std::optional, T>> lua_reference_push(lua_State* L, T& value);

	template<typename T>
	std::enable_if_t<!std::is_reference_v<T>> lua_reference_push(lua_State* L, const T& value);


	// ================================
	// extract_holder
	// ================================

	template<typename T, typename V>
	struct has_extract_holder : std::false_type {};

	template<typename T, typename V>
	constexpr bool has_extract_holder_v = has_extract_holder<T, V>::value;

	template<typename T, typename V>
	struct extract_holder_info;


	// ================================
	// bool
	// ================================

	bool lua_to(lua_State* L, int index, bool*, bool& is_valid);

	// Avoid implicit conversion of T* to bool
	template<typename T>
	inline std::enable_if_t<
		std::is_same_v<std::remove_cvref_t<std::decay_t<T>>, bool>
		|| std::is_same_v<T, std::vector<bool>::reference>
		|| std::is_same_v<T, std::vector<bool>::const_reference>
		, void> lua_push(lua_State* L, T value) {
		lua_pushboolean(L, value);
	}


	// ================================
	// std::integral
	// ================================

	template<typename T>
	inline std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<bool, std::decay_t<T>>, std::decay_t<T>> lua_to(lua_State* L, int index, T* ptr, bool& is_valid);

	template<typename T>
	inline std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<bool, std::decay_t<T>>, void> lua_push(lua_State* L, T n) {
		using Integer = std::decay_t<decltype(n)>;
		if constexpr (std::is_same_v<bool, Integer>) {
			lua_pushboolean(L, n);
		}
		else {
#if LUA_VERSION_NUM >= 503
			// Lua 5.3 and greater checks for numeric precision
			lua_pushinteger(L, n);
#else
			lua_pushnumber(L, (lua_Number) n);
#endif
		}
	}


	// ================================
	// std::floating_point
	// ================================

	template<typename T>
	inline std::enable_if_t<std::is_floating_point_v<T>, std::decay_t<T>> lua_to(lua_State* L, int index, T* ptr, bool& is_valid);

	inline void lua_push(lua_State* L, std::floating_point auto n) {
		lua_pushnumber(L, n);
	}


	// ================================
	// const char*
	// ================================

	const char* lua_to(lua_State* L, int index, const char**, bool& is_valid);

	template<std::size_t N>
	inline void lua_push(lua_State* L, const char(&c_str)[N]) {
		lua_pushlstring(L, c_str, N);
	}

	inline void lua_push(lua_State* L, const char* c_str) {
		lua_pushstring(L, c_str);
	}


	// ================================
	// std::string
	// ================================

	inline std::string lua_to(lua_State* L, int index, std::string*, bool& is_valid);

	inline void lua_push(lua_State* L, const std::string& str) {
		lua_pushlstring(L, str.c_str(), str.size());
	}

#ifdef _MSC_VER
	inline std::wstring lua_to(lua_State* L, int index, std::wstring*, bool& is_valid);

	inline void lua_push(lua_State* L, const std::wstring& wstr) {
		std::string str; wide_char::wcs_to_utf8(wstr, str);
		lua_push(L, str);
	}
#endif


	// ================================
	// std::string_view
	// ================================

	inline void lua_push(lua_State* L, const std::string_view& str) {
		lua_pushlstring(L, str.data(), str.size());
	}


	// ================================
	// void*
	// ================================

	void* lua_to(lua_State* L, int index, void**, bool& is_valid);

	inline void lua_push(lua_State* L, void* ptr) {
		if (ptr) {
			lua_pushlightuserdata(L, ptr);
		}
		else {
			lua_pushnil(L);
		}
	}

	inline void lua_push(lua_State* L, const void* ptr) {
		lua_push(L, const_cast<void*>(ptr));
	}


	// ================================
	// _Object
	// ================================

	template<int Kind>
	struct _Object;

	template<int Kind>
	inline bool lua_is_object(lua_State* L, int index, _Object<Kind>*);

	template<int Kind>
	struct _Object {
		_Object() = default;
		~_Object() {
			reset();
		}

		_Object(lua_State* L, int index, bool& is_valid) {
			is_valid = lua_is_object(L, index, static_cast<_Object<Kind>*>(nullptr));
			init(L, index);
		}

		_Object(const _Object& other) {
			*this = other;
		}

		_Object(lua_State* L, const _Object& other) {
			assign(L, other);
		}

		template<typename T>
		_Object(lua_State* L, const T& any);

		void init(lua_State* L_, int index) {
			L = L_;
			if (L != nullptr) {
				lua_pushvalue(L, index);
				ref = luaL_ref(L, LUA_REGISTRYINDEX);
			}
		}

		_Object& operator=(const _Object& other) {
			// Guard self assignment
			if (this == &other) {
				return *this;
			}

			reset();

			if (other.L != nullptr) {
				L = other.L;
				lua_rawgeti(other.L, LUA_REGISTRYINDEX, other.ref);
				ref = luaL_ref(other.L, LUA_REGISTRYINDEX);
			}

			return *this;
		}

		_Object(_Object&& other) noexcept {
			*this = std::move(other);
		}

		_Object& operator=(_Object&& other) noexcept {
			// Guard self assignment
			if (this == &other) {
				return *this;
			}

			reset();

			L = std::exchange(other.L, nullptr); // leave other in valid state
			ref = std::exchange(other.ref, LUA_REFNIL);
			return *this;
		}

		void reset() {
			if (ref != LUA_REFNIL) {
				luaL_unref(L, LUA_REGISTRYINDEX, ref);
				free();
			}
		}

		void free() {
			L = nullptr;
			ref = LUA_REFNIL;
		}

		void assign(lua_State* L, const _Object& other) {
			reset();

			if (L != nullptr) {
				this->L = L;
				lua_push(L, other);
				ref = luaL_ref(L, LUA_REGISTRYINDEX);
			}
		}

		const bool isnil() const;

		inline bool operator==(const _Object& rhs) const {
			return L == rhs.L && ref == rhs.ref;
		}

		inline bool operator!=(const _Object& rhs) {
			return !(*this == rhs);
		}

		lua_State* L = nullptr;
		int ref = LUA_REFNIL;
	};

	using Object = _Object<0>;
	template<>
	inline bool lua_is_object<0>(lua_State* L, int index, Object*) {
		index = lua_absindex(L, index);
		return lua_gettop(L) >= index;
	}

	using Table = _Object<1>;
	template<>
	inline bool lua_is_object<1>(lua_State* L, int index, Table*) {
		return lua_istable(L, index);
	}

	using Function = _Object<2>;
	template<>
	inline bool lua_is_object<2>(lua_State* L, int index, Function*) {
		return lua_isfunction(L, index);
	}

	const Object lua_nil;

	struct PushGuard {
		lua_State* L;

		template<typename T>
		PushGuard(lua_State* L, const T& any);

		~PushGuard() {
			lua_pop(L, 1);
		}
	};

	template<int Kind>
	inline _Object<Kind> lua_to(lua_State* L, int index, _Object<Kind>*, bool& is_valid) {
		return _Object<Kind>(L, index, is_valid);
	}

	template<int Kind>
	inline void lua_push(lua_State* L, const _Object<Kind>& o) {
		if (o.L == nullptr) {
			lua_pushnil(L);
			return;
		}

		lua_rawgeti(o.L, LUA_REGISTRYINDEX, o.ref);

		if (o.L != L) {
			lua_xmove(o.L, L, 1);
		}
	}

	template<int Kind>
	inline void lua_push(lua_State* L, const _Object<Kind>* o) {
		lua_push(L, *o);
	}

	// ================================
	// T
	// ================================

	template<typename T>
	inline void lua_to_by_ref(lua_State* L, int index, T*, T*& out, bool& is_valid);

	template<typename T>
	inline std::shared_ptr<T> lua_userdata_to(lua_State* L, int index, T*, bool& is_valid);

	template<typename T>
	struct CFunctionDeleter {
		using Deleter = void (*)(T*);

		CFunctionDeleter(Deleter d) : d(d) {}

		void operator()(T* obj) {
			d(obj);
		}

		Deleter d;
	};

	template<typename T>
	inline void lua_push(lua_State* L, T* ptr, void (*d)(T*));

	// ================================
	// T[]
	// ================================

	template<typename T, size_t __sizeof>
	bool runtime_register_class(lua_State* L);

	template<typename T>
	struct PointerArray {
		static void register_class(lua_State* L);
		static int __index(lua_State* L);
		static int __newindex(lua_State* L);
		static int __add(lua_State* L);
		static int __sub(lua_State* L);
		static int __lt(lua_State* L);
		static int __le(lua_State* L);

		PointerArray(T* data) : data(data) {}
		virtual ~PointerArray() = default;

		T& operator[](size_t i) {
			return this->data[i];
		}

		const T& operator[](size_t i) const {
			return this->data[i];
		}

		T* data;
	};

	template<typename T>
	struct is_usertype<PointerArray<T>> : std::true_type {};

	template<typename T>
	struct usertype_info<PointerArray<T>> {
		static std::mutex mutex;
		static const struct luaL_Reg methods[];
		static const struct luaL_Reg meta_methods[];
		static std::shared_ptr<PointerArray<T>> lua_userdata_to(lua_State* L, int index, bool& is_valid);
	};


	// ================================
	// T if std::is_enum_v<T>
	// ================================

	template<typename T>
	inline std::enable_if_t<std::is_enum_v<T>, int> lua_to(lua_State* L, int index, T* ptr, bool& is_valid);

	template<typename T>
	inline std::enable_if_t<std::is_enum_v<T>, void> lua_push(lua_State* L, const T& value) {
		lua_push(L, static_cast<int>(value));
	}


	// ================================
	// T if is_usertype_v<T>
	// ================================

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, std::shared_ptr<T>> lua_to(lua_State* L, int index, std::shared_ptr<T>*, bool& is_valid);

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, std::shared_ptr<T>> lua_to(lua_State* L, int index, T* ptr, bool& is_valid);

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, T*> lua_to(lua_State* L, int index, T**, bool& is_valid);

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, T* ptr);

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, const T* ptr);

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, T&& obj);

	template<typename T>
	inline std::enable_if_t<is_usertype_v<T>, void> lua_push(lua_State* L, const T& obj);


	// ================================
	// T if !is_usertype_v<remove_cvref_all_pointers_t<T>
	// ================================

	template<typename T>
	inline std::enable_if_t<!std::is_function_v<T> && !is_usertype_v<remove_cvref_all_pointers_t<T>> && !std::is_same_v<remove_cvref_all_pointers_t<T>, void>, T*> lua_to(lua_State* L, int index, T**, bool& is_valid);

	template<typename T>
	inline std::enable_if_t<!std::is_function_v<T> && !is_usertype_v<remove_cvref_all_pointers_t<T>> && !std::is_same_v<remove_cvref_all_pointers_t<T>, void>, void> lua_push(lua_State* L, T* ptr) {
		lua_push(L, static_cast<void*>(ptr));
	}

	template<typename R, typename... Args>
	inline void lua_push(lua_State* L, R(*fn)(Args...)) {
		lua_pushlightuserdata(L, reinterpret_cast<void*>(fn));
	}

	// ================================
	// T** as void*
	// ================================

	template<typename T>
	inline T** lua_to(lua_State* L, int index, T***, bool& is_valid);

	template<typename T>
	inline void lua_push(lua_State* L, T** ptr) {
		lua_push(L, static_cast<void*>(ptr));
	}


	// ================================
	// std::shared_ptr
	// ================================

	template<typename T>
	inline std::enable_if_t<!is_usertype_v<T>, std::shared_ptr<T>> lua_to(lua_State* L, int index, std::shared_ptr<T>*, bool& is_valid);

	template<typename T>
	inline void lua_push(lua_State* L, const std::shared_ptr<T>& ptr);


	// ================================
	// stl map container
	// ================================

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline void _stl_map_container_lua_to(lua_State* L, int index, Container<K, V, _Ts...>& out, bool& is_valid);

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline std::shared_ptr<Container<K, V, _Ts...>> _stl_map_container_lua_to(lua_State* L, int index, Container<K, V, _Ts...>* ptr, bool& is_valid);

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline void _stl_map_container_lua_push(lua_State* L, Container<K, V, _Ts...>&& kv);

	template<template<typename, typename, typename...> typename Container, typename K, typename V, typename... _Ts>
	inline void _stl_map_container_lua_push(lua_State* L, const Container<K, V, _Ts...>& kv);


	// ================================
	// std::map
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::map<K, V, _Ts...>& out, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::map<K, V, _Ts...>> lua_to(lua_State* L, int index, std::map<K, V, _Ts...>* ptr, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::map<K, V, _Ts...>&& kv);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::map<K, V, _Ts...>& kv);


	// ================================
	// std::multimap
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::multimap<K, V, _Ts...>& out, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::multimap<K, V, _Ts...>> lua_to(lua_State* L, int index, std::multimap<K, V, _Ts...>* ptr, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::multimap<K, V, _Ts...>&& kv);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::multimap<K, V, _Ts...>& kv);


	// ================================
	// std::unordered_map
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::unordered_map<K, V, _Ts...>& out, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::unordered_map<K, V, _Ts...>> lua_to(lua_State* L, int index, std::unordered_map<K, V, _Ts...>* ptr, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::unordered_map<K, V, _Ts...>&& kv);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::unordered_map<K, V, _Ts...>& kv);


	// ================================
	// std::unordered_multimap
	// ================================

	template<typename K, typename V, typename... _Ts>
	inline void lua_to(lua_State* L, int index, std::unordered_multimap<K, V, _Ts...>& out, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline std::shared_ptr<std::unordered_multimap<K, V, _Ts...>> lua_to(lua_State* L, int index, std::unordered_multimap<K, V, _Ts...>* ptr, bool& is_valid);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, std::unordered_multimap<K, V, _Ts...>&& kv);

	template<typename K, typename V, typename... _Ts>
	inline void lua_push(lua_State* L, const std::unordered_multimap<K, V, _Ts...>& kv);


	// ================================
	// std::optional
	// ================================

	template<typename T>
	inline std::shared_ptr<std::optional<T>> lua_to(lua_State* L, int index, std::optional<T>*, bool& is_valid);

	template<typename T>
	inline void lua_push(lua_State* L, const std::optional<T>& p);


	// ================================
	// std::pair
	// ================================

	template<typename T1, typename T2>
	inline std::shared_ptr<std::pair<T1, T2>> lua_to(lua_State* L, int index, std::pair<T1, T2>* ptr, bool& is_valid);

	template<typename T1, typename T2>
	inline void lua_push(lua_State* L, const std::pair<T1, T2>& p);


	// ================================
	// std::tuple
	// ================================

	template<typename... _Ts>
	inline std::shared_ptr<std::tuple<_Ts...>> lua_to(lua_State* L, int index, std::tuple<_Ts...>* ptr, bool& is_valid);

	template<typename... _Ts>
	inline void lua_push(lua_State* L, const std::tuple<_Ts...>& value);


	// ================================
	// std::variant
	// ================================

	template<typename... _Ts>
	inline std::variant<_Ts...> lua_to(lua_State* L, int index, std::variant<_Ts...>* ptr, bool& is_valid);

	template<typename... _Ts>
	inline void lua_push(lua_State* L, const std::variant<_Ts...>& value);


	// ================================
	// stl container
	// ================================

	template<template<typename...> typename Container, typename... _Ts>
	inline void _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>& container, bool& is_valid, size_t len, bool loose);

	template<template<typename...> typename Container, typename... _Ts>
	inline std::shared_ptr<Container<_Ts...>> _stl_container_lua_to(lua_State* L, int index, Container<_Ts...>* ptr, bool& is_valid, size_t len, bool loose);

	template<template<typename...> typename Container, typename... _Ts>
	inline void _stl_container_lua_push(lua_State* L, Container<_Ts...>&& container);

	template<template<typename...> typename Container, typename... _Ts>
	inline void _stl_container_lua_push(lua_State* L, const Container<_Ts...>& container);


	// ================================
	// std::vector
	// ================================

	template<class T, class Allocator = std::allocator<T>>
	inline void lua_to(lua_State* L, int index, std::vector<T, Allocator>& vec, bool& is_valid, size_t len = 0, bool loose = false);

	template<class T, class Allocator = std::allocator<T>>
	inline std::shared_ptr<std::vector<T, Allocator>> lua_to(lua_State* L, int index, std::vector<T, Allocator>* ptr, bool& is_valid, size_t len = 0, bool loose = false);

	template<class T, class Allocator = std::allocator<T>>
	inline void lua_push(lua_State* L, std::vector<T, Allocator>&& vec);

	template<class T, class Allocator = std::allocator<T>>
	inline void lua_push(lua_State* L, const std::vector<T, Allocator>& vec);


	// ================================
	// std::function
	// ================================

	template<class R, class... Args>
	inline std::function<R(Args...)> lua_to(lua_State* L, int index, std::function<R(Args...)>*, bool& is_valid);


	// ================================
	// misc functions
	// ================================

	template<typename T, typename... _Ts>
	inline void lua_register_class(lua_State* L, const char* name);

	void lua_module_push_cfunction(lua_State *L, lua_CFunction fn);

	void lua_module_setfuncs(lua_State *L, const luaL_Reg *l);

	void lua_module_setfuncs_if_not_exists(lua_State* L, const luaL_Reg* l);

	template<typename T>
	inline void lua_register_defaults(lua_State* L);

	int try_mt__index(lua_State* L);

	int lua_missing_declaration(lua_State* L);

	bool lua_newkwargs_from_table(lua_State* L, int index, bool& is_valid);

	// ================================
	// __eq__
	// ================================

	template<typename T>
	inline bool __eq__(const T& o1, const T& o2);

	template<typename T>
	inline bool __eq__(const std::shared_ptr<T>& p1, const std::shared_ptr<T>& p2);

	template<typename K, typename V>
	inline bool __eq__(const std::map<K, V>& m1, const std::map<K, V>& m2);

	template<typename T1, typename T2>
	inline bool __eq__(const std::pair<T1, T2>& p1, const std::pair<T1, T2>& p2);

	template<class T, class Allocator = std::allocator<T>>
	inline bool __eq__(const std::vector<T, Allocator>& v1, const std::vector<T, Allocator>& v2);

	// ================================
}

LUAAPI(int) LUA_MODULE_LUAOPEN(lua_State* L);
