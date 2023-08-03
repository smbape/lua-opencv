#include <my_object/my_object.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace {
	using namespace LUA_MODULE_NAME;

	// Uses some of the fancier bits of sol2, including the
	// "transparent argument", sol::this_state, which gets the
	// current state and does not increment function arguments
	sol::object fancy_func(sol::object a, sol::this_state s) {
		sol::state_view lua(s);

		if (a.is<cv::Mat>()) {
			return sol::object(
				lua,
				sol::in_place,
				std::make_shared<cv::Mat>(100, 100, CV_8U, 125.)
			);
		}

		if (a.is<double>()) {
			return sol::object(lua, sol::in_place, a.as<double>());
		}

		// Can also use make_object
		return sol::make_object(lua, sol::lua_nil);
	}

	std::shared_ptr<cv::Mat> create(sol::this_state ts, sol::variadic_args args) {
		using namespace cv;

		const int argc = args.size() - 1;
		const bool has_kwarg = object_is<NamedParameters>(args.get<sol::object>(argc));

		NamedParameters kwargs;
		if (has_kwarg) {
			kwargs = object_as<NamedParameters>(args.get<sol::object>(argc));
		}

		{
			int usedkw = 0;

			// get argument m
			bool m_positional = false;
			bool m_kwarg = false;
			if (!has_kwarg || argc > 0) {
				// positional parameter
				if (argc < 0 || !object_is<cv::Mat>(args.get<sol::object>(0)) || has_kwarg && kwargs.count("m")) {
					goto overload1;
				}

				m_positional = true;
			}
			else if (kwargs.count("m")) {
				// named parameter
				auto& kwarg = kwargs.at("m");
				if (!object_is<cv::Mat>(kwarg)) {
					goto overload1;
				}

				m_kwarg = true;
				usedkw++;
			}
			else {
				// mandatory parameter
				goto overload1;
			}

			// too many parameters
			// unknown named parameters
			if (argc > 0 || usedkw != kwargs.size()) {
				goto overload1;
			}

			cv::Mat m_default;
			auto obj = args.get<sol::object>(0);
			auto& val = obj.as<cv::FileNode>();
			auto& m = m_positional ? object_as<cv::Mat>(args.get<sol::object>(0)) : m_kwarg ? object_as<cv::Mat>(kwargs.at("m")) : m_default;
			return std::make_shared<cv::Mat>(m);
		}
	overload1:

		{
			int usedkw = 0;

			int rows = 0;
			int cols = 0;
			int type = 0;
			cv::Scalar s;

			// get argument rows
			if (!has_kwarg || argc > 0) {
				// positional parameter
				if (argc < 0 || !object_is<int>(args.get<sol::object>(0)) || has_kwarg && kwargs.count("rows")) {
					goto overload2;
				}

				rows = object_as<int>(args.get<sol::object>(0));
			}
			else if (kwargs.count("rows")) {
				// named parameter
				auto& kwarg = kwargs.at("rows");
				if (!object_is<int>(kwarg)) {
					goto overload2;
				}

				rows = object_as<int>(kwarg);
				usedkw++;
			}
			else {
				// mandatory parameter
				goto overload2;
			}

			// get argument cols
			if (!has_kwarg || argc > 1) {
				// positional parameter
				if (argc < 1 || !object_is<int>(args.get<sol::object>(1)) || has_kwarg && kwargs.count("cols")) {
					goto overload2;
				}

				cols = object_as<int>(args.get<sol::object>(1));
			}
			else if (kwargs.count("cols")) {
				// named parameter
				auto& kwarg = kwargs.at("cols");
				if (!object_is<int>(kwarg)) {
					goto overload2;
				}

				cols = object_as<int>(kwarg);
				usedkw++;
			}

			// get argument type
			if (!has_kwarg || argc > 2) {
				// positional parameter
				if (argc < 2 || !object_is<int>(args.get<sol::object>(2)) || has_kwarg && kwargs.count("type")) {
					goto overload2;
				}

				type = object_as<int>(args.get<sol::object>(2));
			}
			else if (kwargs.count("type")) {
				// named parameter
				auto& kwarg = kwargs.at("type");
				if (!object_is<int>(kwarg)) {
					goto overload2;
				}

				type = object_as<int>(kwarg);
				usedkw++;
			}

			// get argument s
			if (!has_kwarg || argc > 3) {
				// positional parameter
				if (argc < 3 || !object_is<cv::Scalar>(args.get<sol::object>(3)) || has_kwarg && kwargs.count("s")) {
					goto overload2;
				}

				s = object_as<cv::Scalar>(args.get<sol::object>(3));
			}
			else if (kwargs.count("s")) {
				// named parameter
				auto& kwarg = kwargs.at("s");
				if (!object_is<cv::Scalar>(kwarg)) {
					goto overload2;
				}

				s = object_as<cv::Scalar>(kwarg);
				usedkw++;
			}

			// too many parameters
			// unknown named parameters
			if (argc > 3 || usedkw != kwargs.size()) {
				goto overload2;
			}

			return std::make_shared<cv::Mat>(rows, cols, type, s);
		}
	overload2:

		// LUA_MODULE_THROW("Overload resolution failed");
		sol::state_view lua(ts);
		luaL_error(lua.lua_state(), "Overload resolution failed");
		return std::shared_ptr<cv::Mat>();
	}


	using namespace cv;

	std::map<std::string, std::function<sol::object(sol::this_state&)>> getters({
	});

	sol::object dynamic_get(sol::table ns, sol::stack_object key, sol::this_state ts) {
		// we use stack_object for the arguments because we
		// know the values from Lua will remain on Lua's stack,
		// so long we we don't mess with it
		auto maybe_string_key = key.as<sol::optional<std::string>>();
		if (maybe_string_key) {
			const std::string& k = *maybe_string_key;
			if (getters.count(k)) {
				return getters[k](ts);
			}
		}

		// sol::state_view lua(ts);
		// std::stringstream err;
		// err << "AttributeError: module 'cv' has no attribute '" << (maybe_string_key ? *maybe_string_key : "") << "'";
		// luaL_error(lua.lua_state(), err.str().c_str());

		return ns[sol::metatable_key][key];
	}

	std::map<std::string, std::function<void(sol::stack_object&, sol::this_state&)>> setters({

		});

	void dynamic_set(sol::table ns, sol::stack_object key, sol::stack_object value, sol::this_state ts) {
		// we use stack_object for the arguments because we
		// know the values from Lua will remain on Lua's stack,
		// so long we we don't mess with it
		auto maybe_string_key = key.as<sol::optional<std::string>>();
		if (maybe_string_key) {
			const std::string& k = *maybe_string_key;
			if (setters.count(k)) {
				setters[k](value, ts);
				return;
			}
		}

		// sol::state_view lua(ts);
		// std::stringstream err;
		// err << "AttributeError: module 'cv' has no attribute '" << (maybe_string_key ? *maybe_string_key : "") << "'";
		// luaL_error(lua.lua_state(), err.str().c_str());

		ns[sol::metatable_key][key] = value;
	}

	// https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html
	// https://github.com/ThePhD/sol2/blob/develop/examples/source/index_and_newindex_usertype.cpp
	// https://github.com/ThePhD/sol2/blob/develop/examples/source/overloading_with_fallback.cpp
}

namespace LUA_MODULE_NAME {
	void regitster_my_object(sol::state_view& lua, sol::table& module) {
        sol::table exports = lua.create_table();

		exports[sol::meta_function::new_index] = dynamic_set;
		exports[sol::meta_function::index] = dynamic_get;

		sol::table proxy = lua.create_table();
		proxy[sol::metatable_key] = exports;

		module["cv2"] = proxy;
		// module["cv2"] = exports;

		exports.new_usertype<test>("test",
			sol::constructors<test(), test(int)>(),
			"value", &test::value
		);

		exports["CV_8UC1"] = CV_8UC1;
		exports["CV_8UC2"] = CV_8UC2;
		exports["CV_8UC3"] = CV_8UC3;
		exports["CV_8UC4"] = CV_8UC4;

		exports.new_usertype<NamedParameters>("kwargs",
			sol::call_constructor,
			sol::factories(
				[]() {
					return std::make_shared<NamedParameters>();
				},
				[](NamedParameters::Table kwargs) {
					return std::make_shared<NamedParameters>(kwargs);
				}
			)
		);

		exports.new_usertype<cv::Mat>("Mat",
			// Mat(...) syntax, only
			sol::call_constructor,
			sol::factories(create),
			// "rows", &cv::Mat::rows,
			"cols", &cv::Mat::cols,
			"dims", &cv::Mat::dims,
			"channels", &cv::Mat::channels
		);

		sol::usertype<cv::Mat> mat_type = exports["Mat"];
		mat_type["rows"] = &cv::Mat::rows;
		// mat_type.set("rows", sol::readonly(&cv::Mat::rows));
		// mat_type.set("rows", &cv::Mat::rows);

		exports["IMREAD_COLOR"] = cv::IMREAD_COLOR;

		exports.set_function("shared_ptr", [](std::shared_ptr<float> ptr) {
			*ptr = 3.f;
			});

		exports.set_function("imread", sol::overload([](const cv::String& filename) {
			return cv::imread(filename, cv::IMREAD_COLOR);
			}, cv::imread));

		exports.set_function("imshow", [](const cv::String& winname, std::variant<cv::Mat, double> _mat) {
			cv::Ptr<cv::_InputArray> mat;
			if (std::holds_alternative<cv::Mat>(_mat)) {
				mat.reset(new cv::_InputArray(std::get<cv::Mat>(_mat)));
			}
			else if (std::holds_alternative<double>(_mat)) {
				mat.reset(new cv::_InputArray(std::get<double>(_mat)));
			}

			cv::imshow(winname, *mat.get());
			});

		exports.set_function("waitKey", sol::overload([] {
			return cv::waitKey();
			}, cv::waitKey));

		exports["getBuildInformation"] = cv::getBuildInformation;

		exports["fancy_func"] = fancy_func;

		exports.set_function("multi_tuple", [] { return std::make_tuple(10, "goodbye"); });

		exports.set_function("proxy", [](sol::function fn, const cv::Mat& mat) {
			fn(mat);
			});
	}
}
