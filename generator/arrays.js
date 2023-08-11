(() => {
    const types = [
        // Unique types
        "cv::Mat",
        "cv::UMat",
        "cv::RotatedRect",

        // Ambiguous because Array of numbers
        "char", // Array of n numbers
        "uchar", // Array of n numbers
        "int", // Array of n numbers
        "float", // Array of n numbers
        "double", // Array of n numbers

        // Ambiguous because Array of Array numbers
        "cv::Point", // Array of Array of 2 numbers
        "cv::Point2f", // Array of Array of 2 numbers
        "cv::Rect", // Array of Array of 4 numbers
        "cv::Size", // Array of Array of 2 numbers
        "cv::Vec6f", // Array of Array of 6 numbers
        "std::vector<char>", // Array of Array of n numbers
        "std::vector<int>", // Array of Array of n numbers

        // Ambiguous because Array of Array of Array of 2 numbers
        "std::vector<cv::Point>", // Array of Array of Array of 2 numbers
        "std::vector<cv::Point2f>", // Array of Array of Array of 2 numbers
    ];

    console.log(`
        template<typename Array, typename _To = sol::object>
        bool object_is_arrays(const _To& obj, Array*) {
            return object_is_impl(obj, static_cast<Array*>(nullptr))
                || ${ types.map(type => `object_is_impl(obj, static_cast<std::vector<${ type }>*>(nullptr))`).join(`\n${ " ".repeat(16) }|| `) }
                ;
        }

        template<typename Array, typename _To = sol::object>
        struct ArraysSharedPtr
        {
            ArraysSharedPtr(_To& obj) {
                if (object_is_impl(obj, static_cast<Array*>(nullptr))) {
                    const Array& input = object_as_impl(obj, static_cast<Array*>(nullptr));
                    ptr = reference_internal(input);
                    return;
                }

                ${ types.map((type, i) => `
                if (object_is_impl(obj, static_cast<std::vector<${ type }>*>(nullptr))) {
                    val${ i + 1 } = object_as_impl(obj, static_cast<std::vector<${ type }>*>(nullptr));
                    setField(*this, *this, ${ i + 1 });
                    return;
                }`.trim()).join(`\n\n${ " ".repeat(16) }`) }
            }

            ArraysSharedPtr(const _To& obj) : ArraysSharedPtr(const_cast<_To&>(obj)) {}

            ArraysSharedPtr(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

            ArraysSharedPtr(const ArraysSharedPtr& src) {
                src.setField(src, *this, src.field);
            }

            template<typename T>
            void reset(T& obj) {
                ptr = std::make_shared<Array>(obj);
            }

            template<typename T>
            void reset(const T& obj) {
                ptr = std::make_shared<Array>(obj);
            }

            static void setField(const ArraysSharedPtr& src, ArraysSharedPtr& dst, std::uint8_t _field) {
                dst.field = _field;

                switch (_field) {
                ${ types.map((type, i) => `
                case ${ i + 1 }:
                    if (&src != &dst) {
                        dst.val${ i + 1 } = src.val${ i + 1 };
                    }
                    dst.reset(dst.val${ i + 1 });
                    break;
                `.trim()).join(`\n${ " ".repeat(16) }`) }
                default:
                    if (&src != &dst) {
                        dst.ptr = src.ptr;
                    }
                }
            }

            auto& operator*() {
                return *ptr;
            }

            std::uint8_t field = 0;
            std::shared_ptr<Array> ptr;

            ${ types.map((type, i) => `std::vector<${ type }> val${ i + 1 };`).join(`\n${ " ".repeat(12) }`) }
        };

        template<typename Array, typename _To = sol::object>
        decltype(auto) object_as_arrays(const _To& obj, Array*) {
            return ArraysSharedPtr<Array>(obj);
        }

        template<typename Array>
        decltype(auto) object_as_arrays(const std::shared_ptr<Array>& ptr) {
            return ArraysSharedPtr<Array>(ptr);
        }
    `.replace(/^ {4}/mg, "").trim());
})();
