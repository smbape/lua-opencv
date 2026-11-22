#include <lua_bridge_common.hpp>

#define luaB_require(L, modname) _luaB_require(L, "" modname, sizeof(modname) / sizeof(char) - 1)

namespace {
	int _luaB_require(lua_State* L, const char* modname, size_t len) {
		lua_getglobal(L, "require");
		if (lua_isnil(L, -1)) {
			luaL_error(L, "global variable 'require' wast not found");
		}

		lua_pushlstring(L, modname, len);
		lua_call(L, 1, 1);

		return lua_gettop(L);
	}

	void* lua_cdata_to_pointer(lua_State* L, int index, bool& is_valid) {
		/* working with luajit 2.x but may not work when luajit updates to 3.0 */
		is_valid = lua_type(L, index) == LUA_TCDATA;
		if (!is_valid) {
			return nullptr;
		}

		index = lua_absindex(L, index);

		is_valid = false;

		auto top = lua_gettop(L);
		intptr_t ptr = 0;
		uint8_t* bytes = reinterpret_cast<uint8_t*>(&ptr);
		size_t N;

		// call cdata_utils.get_cdata_bytes(cdata)
		luaB_require(L, LUA_MODULE_NAME_STR ".cdata_utils.get_cdata_bytes");
		lua_pushvalue(L, index);
		if (lua_pcall(L, 1, LUA_MULTRET, 0) != 0) {
			goto invalid;
		}

		N = lua_gettop(L) - top;
		if (N == 0) {
			goto invalid;
		}

		for (auto i = 1; i <= N; ++i) {
			bytes[i - 1] = static_cast<uint8_t>(lua_tonumber(L, top + i));
		}

		is_valid = true;
		lua_settop(L, top);
		return reinterpret_cast<void*>(ptr);

	invalid:
		if (top < lua_gettop(L)) {
			lua_settop(L, top);
		}

		return nullptr;
	}

	void* lua_self_pointer(lua_State* L, int index, bool& is_valid) {
		void* ptr = nullptr;

		index = lua_absindex(L, index);

		is_valid = (lua_isuserdata(L, index) || lua_istable(L, index)) && lua_getmetatable(L, index); // push metatable
		if (!is_valid) {
			return ptr;
		}

		lua_pushliteral(L, "__has_self");
		lua_rawget(L, -2); // push metatable.__has_self
		is_valid = lua_isboolean(L, -1) && lua_toboolean(L, -1);
		lua_pop(L, 2); // pop metatable.__has_self, metatable

		if (!is_valid) {
			return ptr;
		}

		lua_pushliteral(L, "__self");
		lua_gettable(L, index); // push userdata.__self
		is_valid = lua_islightuserdata(L, -1);

		if (is_valid) {
			ptr = lua_touserdata(L, -1);
			is_valid = static_cast<bool>(ptr);
		}

		lua_pop(L, 1); // pop userdata.__self
		return ptr;
	}

	void lua_module_setfunc(lua_State* L, const luaL_Reg* l, int nup) {
		lua_pushstring(L, l->name);
		for (int i = 0; i < nup; ++i) {  /* copy upvalues to the top */
			lua_pushvalue(L, -(nup + 1));
		}
		lua_pushcclosure(L, l->func, nup);  /* closure with those upvalues */
		lua_rawset(L, -(nup + 3));
	}
}

namespace LUA_MODULE_NAME {


	// ================================
	// bool
	// ================================

	bool lua_to(lua_State* L, int index, bool*, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<bool>) {
			const auto v = lua_to_custom_bridge<bool>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isboolean(L, index);
		return is_valid && !!lua_toboolean(L, index);
	}


	// ================================
	// const char*
	// ================================

	const char* lua_to(lua_State* L, int index, const char**, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<const char*>) {
			const auto v = lua_to_custom_bridge<const char*>::lua_to(L, index, is_valid);
			if (is_valid) {
				return v;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return nullptr;
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			return static_cast<const char*>(lua_touserdata(L, index));
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (!is_valid) {
			return nullptr;
		}

		size_t len;
		auto c_str = lua_tolstring(L, index, &len);
		return c_str;
	}


	// ================================
	// void*
	// ================================

	void* lua_to(lua_State* L, int index, void**, bool& is_valid) {
		if constexpr (has_lua_to_custom_bridge_v<void*>) {
			const auto ptr = lua_to_custom_bridge<void*>::lua_to(L, index, is_valid);
			if (is_valid) {
				return ptr;
			}
		}

		is_valid = lua_isnil(L, index);
		if (is_valid) {
			return nullptr;
		}

		is_valid = lua_type(L, index) == LUA_TSTRING;
		if (is_valid) {
			size_t len;
			auto c_str = lua_tolstring(L, index, &len);
			return const_cast<char*>(c_str);
		}

		is_valid = lua_islightuserdata(L, index);
		if (is_valid) {
			return lua_touserdata(L, index);
		}

		void* ptr;

		ptr = lua_cdata_to_pointer(L, index, is_valid);
		if (is_valid) {
			return ptr;
		}

		ptr = lua_self_pointer(L, index, is_valid);
		if (is_valid) {
			return ptr;
		}

		return nullptr;
	}


	// ================================
	// misc functions
	// ================================

	void lua_module_push_cfunction(lua_State* L, lua_CFunction fn) {
		lua_module_push_context(L); // push upvalue
		lua_pushcclosure(L, fn, 1); // push c function with upvalue
	}

	void lua_module_setfuncs(lua_State* L, const luaL_Reg* l) {
		int tbl = lua_gettop(L);
		lua_module_push_context(L); // push upvalue
		int nup = lua_gettop(L) - tbl;

		for (; l->name != nullptr; ++l) {  /* fill the table with given functions */
			lua_module_setfunc(L, l, nup);
		}

		if (nup > 0) {
			lua_pop(L, nup);  /* remove upvalues */
		}
	}

	void lua_module_setfuncs_if_not_exists(lua_State* L, const luaL_Reg* l) {
		int tbl = lua_gettop(L);
		lua_module_push_context(L); // push upvalue
		int nup = lua_gettop(L) - tbl;

		for (; l->name != nullptr; ++l) {  /* fill the table with given functions */
			lua_pushstring(L, l->name);
			lua_rawget(L, tbl);
			bool exists = !lua_isnil(L, -1);
			lua_pop(L, 1);

			if (!exists) {
				lua_module_setfunc(L, l, nup);
			}
		}

		if (nup > 0) {
			lua_pop(L, nup);  /* remove upvalues */
		}
	}

	int try_mt__index(lua_State* L) {
		if (lua_getmetatable(L, 1)) {
			lua_pushvalue(L, 2); // push the key
			lua_rawget(L, -2);
			lua_remove(L, -2); // remove the metatable

			if (!lua_isnil(L, -1)) {
				return 1; // return metatable[key]
			}

			lua_pop(L, 1); // pop nil
		}

		return 0;
	}

	int lua_missing_declaration(lua_State* L) {
		const auto arg = 2;
		char const* sname;
		if (lua_type(L, arg) == LUA_TSTRING) {
			sname = lua_tostring(L, arg);
		}
		else if (lua_type(L, arg) == LUA_TLIGHTUSERDATA) {
			sname = "light userdata";  /* special name for messages */
		}
		else {
			sname = luaL_typename(L, arg);  /* standard name */
		}
		luaL_error(L, "missing declaration for symbol '%s'", sname);
		return -1;
	}

	bool lua_newkwargs_from_table(lua_State* L, int index, bool& is_valid) {
		is_valid = lua_istable(L, index);
		if (!is_valid) {
			return is_valid;
		}

		if (lua_getmetatable(L, index)) {
			// table already has a metatable
			lua_pop(L, 1);
			is_valid = false;
			return is_valid;
		}

		index = lua_absindex(L, index);

		lua_newtable(L);
		// stack now contains: -1 => kwargs

		const auto kwargs_index = lua_gettop(L);
		const auto __top__ = kwargs_index + 1;

		// https://www.lua.org/manual/5.1/manual.html#lua_next

		bool remove_first_key = true;

		lua_pushnil(L);  /* first key */
		// stack now contains: -2 => kwargs; -1 => nil

		while (lua_next(L, index) != 0) {
			remove_first_key = false;

			// stack now contains: -3 => kwargs; -2 => key; -1 => value
			const auto key = lua_to(L, -2, static_cast<std::string*>(nullptr), is_valid);

			if (!is_valid) {
				/* removes 'value'; keeps 'key' for the next iteration */
				lua_pop(L, 1);
				// stack now contains: -2 => kwargs; -1 => key

				remove_first_key = true;
				break;
			}

			const auto k = __top__ - 2;
			const auto v = __top__ - 1;
			lua_pushvalue(L, k);
			lua_pushvalue(L, v);
			lua_rawset(L, kwargs_index);

			/* removes 'value'; keeps 'key' for the next iteration */
			lua_pop(L, 1);
			// stack now contains: -2 => kwargs; -1 => key
		}

		if (remove_first_key) {
			lua_pop(L, 1);
			// stack now contains: -1 => kwargs
		}

		if (is_valid) {
			get_context_from_upvalue(L)->push_metatable<Keywords>(L);
			lua_setmetatable(L, -2);
		}

		return is_valid;
	}
}

#define LUA_GIL_LOCK_UNIQUE_NAME "__lua_brige_gil__"

namespace {
	using namespace LUA_MODULE_NAME;

	struct HeldGils {
		~HeldGils() {
			const auto this_thread_id = std::this_thread::get_id();
			for (auto& w : held) {
				if (auto gil = w.lock()) {
					gil->release_if_owned_by(this_thread_id);
				}
			}
		}

		void hold(const GilPtr& gil) {
			std::erase_if(held, [](auto& w) {
				return w.expired();
			});
			held.push_back(gil);
		}

		std::vector<std::weak_ptr<Gil>> held;
	};

	thread_local HeldGils held_gils;

	template<typename T>
	int destructor__gc(lua_State* L) {
		auto object = static_cast<T*>(lua_touserdata(L, 1));
		object->~T();
		return 0;
	}

	// Pushes a new userdata holding T(args...), tagged with class_id. Leaves it on the stack.
	// The caller sets its metatable, whose __gc must be gc<T, class_id>.
	template <class T, int (*gc)(lua_State*), class... Args>
	T* push_newuserdata_with_gc(lua_State* L, Args&&... args) {
		auto object = static_cast<T*>(lua_compat_newuserdata(L, sizeof(T)));
		new (object) T(std::forward<Args>(args)...);

		lua_newtable(L);
		lua_pushstring(L, "__gc");
		lua_pushcfunction(L, gc);
		lua_rawset(L, -3);

		lua_setmetatable(L, -2);

		return object;
	}

	// Pushes a new userdata holding T(args...), tagged with class_id. Leaves it on the stack.
	// The caller sets its metatable, whose __gc must be gc<T, class_id>.
	template <class T, class... Args>
	T* push_newuserdata_with_destructor_gc(lua_State* L, Args&&... args) {
		return push_newuserdata_with_gc<T, destructor__gc<T>>(L, std::forward<Args>(args)...);
	}

	int gil__gc(lua_State* L) {
		auto* object = static_cast<GilPtr*>(lua_touserdata(L, 1));
		(*object)->close();
		object->~GilPtr();
		return 0;
	}

	GilPtr create_gil_and_lock(lua_State* L) {
		lua_pushliteral(L, LUA_GIL_LOCK_UNIQUE_NAME);
		auto gil = push_newuserdata_with_gc<GilPtr, gil__gc>(L, std::make_shared<Gil>());
		lua_rawset(L, LUA_REGISTRYINDEX);

		(*gil)->lock();
		held_gils.hold(*gil);
		return *gil;
	}

	const char kContextKey[] = "module.context";

	void create_context(lua_State* L) {
		auto context = push_newuserdata_with_destructor_gc<std::shared_ptr<LuaStateContext>>(L, std::make_shared<LuaStateContext>());
		lua_rawsetp(L, LUA_REGISTRYINDEX, kContextKey);
	}

	void register_Callbacks(lua_State* L) {
		const struct luaL_Reg callbacks_funcs[] = {
			{ "notifyCallbacks", yield },
			{ "yield", yield },
			{ NULL, NULL }
		};
		lua_module_setfuncs(L, callbacks_funcs);
	}

	int luaB_tolighuserdata(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc == 0) {
			return luaL_error(L, "self is not defined");
		}

		if (vargc != 1) {
			return luaL_error(L, "too many arguments");
		}

		bool is_valid = false;
		auto ptr = lua_to(L, 1, static_cast<void**>(nullptr), is_valid);

		if (!is_valid) {
			luaL_typeerror(L, 1, "void*");
		}

		lua_push(L, ptr);
		return 1;
	}

	void register_GetSelf(lua_State* L) {
		lua_pushliteral(L, "__self");
		lua_pushcfunction(L, luaB_tolighuserdata);
		lua_rawset(L, -3);

		lua_pushliteral(L, "tolightuserdata");
		lua_pushcfunction(L, luaB_tolighuserdata);
		lua_rawset(L, -3);
	}

	int luaB_createtable(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc > 2) {
			return luaL_error(L, "too many arguments");
		}

		bool is_valid = false;

		int narr = 0;
		if (vargc >= 1) {
			narr = lua_to(L, 1, static_cast<int*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_typeerror(L, 1, "int");
			}
		}

		int nrec = 0;
		if (vargc >= 2) {
			nrec = lua_to(L, 2, static_cast<int*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_typeerror(L, 2, "int");
			}
		}

		lua_createtable(L, narr, nrec);
		return 1;

	}

	void register_CreateTable(lua_State* L) {
		lua_pushliteral(L, "createtable");
		lua_pushcfunction(L, luaB_createtable);
		lua_rawset(L, -3);
	}

	void lua_push_this_thread_id_key(lua_State* L) {
		thread_local std::string thread_id_key = []() {
			std::ostringstream id_key(LUA_GIL_LOCK_UNIQUE_NAME);
			id_key << std::this_thread::get_id();
			return id_key.str();
			} ();
		lua_pushlstring(L, thread_id_key.c_str(), thread_id_key.size());
	}

	struct GilHolder {
		std::shared_ptr<std::unique_lock<std::mutex>> gil;
		std::shared_ptr<std::mutex> gil_mutex;
	};

	void gil_create_and_push(lua_State* L, std::shared_ptr<std::mutex> gil_mutex) {
		lua_push_this_thread_id_key(L);
		push_newuserdata_with_destructor_gc<std::shared_ptr<GilHolder>>(L, std::make_shared<GilHolder>(GilHolder{
			.gil = std::make_shared<std::unique_lock<std::mutex>>(*gil_mutex, std::defer_lock),
			.gil_mutex = gil_mutex
			}));
		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_push_this_thread_id_key(L);
		lua_rawget(L, LUA_REGISTRYINDEX);
	}

	class MinArgError : public CallInfo::Error {
	public:
		MinArgError(int actual, int expected) : actual_(actual), expected_(expected) {
			// Nothing to do
		}

		void insert_into(std::ostringstream& buffer) const override {
			buffer << "expected at least " << expected_ << " arguments, got " << actual_;
		}
	private:
		int actual_;
		int expected_;
	};

	class MaxArgError : public CallInfo::Error {
	public:
		MaxArgError(int actual, int expected) : actual_(actual), expected_(expected) {
			// Nothing to do
		}

		void insert_into(std::ostringstream& buffer) const override {
			buffer << "expected at most " << expected_ << " arguments, got " << actual_;
		}
	private:
		int actual_;
		int expected_;
	};

}

namespace LUA_MODULE_NAME {
	void register_Common(lua_State* L) {
		get_gil_of(L);
		create_context(L);
		register_Keywords(L);
		register_Callbacks(L);
		register_GetSelf(L);
		register_CreateTable(L);
	}

	int Gil::lock_unsafe(std::unique_lock<std::mutex>& lk, int locks) {
		if (locks <= 0) {
			throw std::runtime_error("number of locks must be greater than 0");
		}

		const auto this_thread_id = std::this_thread::get_id();
		if (owner_ == this_thread_id) {
			locks_ += locks;
			return locks_;
		}

		cv_.wait(lk, [this] {
			return closed_ || owner_ == std::thread::id{};
		});

		if (closed_) {
			throw std::runtime_error("lock of an already closed lua_State");
		}

		owner_ = this_thread_id;
		locks_ += locks;
		return locks_;
	}

	int Gil::lock(int locks) {
		std::unique_lock lk(m_);
		return lock_unsafe(lk, locks);
	}

	void Gil::unlock_unsafe(int locks) {
		if (locks <= 0) {
			throw std::runtime_error("number of locks must be greater than 0");
		}

		if (owner_ != std::this_thread::get_id()) {
			throw std::runtime_error("unlock of mutex not owned by the current thread");
		}

		if (locks > locks_) {
			throw std::runtime_error("number of locks " + std::to_string(locks) + " is greater than current number of locks " + std::to_string(locks_));
		}

		locks_ -= locks;

		if (locks_ == 0) {
			owner_ = {};
			cv_.notify_one();
		}
	}

	int Gil::unlock(int locks) {
		std::lock_guard lk(m_);
		unlock_unsafe(1);
		return locks_;
	}

	int Gil::release(int& id) {
		std::lock_guard lk(m_);
		int released = locks_;
		id = ++ids_;

		// The thread previously owning the lock may have died and another thread is closing the lua_State
		// thus calling gc > destructor > yield on an unlocked Gil.
		// That is why we ignore yield for unlocked Gil
		if (locks_ != 0) {
			unlock_unsafe(released);
		}

		return released;
	}

	void Gil::acquire(int id, int locks) {
		std::unique_lock lk(m_);

		cv_.wait(lk, [this, id] {
			return closed_ || owner_ == std::thread::id{} && ids_ == id;
		});

		--ids_;

		// The thread previously owning the lock may have died and another thread is closing the lua_State
		// thus calling gc > destructor > yield on an unlocked Gil.
		// That is why we ignore yield for unlocked Gil
		if (locks != 0) {
			lock_unsafe(lk, locks);
		}

		cv_.notify_one();
	}

	void Gil::release_if_owned_by(std::thread::id id) {
		std::lock_guard lk(m_);
		if (owner_ == id) {
			owner_ = {};
			locks_ = 0;
			cv_.notify_one();
		}
	}

	void Gil::close() {
		std::lock_guard lk(m_);
		closed_ = true;
		owner_ = {};
		locks_ = 0;
		cv_.notify_all();
	}

	std::unique_lock<std::mutex> Gil::create_lock() {
		return std::unique_lock{ m_ };
	}

	GilPtr get_gil_of(lua_State* L) {
		lua_pushliteral(L, LUA_GIL_LOCK_UNIQUE_NAME);
		lua_rawget(L, LUA_REGISTRYINDEX);

		if (lua_isnil(L, -1)) {
			lua_pop(L, 1);
			return create_gil_and_lock(L);
		}

		auto gil = static_cast<GilPtr*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return *gil;
	}

	GilGuard::GilGuard(lua_State* L) : GilGuard::GilGuard(get_gil_of(L)) {
		// Nothing to do
	}

	GilGuard::GilGuard(GilPtr gil) : gil_(gil) {
		gil_->lock();
	}

	GilGuard::~GilGuard() {
		gil_->unlock();
	}

	GilYieldGuard::GilYieldGuard(lua_State* L) : GilYieldGuard(L, 0L) {
		// Nothing to do
	}

	GilYieldGuard::GilYieldGuard(lua_State* L, long timeout_ms) : GilYieldGuard(get_gil_of(L), timeout_ms) {
		// Nothing to do
	}

	GilYieldGuard::GilYieldGuard(GilPtr gil, long timeout_ms) : gil_(gil), timeout_ms_(timeout_ms) {
		rlocks_ = gil_->release(rid_);
	}

	GilYieldGuard::~GilYieldGuard() {
		if (timeout_ms_ > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout_ms_));
		}
		gil_->acquire(rid_, rlocks_);
	}

	GilUnlockGuard::GilUnlockGuard(lua_State* L) : GilUnlockGuard(get_gil_of(L)) {
		// Nothing to do
	}

	GilUnlockGuard::GilUnlockGuard(GilPtr gil) : gil_(gil), lk_(gil->create_lock()) {
		gil_->unlock_unsafe(1);
	}

	GilUnlockGuard::~GilUnlockGuard() {
		gil_->lock_unsafe(lk_, 1);
	}

	std::unique_lock<std::mutex>& GilUnlockGuard::get_lock() {
		return lk_;
	}

	std::mutex LuaStateContext::mutex;
	unsigned short LuaStateContext::g_class_id = 0;

	LuaStateContext::SwitchUpValueGuard::SwitchUpValueGuard(lua_State* L, int n) : L_(L), n_(n) {
		lua_pushvalue(L_, lua_upvalueindex(n_));
		ref_ = luaL_ref(L_, LUA_REGISTRYINDEX);
		lua_replace(L_, lua_upvalueindex(n_));
	}

	LuaStateContext::SwitchUpValueGuard::~SwitchUpValueGuard() {
		lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);
		luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
		lua_replace(L_, lua_upvalueindex(n_));
	}

	int LuaStateContext::add_dependecy(lua_State* L) {
		if (g_dep_id >= kMaxDependencies) {
			return -1;
		}

		if (g_dep_id == 0) {
			std::fill_n(dependencies, kMaxDependencies, LUA_REFNIL);
		}

		dependencies[g_dep_id] = luaL_ref(L, LUA_REGISTRYINDEX);
		return g_dep_id++;
	}

	LuaStateContext::SwitchUpValueGuard LuaStateContext::switch_dependency(lua_State* L, int n, int id) {
		if (id < 0 || id >= g_dep_id) {
			lua_pushnil(L);
			return SwitchUpValueGuard{ L, n };
		}

		auto ref = dependencies[id];
		if (ref == LUA_REFNIL) {
			lua_pushnil(L);
			return SwitchUpValueGuard{ L, n };
		}

		lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
		return SwitchUpValueGuard{ L, n };
	}

	void lua_module_push_context(lua_State* L) {
		lua_rawgetp(L, LUA_REGISTRYINDEX, kContextKey);
	}

	std::shared_ptr<LuaStateContext> get_context_from_registry(lua_State* L) {
		lua_module_push_context(L);
		if (lua_isnil(L, -1)) {
			return std::shared_ptr<LuaStateContext>();
		}

		auto context = static_cast<std::shared_ptr<LuaStateContext>*>(lua_touserdata(L, -1));
		lua_pop(L, 1);
		return *context;
	}

	std::shared_ptr<LuaStateContext> get_context_from_upvalue(lua_State* L) {
		return *static_cast<std::shared_ptr<LuaStateContext>*>(lua_touserdata(L, lua_upvalueindex(1)));
	}

	CallInfo::Error::Error(const char* message) : message_(message) {
		// Nothing to do
	}

	void CallInfo::Error::insert_with_signature_into(std::ostringstream& buffer) const {
		buffer << "\n    ";

		if (signature_ != nullptr) {
			buffer << signature_ << ": ";
		}

		insert_into(buffer);
	}

	void CallInfo::Error::insert_into(std::ostringstream& buffer) const {
		buffer << message_;
	}

	void CallInfo::Error::set_signature(const char* signature) {
		signature_ = signature;
	}

	CallInfo::MissingError::MissingError(const char* argname) : argname_(argname) {
		// Nothing to do
	}

	void CallInfo::MissingError::insert_into(std::ostringstream& buffer) const {
		buffer << "bad argument '" << argname_ << "' (argument is missing)";
	}

	CallInfo::TypeError::TypeError(lua_State* L, int index, const char* type) : L_(L), type_(type) {
		lua_pushvalue(L, index);
		ref_ = luaL_ref(L_, LUA_REGISTRYINDEX);
	}

	CallInfo::TypeError::~TypeError() {
		luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
	}

	void CallInfo::TypeError::insert_into(std::ostringstream& buffer) const {
		lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);
		buffer << "bad argument ";
		insert_description_into(buffer);
		buffer << " (cannot convert '" << internal::LuaTypeName(L_, -1) << "' to '" << type_ << "')";
		lua_pop(L_, 1);
	}

	CallInfo::PositionalTypeError::PositionalTypeError(lua_State* L, int pos, int index, const char* type) : CallInfo::TypeError(L, index, type), pos_(pos) {
		// Nothing to do
	}

	void CallInfo::PositionalTypeError::insert_description_into(std::ostringstream& buffer) const {
		buffer << "#" << pos_;
	}

	CallInfo::KeywordTypeError::KeywordTypeError(lua_State* L, const char* argname, int index, const char* type) : CallInfo::TypeError(L, index, type), argname_(argname) {
		// Nothing to do
	}

	void CallInfo::KeywordTypeError::insert_description_into(std::ostringstream& buffer) const {
		buffer << "'" << std::string(argname_) << "'";
	}

	CallInfo::DuplicatedError::DuplicatedError(int pos, const char* argname) : pos_(pos), argname_(argname) {
		// Nothing to do
	}

	void CallInfo::DuplicatedError::insert_into(std::ostringstream& buffer) const {
		buffer << "bad argument #" << pos_ << " (already provided as named parameter '" << argname_ << "'";
	}

	CallInfo::BadKeywordError::BadKeywordError(lua_State* L, int pos, int index) : L_(L), pos_(pos) {
		lua_pushvalue(L, index);
		ref_ = luaL_ref(L_, LUA_REGISTRYINDEX);
	}

	CallInfo::BadKeywordError::~BadKeywordError() {
		luaL_unref(L_, LUA_REGISTRYINDEX, ref_);
	}

	void CallInfo::BadKeywordError::insert_into(std::ostringstream& buffer) const {
		lua_rawgeti(L_, LUA_REGISTRYINDEX, ref_);
		buffer << "bad keyword argument #" << pos_ << " (expected a string, got " << internal::LuaTypeName(L_, -1) << ")";
		lua_pop(L_, 1);
	}

	CallInfo::UnkownKeywordError::UnkownKeywordError(const std::string& key) : key_(key) {
		// Nothing to do
	}

	void CallInfo::UnkownKeywordError::insert_into(std::ostringstream& buffer) const {
		buffer << "unknown keyword argument '" << key_ << "'";
	}


	CallInfo::CallInfo(lua_State* L, const char* fname, int offset) :
		L_(L),
		fname_(fname),
		offset_(offset),
		top_(lua_gettop(L)
		) {
		has_kwargs_ = top_ != 0 && usertype_info<Keywords>::lua_userdata_is(L_, top_);
		kwargc_ = has_kwargs_ ? Keywords::size(L_, top_) : 0;
		argc_ = (has_kwargs_ ? top_ - 1 : top_) - offset_;
	}

	bool CallInfo::add_overload(const char* signature, int argn, bool variadic) {
		lua_settop(L_, top_);  // drop what a previous overload may have pushed
		++overloads_;
		signature_ = signature;
		usedkw_ = 0;

		if (variadic) {
			if (has_kwargs_) {
				add_error("keyword arguments are not allowed for function with variadic parameters");
				return false;
			}

			if (argc_ < argn - 1) {
				add_error(MinArgError(argc_, argn - 1));
				return false;
			}
		}
		else if (argc_ + kwargc_ > argn) {
			add_error(MaxArgError(argc_ + kwargc_, argn));
			return false;
		}

		return true;
	}

	int CallInfo::overload_resolution_failed() {
		std::ostringstream buffer;

		if (overloads_ > 1) {
			buffer << "Overload resolution failed:";
		}

		for (auto i = 0; i < nerr_; i++) {
			errors_[i].insert_with_signature_into(buffer);
		}

		if (nerr_ == kMaxErrors) {
			buffer << "\n    Stopped after " << kMaxErrors << " errors;";
		}

		lua_settop(L_, top_);
		lua_module_push_error(L_, buffer.str().c_str());

		return -1;
	}

	int CallInfo::get_results() {
		return lua_gettop(L_) - top_;
	}

	void CallInfo::add_error(const char* message) {
		add_error(CallInfo::Error(message));
	}

	void CallInfo::add_error(CallInfo::Error&& error) {
		if (nerr_ == kMaxErrors) {
			return;
		}
		errors_[nerr_++] = std::move(error);
	}

	void CallInfo::add_error(const CallInfo::Error& error) {
		if (nerr_ == kMaxErrors) {
			return;
		}
		errors_[nerr_++] = error;
	}

	bool CallInfo::validate(const char* const* params, int nparams) {
		if (!has_kwargs_ || usedkw_ == kwargc_) {
			return true;
		}

		lua_pushnil(L_);  /* first key */
		int i = 0;

		while (lua_next(L_, top_) != 0) {
			++i;
			lua_pop(L_, 1);  // remove the value

			if (lua_type(L_, -1) != LUA_TSTRING) {
				add_error(BadKeywordError(L_, i, -1));
				continue;
			}

			auto key = lua_tostring(L_, -1);

			bool known = false;
			for (int i = 0; i < nparams && !known; ++i) {
				if (std::strcmp(key, params[i]) == 0) {
					known = true;
					break;
				}
			}

			if (!known) {
				add_error(UnkownKeywordError(key));
			}
		}

		if (i == 0) {
			lua_pop(L_, 1);  /* remove the first key */
		}

		return false;
	}

	void lua_module_push_error(lua_State* L, const char* message) {
		luaL_where(L, 1);
		lua_pushstring(L, message);
		lua_concat(L, 2);
	}

	int yield(lua_State* L) {
		auto vargc = lua_gettop(L);

		if (vargc > 1) {
			return luaL_error(L, "too many arguments");
		}

		// TODO : find a better way to force context switch when another thread is waiting for the mutex
		long timeout_ms = 5;

		if (vargc == 1) {
			bool is_valid = false;
			timeout_ms = lua_to(L, 1, static_cast<long*>(nullptr), is_valid);
			if (!is_valid) {
				luaL_typeerror(L, 1, "long");
			}
		}

		GilYieldGuard guard(L, timeout_ms);
		return 0;
	}

	int __call_constructor(lua_State* L) {
		auto vargc = lua_gettop(L);
		auto nargs = vargc - 1;

		lua_pushliteral(L, "new");
		lua_rawget(L, 1);

		for (int i = 2; i <= vargc; ++i) {
			lua_pushvalue(L, i);
		}

		lua_call(L, nargs, LUA_MULTRET);

		return lua_gettop(L) - vargc;
	}
}

namespace {
	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	template<typename T>
	int _atoi(lua_State* L, const std::string& s) {
		const char* str = s.c_str();
		auto len = s.length();

		T value = 0;
		decltype(len) i = 0;

		for (; i < len && std::isdigit(static_cast<unsigned char>(*str)); ++str) {
			int digit = *str - '0';
			value *= 10;
			value += digit;
			++i;
		}

		if (i != len) {
			luaL_error(L, "invalid index %s", s.c_str());
		}

		return value;
	}
}

namespace LUA_MODULE_NAME {
	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	size_t atosize_t(lua_State* L, const std::string& s) {
		return _atoi<size_t>(L, s);
	}

	/**
	 * https://en.cppreference.com/w/cpp/string/byte/atoi
	 */
	int atoi(lua_State* L, const std::string& s) {
		return _atoi<int>(L, s);
	}
}

#define LUA_MODULE_LUAOPEN_cdata_utils LUA_TOKEN_CONCAT_3(luaopen_, LUA_MODULE_NAME, _cdata_utils)
LUAAPI(int) LUA_MODULE_LUAOPEN_cdata_utils(lua_State* L) {
	const char cdata_utils_filename[] = "<built-in>/" LUA_MODULE_NAME_STR "/cdata_utils.lua";
	const char cdata_utils_string[] =
#include "cdata_utils_string.lua.inc"
		;

	auto top = lua_gettop(L);

	if (luaL_loadbuffer(L, cdata_utils_string, sizeof(cdata_utils_string) - 1, cdata_utils_filename) != 0) {
		lua_error(L);
	}

	lua_call(L, 0, LUA_MULTRET);
	return lua_gettop(L) - top;
}

#define LUA_MODULE_LUAOPEN_cdata_utils_tovoid_ptr LUA_TOKEN_CONCAT_3(luaopen_, LUA_MODULE_NAME, _cdata_utils_tovoid_ptr)
LUAAPI(int) LUA_MODULE_LUAOPEN_cdata_utils_tovoid_ptr(lua_State* L) {
	auto top = lua_gettop(L);
	auto cdata_utils = luaB_require(L, LUA_MODULE_NAME_STR ".cdata_utils");
	lua_pushliteral(L, "tovoid_ptr");
	lua_rawget(L, cdata_utils);
	lua_replace(L, cdata_utils);
	return lua_gettop(L) - top;
}

#define LUA_MODULE_LUAOPEN_cdata_utils_get_cdata_bytes LUA_TOKEN_CONCAT_3(luaopen_, LUA_MODULE_NAME, _cdata_utils_get_cdata_bytes)
LUAAPI(int) LUA_MODULE_LUAOPEN_cdata_utils_get_cdata_bytes(lua_State* L) {
	auto top = lua_gettop(L);
	auto cdata_utils = luaB_require(L, LUA_MODULE_NAME_STR ".cdata_utils");
	lua_pushliteral(L, "get_cdata_bytes");
	lua_rawget(L, cdata_utils);
	lua_replace(L, cdata_utils);
	return lua_gettop(L) - top;
}
