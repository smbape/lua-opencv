const fs = require("node:fs");
const sysPath = require("node:path");

const getTypename = type => {
    return type
        .replace(/\b(u?int(?:8|16|32|64))_t\b/g, "$1")
        .replaceAll("std::map", "MapOf")
        .replaceAll("std::pair", "PairOf")
        .replaceAll("std::vector", "VectorOf")
        .replaceAll("std::shared_ptr", "SharedPtrOf")
        .replace("cv::", "")
        .replace("std::", "")
        .replace(/\b_variant_t\b/g, "Variant")
        .replace(/::/g, "_")
        .replace(/\b[a-z]/g, m => m.toUpperCase())
        .replace(/, /g, "And")
        .replace(/[<>]/g, "");
};

const plain = new Set([
    "bool",
    "double",
    "cv::Mat",
    "cv::Scalar",
]);

// include/opencv2/core/mat.hpp(198)
// include/opencv2/core/mat.hpp(323)
// include/opencv2/core/mat.hpp(325)
// include/opencv2/core/mat.hpp(397)
const supported = new Map([
    ["bool", ["InputArray"]],
    ["double", ["InputArray"]],
    ["cv::Scalar", ["InputArray"]],
    ["std::vector<std::vector<bool>>", ["InputOutputArray"]],
    ["std::vector<bool>", ["InputArray"]],
]);

const constexpr = (type, spaces, expr) => {
    const conditions = [];

    if (plain.has(type)) {
        conditions.push("!is_arrays_type");
    }

    if (supported.has(type)) {
        const condition = supported.get(type).map(array => `std::is_same_v<Array, cv::_${ array }>`).join(" || ");
        conditions.push(plain.has(type) ? `(${ condition })` : condition);
    }

    if (conditions.length === 0) {
        return expr;
    }

    const indent = " ".repeat(spaces);
    const lines = expr.replace(new RegExp(`^${ indent }`, "mg"), "").split("\n");

    return `
        if constexpr (${ conditions.join(" && ") }) {
            ${ lines.join(`\n${ " ".repeat(12) }`) }
        }
    `.trim().replace(/^ {8}/mg, indent);
};

const types = [
    // Unique types
    "cv::Mat",
    "cv::UMat",
    "bool",
    "cv::RotatedRect",
    "cv::Range",
    "cv::Moments",

    // Ambiguous because number
    "uchar",
    "schar",
    "char",
    "ushort",
    "short",
    "int",
    "float",
    "double",
    // "float16_t",

    // Ambiguous because array of numbers
    "cv::Point3i",
    "cv::Point3f",
    "cv::Point3d",

    // "cv::Point2l", // DataType<int64>::depth is not defined
    "cv::Point2f",
    "cv::Point2d",
    "cv::Point",

    "cv::Rect2f",
    "cv::Rect2d",
    "cv::Rect",

    // "cv::Size2l", // DataType<int64>::depth is not defined
    "cv::Size2f",
    "cv::Size2d",
    "cv::Size",
];

// Ambiguous because array of numbers
for (const _Tp of ["b", "s", "w"]) {
    for (const cn of [2, 3, 4]) { // eslint-disable-line no-magic-numbers
        types.push(`cv::Vec${ cn }${ _Tp }`);
    }
}

for (const cn of [2, 3, 4, 6, 8]) { // eslint-disable-line no-magic-numbers
    types.push(`cv::Vec${ cn }i`);
}

for (const _Tp of ["f", "d"]) {
    for (const cn of [2, 3, 4, 6]) { // eslint-disable-line no-magic-numbers
        types.push(`cv::Vec${ cn }${ _Tp }`);
    }
}

const length = types.length;

// Mat and UMat does not have vector<vector>
for (let i = 2; i < length; i++) {
    types.push(`std::vector<${ types[i] }>`);
}

for (let i = 0; i < types.length; i++) {
    types[i] = `std::vector<${ types[i] }>`;
}

types.unshift(...plain);

const code = `
    #pragma once

    #include <lua_utils.hpp>
    #include <lua_generated_include.hpp>

    namespace LUA_MODULE_NAME {
        // InputArray, outputArray, InputOutputArray
        // InputArrayOfArrays, outputArrayOfArrays, InputOutputArrayOfArrays
        template<typename Array, typename _To, bool is_arrays_type>
        struct _OptionalArray
        {
            _OptionalArray() = default;

            _OptionalArray(_To& obj) {
                if (obj == sol::lua_nil) {
                    if constexpr (is_arrays_type) {
                        setField(*this, *this, ${ types.length + 1 });
                    } else {
                        setField(*this, *this, ${ types.length + 2 });
                    }
                    return;
                }

                auto maybe_Array = maybe_impl(obj, static_cast<Array*>(nullptr));
                if (maybe_Array) {
                    ptr = reference_internal(*maybe_Array);
                    return;
                }

                ${ types.map((type, i) => constexpr(type, 16, `
                auto maybe_${ getTypename(type) } = maybe_impl(obj, static_cast<${ type }*>(nullptr));
                if (maybe_${ getTypename(type) }) {
                    this->obj = new std::shared_ptr<${ type }>(maybe_${ getTypename(type) });
                    setField(*this, *this, ${ i + 1 });
                    return;
                }`.trim())).join(`\n\n${ " ".repeat(16) }`) }
            }

            _OptionalArray(const _To& obj) : _OptionalArray(const_cast<_To&>(obj)) {}

            _OptionalArray(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

            _OptionalArray(const _OptionalArray& other) {
                other.setField(other, *this, other.field);
            }

            _OptionalArray& operator=(const _OptionalArray& other) {
                other.setField(other, *this, other.field);
                return *this;
            }

            _OptionalArray(_OptionalArray&& other) noexcept :
                ptr(std::move(other.ptr)),
                mval(std::move(other.mval)),
                vval(std::move(other.vval))
            {
                switch (field) {
                ${ types.map((type, i) => `
                case ${ i + 1 }:
                    ${ constexpr(type, 20, `
                    this->obj = new std::shared_ptr<${ type }>(std::move(*reinterpret_cast<std::shared_ptr<${ type }>*>(other.obj)));
                    `.trim()) }
                    break;
                `.trim()).join(`\n${ " ".repeat(16) }`) }
                default:
                    // Nothind do do
                    break;
                }
            }

            ~_OptionalArray() {
                switch (field) {
                ${ types.map((type, i) => `
                case ${ i + 1 }:
                    ${ constexpr(type, 20, `
                    delete reinterpret_cast<std::shared_ptr<${ type }>*>(obj);
                    obj = nullptr;
                    `.trim()) }
                    break;
                `.trim()).join(`\n${ " ".repeat(16) }`) }
                default:
                    // Nothind do do
                    break;
                }
            }

            template<typename T>
            inline void reset(T& obj) {
                ptr = std::make_shared<Array>(obj);
            }

            template<typename T>
            inline void reset(const T& obj) {
                ptr = std::make_shared<Array>(obj);
            }

            static void setField(const _OptionalArray& src, _OptionalArray& dst, std::uint8_t _field) {
                dst.field = _field;

                switch (_field) {
                ${ types.map((type, i) => `
                case ${ i + 1 }:
                    ${ constexpr(type, 20, `
                    if (&src != &dst) {
                        if (dst.obj) {
                            *reinterpret_cast<std::shared_ptr<${ type }>*>(dst.obj) = *reinterpret_cast<std::shared_ptr<${ type }>*>(src.obj);
                        } else {
                            dst.obj = new std::shared_ptr<${ type }>(*reinterpret_cast<std::shared_ptr<${ type }>*>(src.obj));
                        }
                    }
                    if (dst.obj) {
                        dst.reset(**reinterpret_cast<std::shared_ptr<${ type }>*>(dst.obj));
                    }
                    `) }
                    break;
                `.trim()).join(`\n${ " ".repeat(16) }`) }
                case ${ types.length + 1 }:
                    dst.reset(dst.vval);
                    break;
                case ${ types.length + 2 }:
                    dst.reset(dst.mval);
                    break;
                default:
                    if (&src != &dst) {
                        dst.ptr = src.ptr;
                    }
                }
            }

            operator bool() const {
                return static_cast<bool>(ptr);
            }

            auto& operator*() {
                return *ptr;
            }

            std::uint8_t field = 0;
            std::shared_ptr<Array> ptr;
            cv::Mat mval;
            std::vector<cv::Mat> vval;
            void* obj = nullptr;
        };

        // InputArray, outputArray, InputOutputArray
        template<typename Array, typename _To = sol::object>
        struct OptionalArray : public _OptionalArray<Array, _To, false> {};

        // InputArrayOfArrays, outputArrayOfArrays, InputOutputArrayOfArrays
        template<typename Array, typename _To = sol::object>
        struct OptionalArrays : public _OptionalArray<Array, _To, true> {};

        template<typename Array, typename _To = sol::object>
        inline decltype(auto) maybe_arrays(const _To& obj, Array*) {
            return OptionalArrays<Array>(obj);
        }

        template<typename Array>
        inline decltype(auto) maybe_arrays(const std::shared_ptr<Array>& ptr) {
            return OptionalArrays<Array>(ptr);
        }

        template<typename Array, typename _To = sol::object>
        inline decltype(auto) maybe_array(const _To& obj, Array*) {
            return OptionalArray<Array>(obj);
        }

        template<typename Array>
        inline decltype(auto) maybe_array(const std::shared_ptr<Array>& ptr) {
            return OptionalArray<Array>(ptr);
        }
    }
`.replace(/^ {4}/mg, "").trim().replaceAll(" ".repeat(4), "\t");

fs.writeFileSync(sysPath.join(__dirname, "../src/include/lua_utils_arrays.hpp"), code);
