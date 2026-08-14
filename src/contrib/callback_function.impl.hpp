#pragma once
#include <lua_bridge.hdr.hpp>

namespace LUA_MODULE_NAME {
	template<typename R, typename... Args>
	struct CallbackInvoker {
		using Invoker = CallbackInvoker<R, Args...>;

		static R invoke(Args... args, void* userdata);

		CallbackInvoker(lua_State* L, const Function& fn, const Object& userdata) {
			this->fn.assign(L, fn);
			this->userdata.assign(L, userdata);
		}

		bool active = true;
		Function fn;
		Object userdata;
	};

	template<typename R, typename... Args>
	R CallbackInvoker<R, Args...>::invoke(Args... args, void* user_data) {
		const auto& invoker = *reinterpret_cast<Invoker*>(user_data);
		if (!invoker.active) {
			if constexpr (std::is_same_v<R, void>) {
				return;
			}
			else {
				R ret;
				return ret;
			}
		}

		auto& fn = invoker.fn;
		auto& userdata = invoker.userdata;
		auto& L = fn.L;
		GilLock lock(L);

		lua_push(L, fn);

		// https://stackoverflow.com/questions/7230621/how-can-i-iterate-over-a-packed-variadic-template-argument-list/60136761#60136761
		int nargs = 0;
		([&] {
			lua_push(L, args);
			nargs++;
			} (), ...);

		lua_push(L, userdata);
		nargs++;

		if constexpr (std::is_same_v<R, void>) {
			lua_call(L, nargs, 0);
		}
		else {
			lua_call(L, nargs, 1);
		}

		if constexpr (!std::is_same_v<R, void>) {
			// I did not find a way to keep reference to pointers without memory leak
			// Return the pointer hoping it will not be garbage collected
			if constexpr (std::is_same_v<R, const char*>) {
				bool is_valid = lua_type(L, -1) == LUA_TSTRING;
				if (!is_valid) {
					luaL_typeerror(L, -1, internal::GetTypeName<R>());
				}

				size_t len;
				auto c_str = lua_tolstring(L, -1, &len);
				lua_pop(L, 1);
				return c_str;
			}
			else if constexpr (std::is_pointer_v<R>) {
				bool is_valid = lua_islightuserdata(L, -1);
				if (!is_valid) {
					luaL_typeerror(L, -1, internal::GetTypeName<R>());
				}

				auto ptr = static_cast<R>(lua_touserdata(L, -1));
				lua_pop(L, 1);
				return ptr;
			}
			else {
				bool is_valid;
				auto value_holder = lua_to(L, -1, static_cast<R*>(nullptr), is_valid);
				if (!is_valid) {
					luaL_typeerror(L, -1, internal::GetTypeName<R>());
				}

				lua_pop(L, 1);
				decltype(auto) value = extract_holder(value_holder, static_cast<R*>(nullptr));
				return value;
			}
		}
	}

	template<typename R, typename... Args>
	struct CallbackInfo {
		using Callback = R(*)(Args..., void*);
		using Invoker = CallbackInvoker<R, Args...>;

		Callback callback;
		void* userdata;
		Invoker* invoker;
	};

	template<typename Invoker>
	int CallbackInfo_destroy(lua_State* L) {
		auto invoker_ptr = static_cast<std::shared_ptr<Invoker>*>(lua_touserdata(L, 1));
		invoker_ptr->~shared_ptr();
		return 0;
	}

	template<typename R, typename... Args>
	CallbackInfo<R, Args...> lua_tocallback(lua_State* L, const Function& fn, const Object& userdata, std::function<R(Args...)>*) {
		using Invoker = CallbackInvoker<R, Args...>;
		std::shared_ptr<Invoker>* invoker_ptr;

		lua_push(L, fn);
		const auto fn_lua_ptr = lua_topointer(L, -1);
		lua_pop(L, 1);

		char fn_key[64];
		const int fn_key_len = snprintf(fn_key, 64, LUA_MODULE_NAME_STR "_CallbackInvoker_%p",  fn_lua_ptr);

		lua_pushlstring(L, fn_key, fn_key_len);
		lua_rawget(L, LUA_REGISTRYINDEX);

		if (lua_isnil(L, -1)) {
			// remove nil
			lua_pop(L, 1);

			// create an save invoker in the state
			lua_pushlstring(L, fn_key, fn_key_len);

			invoker_ptr = static_cast<std::shared_ptr<Invoker>*>(lua_newuserdata(L, sizeof(std::shared_ptr<Invoker>)));
			new(invoker_ptr) std::shared_ptr<Invoker>(std::make_shared<Invoker>(L, fn, userdata));

			lua_newtable(L);
			lua_pushstring(L, "__gc");
			lua_pushcfunction(L, CallbackInfo_destroy<Invoker>);
			lua_rawset(L, -3);

			lua_setmetatable(L, -2);
			lua_rawset(L, LUA_REGISTRYINDEX);
		} else {
			invoker_ptr = static_cast<std::shared_ptr<Invoker>*>(lua_touserdata(L, -1));
			lua_pop(L, 1);
		}

		return { .callback = Invoker::invoke, .userdata = invoker_ptr->get(), .invoker = invoker_ptr->get() };
	}

	// https://stackoverflow.com/questions/32024522/remove-the-last-type-of-a-template-parameter-pack#answer-32024861

	template <typename... Args>
	struct callback_function_base;

	template <std::size_t... I, typename R, typename... Args>
	struct callback_function_base<std::index_sequence<I...>, R, Args...> {
		using type = std::function<R(typename std::tuple_element<I, std::tuple<Args...>>::type...)>;
	};

	template<typename>
	struct callback_function;

	template<typename R, typename... Args>
	struct callback_function<R(*)(Args...)> : callback_function_base<std::make_index_sequence<sizeof...(Args) - 1>, R, Args...> {};

	template<typename T>
	using callback_function_t = typename callback_function<T>::type;
}
