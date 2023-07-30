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
		const bool has_kwarg = vargs_is<NamedParameters>(args, argc);

		NamedParameters kwargs;
		if (has_kwarg) {
			kwargs = vargs_get<NamedParameters>(args, argc);
		}

		{
			int usedkw = 0;

			// get argument m
			bool m_positional = false;
			bool m_kwarg = false;
			if (!has_kwarg || argc > 0) {
				// positional parameter
				if (argc < 0 || !vargs_is<cv::Mat>(args, 0) || has_kwarg && kwargs.count("m")) {
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
			if (argc >= 5 || usedkw != kwargs.size()) {
				goto overload1;
			}

			cv::Mat m_default;
			auto& m = m_positional ? vargs_get<cv::Mat>(args, 0) : m_kwarg ? object_as<cv::Mat>(kwargs.at("m")) : m_default;
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
				if (argc < 0 || !vargs_is<int>(args, 0) || has_kwarg && kwargs.count("rows")) {
					goto overload2;
				}

				rows = vargs_get<int>(args, 0);
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
				if (argc < 1 || !vargs_is<int>(args, 1) || has_kwarg && kwargs.count("cols")) {
					goto overload2;
				}

				cols = vargs_get<int>(args, 1);
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
				if (argc < 2 || !vargs_is<int>(args, 2) || has_kwarg && kwargs.count("type")) {
					goto overload2;
				}

				type = vargs_get<int>(args, 2);
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
				if (argc < 3 || !vargs_is<cv::Scalar>(args, 3) || has_kwarg && kwargs.count("s")) {
					goto overload2;
				}

				s = vargs_get<cv::Scalar>(args, 3);
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
			if (argc >= 5 || usedkw != kwargs.size()) {
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

	// https://sol2.readthedocs.io/en/latest/tutorial/cxx-in-lua.html
	// https://github.com/ThePhD/sol2/blob/develop/examples/source/index_and_newindex_usertype.cpp
	// https://github.com/ThePhD/sol2/blob/develop/examples/source/overloading_with_fallback.cpp
}

namespace LUA_MODULE_NAME {
	void regitster_my_object(sol::state_view& lua, sol::table& module) {
		module.new_usertype<test>("test",
			sol::constructors<test(), test(int)>(),
			"value", &test::value
		);

		module["CV_8UC1"] = CV_8UC1;
		module["CV_8UC2"] = CV_8UC2;
		module["CV_8UC3"] = CV_8UC3;
		module["CV_8UC4"] = CV_8UC4;

		module.new_usertype<NamedParameters>("kwargs",
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

		module.new_usertype<cv::Mat>("Mat",
			// Mat(...) syntax, only
			sol::call_constructor,
			sol::factories(create),
			// "rows", &cv::Mat::rows,
			"cols", &cv::Mat::cols,
			"dims", &cv::Mat::dims,
			"channels", &cv::Mat::channels
		);

		sol::usertype<cv::Mat> mat_type = module["Mat"];
		mat_type["rows"] = &cv::Mat::rows;
		// mat_type.set("rows", sol::readonly(&cv::Mat::rows));
		// mat_type.set("rows", &cv::Mat::rows);

		module["IMREAD_COLOR"] = cv::IMREAD_COLOR;

		module.set_function("imread", sol::overload([](const cv::String& filename) {
			return cv::imread(filename, cv::IMREAD_COLOR);
			}, cv::imread));

		module.set_function("imshow", [](const cv::String& winname, std::variant<cv::Mat, double> _mat) {
			cv::Ptr<cv::_InputArray> mat;
			if (std::holds_alternative<cv::Mat>(_mat)) {
				mat.reset(new cv::_InputArray(std::get<cv::Mat>(_mat)));
			}
			else if (std::holds_alternative<double>(_mat)) {
				mat.reset(new cv::_InputArray(std::get<double>(_mat)));
			}

			cv::imshow(winname, *mat.get());
			});

		module.set_function("waitKey", sol::overload([] {
			return cv::waitKey();
			}, cv::waitKey));

		module["getBuildInformation"] = cv::getBuildInformation;

		module["fancy_func"] = fancy_func;

		module.set_function("multi_tuple", [] { return std::make_tuple(10, "goodbye"); });

		module.set_function("proxy", [](sol::function fn, const cv::Mat& mat) {
			fn(mat);
			});
	}
}
