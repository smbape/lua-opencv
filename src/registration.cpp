#include <register_all.hpp>

#define BRET(b) return (int32_t) (b)

namespace {
	using namespace LUA_MODULE_NAME;

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

	void register_bit(sol::state_view& lua, sol::table& module) {
		if (lua["bit"] != sol::lua_nil) {
			module["bit"] = lua["bit"];
			return;
		}

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
	}

	struct kwargs_iterator_state {
		using it_t = NamedParameters::Base::iterator;
		it_t it;
		it_t last;

		kwargs_iterator_state(NamedParameters& kwargs)
			: it(kwargs.begin()), last(kwargs.end()) {
		}
	};

	// this gets called
	// to start the first iteration, and every
	// iteration there after
	auto kwargs_next(sol::user<kwargs_iterator_state&> user_it_state, sol::this_state ts) {
		// the state you passed in my_pairs is argument 1
		// the key value is argument 2, but we do not
		// care about the key value here
		kwargs_iterator_state& it_state = user_it_state;
		auto& it = it_state.it;
		if (it == it_state.last) {
			// return nil to signify that
			// there's nothing more to work with.
			return std::make_tuple(sol::object(sol::lua_nil),
				sol::object(sol::lua_nil));
		}

		// get the next pair
		auto itderef = *it;

		// 2 values are returned (pushed onto the stack):
		// the key and the value
		// the state is left alone
		auto r = std::make_tuple(
			sol::object(ts, sol::in_place, it->first),
			sol::object(ts, sol::in_place, it->second));

		// the iterator must be moved forward one before we return
		std::advance(it, 1);

		return r;
	}

	// pairs expects 3 returns:
	// the "next" function on how to advance,
	// the "table" itself or some state,
	// and an initial key value (can be nil)
	auto kwargs_pairs(NamedParameters& self) {
		// prepare our state
		kwargs_iterator_state it_state(self);

		// sol::user is a space/time optimization over regular
		// usertypes, it's incompatible with regular usertypes and
		// stores the type T directly in lua without any pretty
		// setup saves space allocation and a single dereference
		return std::make_tuple(&kwargs_next,
			sol::user<kwargs_iterator_state>(std::move(it_state)),
			sol::lua_nil);
	}

	void register_kwargs(sol::state_view& lua, sol::table& module) {
		auto kwargs = module.new_usertype<NamedParameters>("kwargs",
			// kwargs.new(...) -- dot syntax, no "self" value
			sol::meta_function::construct, sol::constructors<NamedParameters(), NamedParameters(NamedParameters::Table)>(),

			// kwargs(...) syntax, only
			sol::call_constructor, sol::constructors<NamedParameters(), NamedParameters(NamedParameters::Table)>()
		);

		kwargs[sol::meta_function::pairs] = &kwargs_pairs;

		using mapped_type = NamedParameters::Base::mapped_type;
		using key_type = NamedParameters::Base::key_type;
		using iterator = NamedParameters::Base::iterator;

		kwargs[sol::meta_function::index] = sol::resolve<mapped_type & (const key_type&)>(&NamedParameters::at);
		kwargs.set_function("get", sol::resolve<mapped_type & (const key_type&)>(&NamedParameters::at));

		kwargs[sol::meta_function::new_index] = sol::resolve<std::pair<iterator, bool>(const key_type&, mapped_type&&)>(&NamedParameters::insert_or_assign<mapped_type>);
		kwargs.set_function("set", sol::resolve<std::pair<iterator, bool>(const key_type&, mapped_type&&)>(&NamedParameters::insert_or_assign<mapped_type>));

		kwargs.set_function("has", [](NamedParameters& self, const std::string& key) {
			return self.count(key) != 0;
			});

		kwargs.set_function(sol::meta_function::length, &NamedParameters::size);
		kwargs.set_function("size", &NamedParameters::size);

		kwargs.set_function("is_instance", [](sol::object obj) {
			return obj.is<NamedParameters>();
			});
	}

	void register_garbage_collect(sol::state_view& lua, sol::table& module) {
		module.set_function("is_call_garbage_collect", is_call_garbage_collect);
		module.set_function("call_garbage_collect", call_garbage_collect);
	}

	bool garbage_collecte_active = false;

	void regiter_callbacks(sol::state_view& lua, sol::table& module) {
		module.set_function("notifyCallbacks", &notifyCallbacks);
	}

	std::vector<std::tuple<Callback, void*>> registered_callbacks;
}

namespace LUA_MODULE_NAME {
	sol::table LUA_MODULE_OPEN(sol::this_state L) {
		sol::state_view lua(L);
		sol::table module = lua.create_table();

		register_bit(lua, module);
		register_kwargs(lua, module);
		register_garbage_collect(lua, module);
		regiter_callbacks(lua, module);
		register_all(lua, module);
		register_extension(lua, module);

		return module;
	}

	bool is_call_garbage_collect() {
		return garbage_collecte_active;
	}

	void call_garbage_collect(bool value) {
		garbage_collecte_active = value;
	}

	int deny_new_index(lua_State* L) {
		return luaL_error(L, "Hacking is good. Rebuild it yourself without this protection!");
	}

	std::mutex callback_mutex;

	void registerCallback(Callback callback, void* userdata) {
		std::lock_guard<std::mutex> lock(callback_mutex);
		registered_callbacks.push_back(std::make_tuple(callback, userdata));
	}

	void notifyCallbacks(sol::this_state ts) {
		std::lock_guard<std::mutex> lock(callback_mutex);
		for (const auto& [callback, userdata] : registered_callbacks) {
			callback(userdata, ts);
		}
	}
}

int LUA_MODULE_LUAOPEN(lua_State* L) {
	// pass the lua_State,
	// the index to start grabbing arguments from,
	// and the function itself
	// optionally, you can pass extra arguments to the function
	// if that's necessary, but that's advanced usage and is
	// generally reserved for internals only
	return sol::stack::call_lua(
		L, 1, LUA_MODULE_NAME::LUA_MODULE_OPEN);
}
