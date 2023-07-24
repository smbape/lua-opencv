#include <my_object/my_object.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

namespace {
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
}

namespace LUA_MODULE_NAME {
    void regitster_my_object(sol::table& module) {
        module.new_usertype<test>("test",
            sol::constructors<test(), test(int)>(),
            "value", &test::value
        );

        module.new_usertype<cv::Mat>("Mat",
            // Mat(...) syntax, only
            sol::call_constructor,
            sol::factories(
                []() {
                    return std::make_shared<cv::Mat>();
                },
                [](int rows, int cols, int type) {
                    return std::make_shared<cv::Mat>(rows, cols, type);
                },
                fancy_func
            )
        );

        module["IMREAD_COLOR"] = cv::IMREAD_COLOR;

        module.set_function("imread", sol::overload([] (const cv::String& filename) {
            return cv::imread(filename, cv::IMREAD_COLOR);
        }, [] (const cv::String& filename, int flags) {
            return cv::imread(filename, flags);
        }));

        module.set_function("imshow", [](const cv::String& winname, std::variant<cv::Mat, double> _mat) {
            cv::Ptr<cv::_InputArray> mat;
            if (std::holds_alternative<cv::Mat>(_mat)) {
                mat.reset(new cv::_InputArray(std::get<cv::Mat>(_mat)));
            } else if (std::holds_alternative<double>(_mat)) {
                mat.reset(new cv::_InputArray(std::get<double>(_mat)));
            }

            cv::imshow(winname, *mat.get());
        });

        module.set_function("waitKey", sol::overload([] {
            return cv::waitKey();
        }, [] (int delay) {
            return cv::waitKey(delay);
        }));

        module["getBuildInformation"] = cv::getBuildInformation;

        module["fancy_func"] = fancy_func;
    }
}
