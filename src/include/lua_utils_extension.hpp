#pragma once

#include <luadef.hpp>
#include <opencv2/core/types.hpp>

namespace LUA_MODULE_NAME {
    // cv::Point_
    template<typename _To, typename _Tp>
    bool object_is_impl(const _To& obj, cv::Point_<_Tp>*) {
        if (obj.is<cv::Point_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 2;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    bool object_is_impl(_To& obj, cv::Point_<_Tp>*) {
        if (obj.is<cv::Point_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 2;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(const _To& obj, cv::Point_<_Tp>*) {
        if (obj.is<cv::Point_<_Tp>>()) {
            return obj.as<cv::Point_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Point_<_Tp>(obj.as<_Tp>());
        }

        cv::Point_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 2 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(_To& obj, cv::Point_<_Tp>*) {
        if (obj.is<cv::Point_<_Tp>>()) {
            return obj.as<cv::Point_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Point_<_Tp>(obj.as<_Tp>());
        }

        cv::Point_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 2 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    // cv::Rect_
    template<typename _To, typename _Tp>
    bool object_is_impl(const _To& obj, cv::Rect_<_Tp>*) {
        if (obj.is<cv::Rect_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 4;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    bool object_is_impl(_To& obj, cv::Rect_<_Tp>*) {
        if (obj.is<cv::Rect_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 4;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(const _To& obj, cv::Rect_<_Tp>*) {
        if (obj.is<cv::Rect_<_Tp>>()) {
            return obj.as<cv::Rect_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Rect_<_Tp>(obj.as<_Tp>());
        }

        cv::Rect_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 4 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(_To& obj, cv::Rect_<_Tp>*) {
        if (obj.is<cv::Rect_<_Tp>>()) {
            return obj.as<cv::Rect_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Rect_<_Tp>(obj.as<_Tp>());
        }

        cv::Rect_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 4 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    // cv::Scalar_
    template<typename _To, typename _Tp>
    bool object_is_impl(const _To& obj, cv::Scalar_<_Tp>*) {
        if (obj.is<cv::Scalar_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 4;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    bool object_is_impl(_To& obj, cv::Scalar_<_Tp>*) {
        if (obj.is<cv::Scalar_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 4;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(const _To& obj, cv::Scalar_<_Tp>*) {
        if (obj.is<cv::Scalar_<_Tp>>()) {
            return obj.as<cv::Scalar_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Scalar_<_Tp>(obj.as<_Tp>());
        }

        cv::Scalar_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 4 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(_To& obj, cv::Scalar_<_Tp>*) {
        if (obj.is<cv::Scalar_<_Tp>>()) {
            return obj.as<cv::Scalar_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Scalar_<_Tp>(obj.as<_Tp>());
        }

        cv::Scalar_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 4 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    // cv::Size_
    template<typename _To, typename _Tp>
    bool object_is_impl(const _To& obj, cv::Size_<_Tp>*) {
        if (obj.is<cv::Size_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 2;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    bool object_is_impl(_To& obj, cv::Size_<_Tp>*) {
        if (obj.is<cv::Size_<_Tp>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= 2;
        }

        return false;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(const _To& obj, cv::Size_<_Tp>*) {
        if (obj.is<cv::Size_<_Tp>>()) {
            return obj.as<cv::Size_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Size_<_Tp>(obj.as<_Tp>());
        }

        cv::Size_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 2 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    template<typename _To, typename _Tp>
    auto object_as_impl(_To& obj, cv::Size_<_Tp>*) {
        if (obj.is<cv::Size_<_Tp>>()) {
            return obj.as<cv::Size_<_Tp>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Size_<_Tp>(obj.as<_Tp>());
        }

        cv::Size_<_Tp> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < 2 && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    // cv::Vec
    template<typename _To, typename _Tp, int cn>
    bool object_is_impl(const _To& obj, cv::Vec<_Tp, cn>*) {
        if (obj.is<cv::Vec<_Tp, cn>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= cn;
        }

        return false;
    }

    template<typename _To, typename _Tp, int cn>
    bool object_is_impl(_To& obj, cv::Vec<_Tp, cn>*) {
        if (obj.is<cv::Vec<_Tp, cn>>() || obj.is<_Tp>()) {
            return true;
        }

        if (obj.is<std::vector<_Tp>>()) {
            const auto size = obj.as<std::vector<_Tp>>().size();
            return size > 0 && size <= cn;
        }

        return false;
    }

    template<typename _To, typename _Tp, int cn>
    auto object_as_impl(const _To& obj, cv::Vec<_Tp, cn>*) {
        if (obj.is<cv::Vec<_Tp, cn>>()) {
            return obj.as<cv::Vec<_Tp, cn>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Vec<_Tp, cn>(obj.as<_Tp>());
        }

        cv::Vec<_Tp, cn> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < cn && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }

    template<typename _To, typename _Tp, int cn>
    auto object_as_impl(_To& obj, cv::Vec<_Tp, cn>*) {
        if (obj.is<cv::Vec<_Tp, cn>>()) {
            return obj.as<cv::Vec<_Tp, cn>>();
        }

        if (obj.is<_Tp>()) {
            return cv::Vec<_Tp, cn>(obj.as<_Tp>());
        }

        cv::Vec<_Tp, cn> res;

        if (obj.is<std::vector<_Tp>>()) {
            const auto& vec = obj.as<std::vector<_Tp>>();
            for (int i = 0; i < cn && i < vec.size(); i++) {
                res[i] = vec[i];
            }
        }

        return res;
    }
}
