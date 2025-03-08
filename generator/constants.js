exports.SIMPLE_ARGTYPE_DEFAULTS = new Map([
    ["bool", "0"],
    ["size_t", "0"],
    ["std::size_t", "0"],
    ["SSIZE_T", "0"],
    ["ssize_t", "0"],
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

    ["GArray", "cv::GArray"],
    ["GMetaArg", "cv::GMetaArg"],
    ["GOpaque", "cv::GOpaque"],
    ["GRunArg", "cv::GRunArg"],
    ["Prim", "cv::gapi::wip::draw::Prim"],
]);

exports.ALIASES = new Map([
    ["cv::InputArray", "InputArray"],
    ["cv::InputArrayOfArrays", "InputArrayOfArrays"],
    ["cv::InputOutputArray", "InputOutputArray"],
    ["cv::InputOutputArrayOfArrays", "InputOutputArrayOfArrays"],
    ["cv::OutputArray", "OutputArray"],
    ["cv::OutputArrayOfArrays", "OutputArrayOfArrays"],

    ["DescriptorExtractor", "Feature2D"],
    ["FeatureDetector", "Feature2D"],
    ["GCompileArgs", "std::vector<GCompileArg>"],
    ["GMetaArgs", "std::vector<GMetaArg>"],
    ["GMat2", "std::tuple<GMat_and_GMat>"],
    ["GRunArgs", "std::vector<GRunArg>"],
    ["MatShape", "std::vector<int>"],
    ["Prims", "std::vector<Prim>"],
    ["Pose3DPtr", "cv::Ptr<Pose3D>"],
    ["PoseCluster3DPtr", "cv::Ptr<PoseCluster3D>"],

    // wrong namespace
    ["cv::cuda::cuda::StereoBeliefPropagation", "cv::cuda::StereoBeliefPropagation"],

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
    ["cv.GProtoInputArgs", ["/Simple"]],            // TODO : implement
    ["cv.GProtoOutputArgs", ["/Simple"]],           // TODO : implement
    ["cv.detail.ExtractArgsCallback", ["/Simple"]], // TODO : implement
    ["cv.detail.ExtractMetaCallback", ["/Simple"]], // TODO : implement
];

exports.TEMPLATED_TYPES = new Set([
    "cv::GArray",
    "cv::GOpaque",
]);

exports.IGNORED_CLASSES = new Set([]);
