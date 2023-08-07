const semver = require("semver");

exports.SIMPLE_ARGTYPE_DEFAULTS = new Map([
    ["bool", "0"],
    ["size_t", "0"],
    ["SSIZE_T", "0"],
    ["int", "0"],
    ["float", "0.f"],
    ["double", "0"],
    ["c_string", "(char*)\"\""],

    ["int8", "0"],
    ["int8_t", "0"],
    ["int16", "0"],
    ["int16_t", "0"],
    ["int32", "0"],
    ["int32_t", "0"],
    ["int64", "0"],
    ["int64_t", "0"],

    ["uint8", "0"],
    ["uint8_t", "0"],
    ["uint16", "0"],
    ["uint16_t", "0"],
    ["uint32", "0"],
    ["uint32_t", "0"],
    ["uint64", "0"],
    ["uint64_t", "0"],
    ["Stream", "Stream::Null()"],
]);

exports.IDL_TYPES = new Map([]);

exports.CPP_TYPES = new Map([
    ["InputArray", "cv::_InputArray"],
    ["InputArrayOfArrays", "cv::_InputArray"],
    ["InputOutputArray", "cv::_InputOutputArray"],
    ["InputOutputArrayOfArrays", "cv::_InputOutputArray"],
    ["OutputArray", "cv::_OutputArray"],
    ["OutputArrayOfArrays", "cv::_OutputArray"],

    ["Point", "cv::Point"],
    ["Point2d", "cv::Point2d"],
    ["Rect", "cv::Rect"],
    ["Scalar", "cv::Scalar"],
    ["Size", "cv::Size"],

    ["c_string", "char*"],
    ["GInferListOutputs", "cv::detail::GInferOutputsTyped<cv::GArray<cv::GMat>>"],
    ["String", "std::string"],
    ["string", "std::string"],

    ["LayerId", "dnn::DictValue"],

    ["gapi_wip_IStreamSource_Ptr", "cv::Ptr<cv::gapi::wip::IStreamSource>"],

    ["GMetaArg", "cv::GMetaArg"],
]);

exports.ALIASES = new Map([
    ["DescriptorExtractor", "Feature2D"],
    ["FeatureDetector", "Feature2D"],
    ["GCompileArgs", "vector_GCompileArg"],
    ["GMetaArgs", "vector_GMetaArg"],
    ["GMat2", "tuple_GMat_and_GMat"],
    ["GRunArgs", "vector_GRunArg"],
    ["MatShape", "vector_int"],
    ["Prims", "vector_Prim"],
    ["Pose3DPtr", "Ptr_Pose3D"],
    ["PoseCluster3DPtr", "Ptr_PoseCluster3D"],

    // contrib not speficiying namespace
    ["cv::bgsegm::BackgroundSubtractor", "cv::BackgroundSubtractor"],
    ["cv::xfeatures2d::Feature2D", "cv::Feature2D"],
    ["cv::optflow::DenseOpticalFlow", "cv::DenseOpticalFlow"],
    ["cv::optflow::SparseOpticalFlow", "cv::SparseOpticalFlow"],
    ["cv::xphoto::Tonemap", "cv::Tonemap"],
]);

exports.CLASS_PTR = new Set([
    "cv::cuda::GpuMat",
    "cv::cuda::GpuMat::Allocator",
]);

exports.PTR = new Set([
    "void*",
    "uchar*",
]);

exports.CUSTOM_CLASSES = [
    ["cv.gapi.wip.draw.Prim", ["/Simple", "/DC"]],
    ["cv.gapi.wip.IStreamSource", []],
    ["cv.GProtoInputArgs", ["/Simple"]],
    ["cv.GProtoOutputArgs", ["/Simple"]],
    ["cv.flann.IndexParams", ["/Simple"]],
    ["cv.flann.SearchParams", ["/Simple"]],
];

if (semver.gt(global.OpenCV_VERSION.slice("opencv-".length), "4.5.0")) {
    exports.CUSTOM_CLASSES.push(...[
        ["cv.detail.ExtractArgsCallback", ["/Simple"]],
        ["cv.detail.ExtractMetaCallback", ["/Simple"]],
    ]);
} else {
    exports.CUSTOM_CLASSES.push(...[
        ["cv.GMatDesc", ["/Simple"]],
        ["cv.GScalarDesc", ["/Simple"]],
        ["cv.GArrayDesc", ["/Simple"]],
        ["cv.GOpaqueDesc", ["/Simple"]],
    ]);
}

exports.IGNORED_CLASSES = new Set([]);
