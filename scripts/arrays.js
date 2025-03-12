const fs = require("node:fs");
const sysPath = require("node:path");

const primitiveTypes = new Set([
    "bool",
    "double",
    "cv::Scalar",
]);

const nonArrayTypes = new Set([
    "bool",
    "double",
    "cv::cuda::GpuMat",
    "cv::Mat",
    "cv::UMat",
    "cv::Scalar",
]);

const ndArraySupport = new Set([
    "cv::Mat",
]);

const limitedSupport = new Map([
    ["bool", ["InputArray"]],
    ["double", ["InputArray"]],
    ["cv::Scalar", ["InputArray"]],

    // Remove OutputArray : https://github.com/opencv/opencv/blob/4.11.0/modules/core/include/opencv2/core/mat.hpp#L325
    // Remove InputOutputArray : https://github.com/opencv/opencv/blob/4.11.0/modules/core/include/opencv2/core/mat.hpp#L399
    ["std::vector<bool>", ["InputArray"]],

    // Remove InputArray : https://github.com/opencv/opencv/blob/4.11.0/modules/core/include/opencv2/core/mat.hpp#L200
    // Remove OutputArray : https://github.com/opencv/opencv/blob/4.11.0/modules/core/include/opencv2/core/mat.hpp#L327
    ["std::vector<std::vector<bool>>", ["InputOutputArray"]],
]);

const constexprRestirction = (type, restriction = null) => {
    const restrictions = [];

    if (nonArrayTypes.has(type) && !ndArraySupport.has(type)) {
        restrictions.push("!is_arrays_type");
    }

    if (limitedSupport.has(type)) {
        const validArrayTypes = limitedSupport.get(type);
        const disjuctions = validArrayTypes.map(ArrayType => `std::is_same_v<Array, cv::_${ ArrayType }>`);
        restrictions.push(disjuctions.length > 1 ? `(${ disjuctions.join(" || ") })` : disjuctions[0]);
    }

    if (restrictions.length === 1 && restrictions[0].startsWith("(") && restrictions[0].endsWith(")")) {
        restrictions[0] = restrictions[0].slice(1, -1);
    }

    if (restriction) {
        restrictions.push(restriction);
    }

    if (restrictions.length === 0) {
        return "";
    }

    return restrictions.join(" && ");
};

const constexprWrap = (type, spaces, expr) => {
    const restriction = constexprRestirction(type);

    if (restriction.length === 0) {
        return expr;
    }

    const indent = " ".repeat(spaces);
    const lines = expr.replace(new RegExp(`^${ indent }`, "mg"), "").split("\n");

    return `
        if constexpr (${ restriction }) {
            ${ lines.join(`\n${ " ".repeat(12) }`) }
        }
    `.trim().replace(/^ {8}/mg, indent);
};

const knownArrayTypes = [
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
        knownArrayTypes.push(`cv::Vec${ cn }${ _Tp }`);
    }
}

for (const cn of [2, 3, 4, 6, 8]) { // eslint-disable-line no-magic-numbers
    knownArrayTypes.push(`cv::Vec${ cn }i`);
}

for (const _Tp of ["f", "d"]) {
    for (const cn of [2, 3, 4, 6]) { // eslint-disable-line no-magic-numbers
        knownArrayTypes.push(`cv::Vec${ cn }${ _Tp }`);
    }
}

const length = knownArrayTypes.length;

// Mat and UMat does not have vector<vector>
for (let i = 2; i < length; i++) {
    knownArrayTypes.push(`std::vector<${ knownArrayTypes[i] }>`);
}

for (let i = 0; i < knownArrayTypes.length; i++) {
    knownArrayTypes[i] = `std::vector<${ knownArrayTypes[i] }>`;
}

const types = [...nonArrayTypes, ...knownArrayTypes];

const hdr = `
    #pragma once

    namespace LUA_MODULE_NAME {
        template<typename Array, bool is_arrays_type>
        struct _OptionalArray;

        template<typename Array, bool is_arrays_type>
        inline int lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& array);
    }
`.replace(/^ {4}/mg, "").trim().replace(/[^\S\n]+$/mg, "").replaceAll(" ".repeat(4), "\t");

const hdr_impl = `
    #pragma once

    #include <lua_bridge_common.hpp>
    #include <opencv2/opencv.hpp>

    /**
     * Internal class used to create
     *   InputArray, OutputArray, InputOutputArray
     *   InputArrayOfArrays, OutputArrayOfArrays, InputOutputArrayOfArrays
     * It holds the Array data until destruction.
     * Array defaults to cv::Mat for *Array and std::vector<cv::Mat> for *Arrays
     */

    namespace LUA_MODULE_NAME {
        template<typename Array, bool is_arrays_type>
        class _OptionalArray {
        public:
            _OptionalArray();
            ~_OptionalArray();

            _OptionalArray(_OptionalArray& other);
            _OptionalArray(_OptionalArray&& other) noexcept;
            _OptionalArray(const _OptionalArray& other) = delete;

            _OptionalArray& operator=(_OptionalArray& other);
            _OptionalArray& operator=(_OptionalArray&& other) noexcept;
            _OptionalArray& operator=(const _OptionalArray& other) = delete;

            // Array parameter default value
            // 
            // cv.omnidir.undistortImage void 
            //     InputArray distorted 
            //     OutputArray undistorted /O
            //     InputArray K 
            //     InputArray D 
            //     InputArray xi 
            //     int flags 
            //     InputArray Knew cv::noArray()
            //     Size new_size Size()/C; /Ref
            //     InputArray R Mat::eye(3, 3, CV_64F)
            template<typename T>
            typename std::enable_if<!std::is_same_v<T, _OptionalArray>, _OptionalArray&>::type operator=(T& value) {
                reset(value);
                return *this;
            }

            _OptionalArray(lua_State* L, int index, bool& is_valid, const bool nd_mat = false);

            void reset();

            operator bool() const {
                return static_cast<bool>(m_ptr);
            }

            decltype(auto) get() {
                return m_ptr.get();
            }

            decltype(auto) operator*() {
                return m_ptr.operator*();
            }

            const int index() const {
                return m_index;
            }

            const void* data() const {
                return m_data;
            }

            const bool is_shared_ptr() const {
                return m_is_shared_ptr;
            }

        private:
            void initialize();
            void free();

            template<typename T>
            void reset(T& value);

            template<typename T>
            void reset(T&& value);

            int m_index { -1 };
            void* m_data { nullptr };
            bool m_is_shared_ptr { false };
            std::shared_ptr<Array> m_ptr;
        };

        template<typename Array, bool is_arrays_type>
        int lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& opt_array);

        // InputArray, OutputArray, InputOutputArray
        template<typename Array>
        using OptionalArray = _OptionalArray<Array, false>;

        // InputArrayOfArrays, OutputArrayOfArrays, InputOutputArrayOfArrays
        template<typename Array>
        using OptionalArrays = _OptionalArray<Array, true>;

        template<typename Array>
        inline auto lua_toarray(lua_State* L, int index, Array*, bool& is_valid) {
            // COPY or MOVE constructor/assignment
            return OptionalArray<Array>(L, index, is_valid);
        }

        template<typename Array>
        inline auto lua_toarrays(lua_State* L, int index, Array*, bool& is_valid, const bool nd_mat = false) {
            // COPY or MOVE constructor/assignment
            return OptionalArrays<Array>(L, index, is_valid, nd_mat);
        }
    }
`.replace(/^ {4}/mg, "").trim().replace(/[^\S\n]+$/mg, "").replaceAll(" ".repeat(4), "\t");

const impl = `
    #include <lua_bridge_arrays.hpp>

    namespace LUA_MODULE_NAME {
        template<typename Array, bool is_arrays_type>
        _OptionalArray<Array, is_arrays_type>::_OptionalArray() {
            initialize();
        }

        template<typename Array, bool is_arrays_type>
        _OptionalArray<Array, is_arrays_type>::~_OptionalArray() {
            free();
        }

        template<typename Array, bool is_arrays_type>
        _OptionalArray<Array, is_arrays_type>::_OptionalArray(_OptionalArray<Array, is_arrays_type>& other) {
            *this = other;
        }

        template<typename Array, bool is_arrays_type>
        _OptionalArray<Array, is_arrays_type>::_OptionalArray(_OptionalArray<Array, is_arrays_type>&& other) noexcept {
            *this = std::move(other);
        }

        template<typename Array, bool is_arrays_type>
        _OptionalArray<Array, is_arrays_type>& _OptionalArray<Array, is_arrays_type>::operator=(_OptionalArray<Array, is_arrays_type>& other) {
            free();

            switch (other.m_index) {
                ${ types.map((type, i) => `
                case ${ i }:
                    ${ constexprWrap(type, 20, `
                        ${ !primitiveTypes.has(type) ? `
                            if (other.m_is_shared_ptr) {
                                reset(*reinterpret_cast<std::shared_ptr<${ type }>*>(other.m_data));
                            }
                        `.replace(/^ {4}/mg, "").trim() : "" }
                        reset(*reinterpret_cast<${ type }*>(other.m_data));
                    `.replace(/^ {4}/mg, "").trim()) }
                    break;
                `.trim()).join(`\n${ " ".repeat(16) }`) }
                default:
                    // Nothing to do
                    break;
            }

            return *this;
        }

        // https://stackoverflow.com/questions/6687388/why-do-some-people-use-swap-for-move-assignments#answer-6687520
        template<typename Array, bool is_arrays_type>
        _OptionalArray<Array, is_arrays_type>& _OptionalArray<Array, is_arrays_type>::operator=(_OptionalArray<Array, is_arrays_type>&& other) noexcept {
            // 1. Destroy visible resources
            free();

            // 2. Move assign all bases and members.
            m_index = other.m_index;
            m_data = other.m_data;
            m_is_shared_ptr = other.m_is_shared_ptr;
            m_ptr = std::move(other.m_ptr);

            // 3. If the move assignment of bases and members didn't,
            //           make the rhs resource-less, then make it so.
            other.m_index = -1;
            other.m_data = nullptr;
            other.m_is_shared_ptr = false;

            return *this;
        }

        template<typename Array, bool is_arrays_type>
        _OptionalArray<Array, is_arrays_type>::_OptionalArray(lua_State* L, int index, bool& is_valid, const bool nd_mat) {
            if (lua_isnil(L, index)) {
                is_valid = true;
                initialize();
                return;
            }

            if constexpr (is_arrays_type) {
                if (nd_mat) {
                    ${ Array.from(ndArraySupport).map(type => `
                        auto value = lua_to(L, index, static_cast<${ type }*>(nullptr), is_valid);
                        if (is_valid) {
                            std::vector<${ type }> vec(1);
                            vec.front() = extract_holder(value, static_cast<${ type }*>(nullptr));
                            reset(std::move(vec));
                            return;
                        }
                    `.replace(/^ {4}/mg, "").trim()).join(`\n\n${ " ".repeat(20) }`) }
                }
            }

            ${ types.map((type, i) => constexprWrap(type, 12, `
            {
                auto value = lua_to(L, index, static_cast<${ type }*>(nullptr), is_valid);
                if (is_valid) {
                    reset(value);
                    return;
                }
            }`.trim())).join(`\n\n${ " ".repeat(12) }`) }

            is_valid = false;
        }

        template<typename Array, bool is_arrays_type>
        void _OptionalArray<Array, is_arrays_type>::initialize() {
            if constexpr (is_arrays_type) {
                reset(std::vector<cv::Mat>());
            }
            else {
                reset(cv::Mat());
            }
        }

        template<typename Array, bool is_arrays_type>
        void _OptionalArray<Array, is_arrays_type>::free() {
            if (m_ptr) {
                m_ptr.reset();
            }

            switch (m_index) {
                ${ types.map((type, i) => `
                case ${ i }:
                    ${ constexprWrap(type, 20, !primitiveTypes.has(type) ? `
                        if (m_is_shared_ptr) {
                            delete reinterpret_cast<std::shared_ptr<${ type }>*>(m_data);
                        } else {
                            delete reinterpret_cast<${ type }*>(m_data);
                        }
                    `.replace(/^ {4}/mg, "").trim() : `delete reinterpret_cast<${ type }*>(m_data);`) }
                    break;
                `.trim()).join(`\n${ " ".repeat(16) }`) }
                default:
                    // Nothind do do
                    break;
            }

            m_index = -1;
            m_data = nullptr;
            m_is_shared_ptr = false;
        }

        template<typename Array, bool is_arrays_type>
        void _OptionalArray<Array, is_arrays_type>::reset() {
            free();
            initialize();
        }

        template<typename Array, bool is_arrays_type>
        template<typename T>
        void _OptionalArray<Array, is_arrays_type>::reset(T& value) {
            // reset(T& value);
            // m_data is a copy of value
            // m_ptr = std::make_shared<Array>(m_data);
            // m_index = TypeIndex
            // m_is_shared_ptr = false

            // reset(std::shared_ptr<T>& value);
            // m_data is a copy of shared_ptr
            // m_ptr = std::make_shared<Array>(*m_data);
            // m_index = TypeIndex
            // m_is_shared_ptr = true

            ${ types.map((type, i) => `
                if constexpr (${ constexprRestirction(type, `std::is_same_v<T, ${ type }>`) }) {
                    auto pp = new T(value);
                    m_data = pp;
                    m_index = ${ i };
                    m_is_shared_ptr = false;
                    m_ptr = std::make_shared<Array>(*pp);
                }
                ${ !primitiveTypes.has(type) ? `
                    else if constexpr (${ constexprRestirction(type, `std::is_same_v<T, std::shared_ptr<${ type }>>`) }) {
                        auto pp = new T(value);
                        m_data = pp;
                        m_index = ${ i };
                        m_is_shared_ptr = true;
                        m_ptr = std::make_shared<Array>(**pp);
                    }
                `.replace(/^ {4}/mg, "").trim() : "" }
            `.replace(/^ {4}/mg, "").trim()).join(`\n${ " ".repeat(12) }else `) }
            else {
                static_assert(!std::is_same_v<T, T>, "Unsupported type");
            }
        }

        template<typename Array, bool is_arrays_type>
        template<typename T>
        void _OptionalArray<Array, is_arrays_type>::reset(T&& value) {
            // reset(T&& value);
            // m_data is a move of value
            // m_ptr = std::make_shared<Array>(m_data);
            // m_index = TypeIndex
            // m_is_shared_ptr = false

            // reset(std::shared_ptr<T>&& value);
            // m_data is a copy of shared_ptr
            // m_ptr = std::make_shared<Array>(*m_data);
            // m_index = TypeIndex
            // m_is_shared_ptr = true

            ${ types.map((type, i) => `
                if constexpr (${ constexprRestirction(type, `std::is_same_v<T, ${ type }>`) }) {
                    auto pp = new T(std::move(value));
                    m_data = pp;
                    m_index = ${ i };
                    m_is_shared_ptr = false;
                    m_ptr = std::make_shared<Array>(*pp);
                }
                ${ !primitiveTypes.has(type) ? `
                    else if constexpr (${ constexprRestirction(type, `std::is_same_v<T, std::shared_ptr<${ type }>>`) }) {
                        auto pp = new T(std::move(value));
                        m_data = pp;
                        m_index = ${ i };
                        m_is_shared_ptr = true;
                        m_ptr = std::make_shared<Array>(**pp);
                    }
                `.replace(/^ {4}/mg, "").trim() : "" }
            `.replace(/^ {4}/mg, "").trim()).join(`\n${ " ".repeat(12) }else `) }
            else {
                static_assert(!std::is_same_v<T, T>, "Unsupported type");
            }
        }

        template<typename Array, bool is_arrays_type>
        int lua_push(lua_State* L, const _OptionalArray<Array, is_arrays_type>& opt_array) {
            switch (opt_array.index()) {
                ${ types.map((type, i) => `
                case ${ i }:
                    ${ !primitiveTypes.has(type) ? `
                        if (opt_array.is_shared_ptr()) {
                            return lua_push(L, *reinterpret_cast<const std::shared_ptr<${ type }>*>(opt_array.data()));
                        }
                    `.replace(/^ {4}/mg, "").trim() : "" }
                    return lua_push(L, *reinterpret_cast<const ${ type }*>(opt_array.data()));
                `.trim()).join(`\n${ " ".repeat(16) }`) }
                default:
                    lua_pushnil(L);
                    return 1;
            }
        }

        // Template instantiation to force template definition in this file
        // since template declaration is in a header

        template class _OptionalArray<cv::_InputArray, false>;
        template class _OptionalArray<cv::_OutputArray, false>;
        template class _OptionalArray<cv::_InputOutputArray, false>;

        template class _OptionalArray<cv::_InputArray, true>;
        template class _OptionalArray<cv::_OutputArray, true>;
        template class _OptionalArray<cv::_InputOutputArray, true>;

        // Array parameter default value
        // 
        // cv.omnidir.undistortImage void 
        //     InputArray distorted 
        //     OutputArray undistorted /O
        //     InputArray K 
        //     InputArray D 
        //     InputArray xi 
        //     int flags 
        //     InputArray Knew cv::noArray()
        //     Size new_size Size()/C; /Ref
        //     InputArray R Mat::eye(3, 3, CV_64F)
        template void _OptionalArray<cv::_InputArray, false>::reset<cv::Mat>(cv::Mat&);

        template int lua_push<cv::_InputArray, false>(lua_State* L, const _OptionalArray<cv::_InputArray, false>& opt_array);
        template int lua_push<cv::_OutputArray, false>(lua_State* L, const _OptionalArray<cv::_OutputArray, false>& opt_array);
        template int lua_push<cv::_InputOutputArray, false>(lua_State* L, const _OptionalArray<cv::_InputOutputArray, false>& opt_array);

        template int lua_push<cv::_InputArray, true>(lua_State* L, const _OptionalArray<cv::_InputArray, true>& opt_array);
        template int lua_push<cv::_OutputArray, true>(lua_State* L, const _OptionalArray<cv::_OutputArray, true>& opt_array);
        template int lua_push<cv::_InputOutputArray, true>(lua_State* L, const _OptionalArray<cv::_InputOutputArray, true>& opt_array);
    }

`.replace(/^ {4}/mg, "").trim().replace(/[^\S\n]+$/mg, "").replaceAll(" ".repeat(4), "\t");

const LF = "\n";

fs.writeFileSync(sysPath.join(__dirname, "../src/include/lua_bridge_arrays.hdr.hpp"), hdr + LF);

fs.writeFileSync(sysPath.join(__dirname, "../src/include/lua_bridge_arrays.hpp"), hdr_impl + LF);

fs.writeFileSync(sysPath.join(__dirname, "../src/lua_bridge_arrays.cpp"), impl + LF);

fs.writeFileSync(sysPath.join(__dirname, "../generator/vectors.js"), `module.exports = ${ JSON.stringify(knownArrayTypes, null, 4) };${ LF }`);
