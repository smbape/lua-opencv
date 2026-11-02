const primitiveTypes = new Set([
    "bool",
    "double",
    "cv::Scalar",
]);

const nonArrayTypes = new Set([
    "cv::Mat",
    "std::vector<cv::Mat>",
    "cv::UMat",
    "std::vector<cv::UMat>",
    "double",
    "bool",
    "cv::cuda::GpuMat",
    "std::vector<cv::cuda::GpuMat>",
    "cv::cuda::GpuMatND",
    "std::vector<cv::cuda::GpuMatND>",
    "cv::cuda::HostMem",
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

const knownArrayTypes = [
    // Unique types
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
for (let i = 0; i < length; i++) {
    knownArrayTypes.push(`std::vector<${ knownArrayTypes[i] }>`);
}

for (let i = 0; i < knownArrayTypes.length; i++) {
    knownArrayTypes[i] = `std::vector<${ knownArrayTypes[i] }>`;
}

const types = new Set([...nonArrayTypes, ...knownArrayTypes]);

const declarations = [];

for (const array of ["InputArray", "OutputArray", "InputOutputArray"]) {
    declarations.push([`class cv._${ array }`, "", [], [], "", ""]);
    declarations.push([`cv._${ array }._${ array }`, "", [], [], "", ""]);

    for (const type of types) {
        if (limitedSupport.has(type) && !limitedSupport.get(type).includes(array)) {
            continue;
        }

        declarations.push(
            [`cv._${ array }._${ array }`, "", [], [
                [type, "obj", "", ["/Ref"]],
            ], "", ""]
        );
    }
}

module.exports = declarations;
