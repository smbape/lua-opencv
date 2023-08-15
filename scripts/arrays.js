(() => {
    const getTypename = type => {
        return type
            .replace("cv::", "")
            .replace(/std::vector<(\w+)>/, (match, value_type) => {
                return `VectorOf${ value_type[0].toUpperCase() + value_type.slice() }`;
            });
    };

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
        struct OptionalArrays
        {
            OptionalArrays(_To& obj) {
                auto maybe_Array = maybe_impl(obj, static_cast<Array*>(nullptr));
                if (maybe_Array) {
                    ptr = reference_internal(*maybe_Array);
                    return;
                }

                ${ types.map((type, i) => `
                auto maybe_${ getTypename(type) } = maybe_impl(obj, static_cast<std::vector<${ type }>*>(nullptr));
                if (maybe_${ getTypename(type) }) {
                    val${ i + 1 } = *maybe_${ getTypename(type) };
                    setField(*this, *this, ${ i + 1 });
                    return;
                }`.trim()).join(`\n\n${ " ".repeat(16) }`) }
            }

            OptionalArrays(const _To& obj) : OptionalArrays(const_cast<_To&>(obj)) {}

            OptionalArrays(const std::shared_ptr<Array>& ptr) : ptr(ptr) {}

            OptionalArrays(const OptionalArrays& other) {
                other.setField(other, *this, other.field);
            }

            OptionalArrays() = default;

            template<typename T>
            inline void reset(T& obj) {
                ptr = std::make_shared<Array>(obj);
            }

            template<typename T>
            inline void reset(const T& obj) {
                ptr = std::make_shared<Array>(obj);
            }

            static void setField(const OptionalArrays& src, OptionalArrays& dst, std::uint8_t _field) {
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

            operator bool() const {
                return static_cast<bool>(ptr);
            }

            auto& operator*() {
                return *ptr;
            }

            std::uint8_t field = 0;
            std::shared_ptr<Array> ptr;

            ${ types.map((type, i) => `std::vector<${ type }> val${ i + 1 };`).join(`\n${ " ".repeat(12) }`) }
        };

        template<typename Array, typename _To = sol::object>
        inline decltype(auto) maybe_arrays(const _To& obj, Array*) {
            return OptionalArrays<Array>(obj);
        }

        template<typename Array>
        inline decltype(auto) maybe_arrays(const std::shared_ptr<Array>& ptr) {
            return OptionalArrays<Array>(ptr);
        }
    `.replace(/^ {4}/mg, "").trim());
})();
