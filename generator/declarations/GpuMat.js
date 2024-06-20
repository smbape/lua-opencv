module.exports = ({ self, self_get, shared_ptr }) => [
    ["cv.cuda.GpuMat.", "", ["/Properties"], [
        // Custom Attributes

        ["int", "width", "", ["/RW", "=cols"]],
        ["int", "height", "", ["/RW", "=rows"]],
    ], "", ""],

    // Extended Functions

    ["cv.cuda.GpuMat.sum", "cv::Scalar", ["/Call=cv::sum", `/Expr=${ self }`], [], "", ""],

    ["cv.cuda.GpuMat.operator+", "void", ["/S", "/Call=cv::add"], [
        ["cv::cuda::GpuMat", "src1", "", ["/Ref", "/C"]],
        ["InputArray", "src2", "", []],
        ["cv::cuda::GpuMat", "dst", "cv::cuda::GpuMat()", ["/O"]],
    ], "", ""],

    ["cv.cuda.GpuMat.operator-", "void", ["/S", "/Call=cv::subtract"], [
        ["cv::cuda::GpuMat", "src1", "", ["/Ref", "/C"]],
        ["InputArray", "src2", "", []],
        ["cv::cuda::GpuMat", "dst", "cv::cuda::GpuMat()", ["/O"]],
    ], "", ""],

    ["cv.cuda.GpuMat.operator*", "void", ["/S", "/Call=cv::multiply"], [
        ["cv::cuda::GpuMat", "src1", "", ["/Ref", "/C"]],
        ["InputArray", "src2", "", []],
        ["cv::cuda::GpuMat", "dst", "cv::cuda::GpuMat()", ["/O"]],
    ], "", ""],

    ["cv.cuda.GpuMat.operator/", "void", ["/S", "/Call=cv::divide"], [
        ["cv::cuda::GpuMat", "src1", "", ["/Ref", "/C"]],
        ["InputArray", "src2", "", []],
        ["cv::cuda::GpuMat", "dst", "cv::cuda::GpuMat()", ["/O"]],
    ], "", ""],

    ["cv.cuda.GpuMat.operator==", "void", ["/S", "/Call=cv::compare", "/Expr=$0, cv::CMP_EQ"], [
        ["cv::cuda::GpuMat", "src1", "", ["/Ref", "/C"]],
        ["InputArray", "src2", "", []],
        ["cv::cuda::GpuMat", "dst", "cv::cuda::GpuMat()", ["/O"]],
    ], "", ""],

    ["cv.cuda.GpuMat.operator<", "void", ["/S", "/Call=cv::compare", "/Expr=$0, cv::CMP_LT"], [
        ["cv::cuda::GpuMat", "src1", "", ["/Ref", "/C"]],
        ["InputArray", "src2", "", []],
        ["cv::cuda::GpuMat", "dst", "cv::cuda::GpuMat()", ["/O"]],
    ], "", ""],

    ["cv.cuda.GpuMat.operator<=", "void", ["/S", "/Call=cv::compare", "/Expr=$0, cv::CMP_LE"], [
        ["cv::cuda::GpuMat", "src1", "", ["/Ref", "/C"]],
        ["InputArray", "src2", "", []],
        ["cv::cuda::GpuMat", "dst", "cv::cuda::GpuMat()", ["/O"]],
    ], "", ""],

    ["cv.cuda.GpuMat.createFromArray", "GpuMat", ["/S", "/Call=::cvextra::createGpuMatFromArray", "/Expr=L, $0"], [
        ["Table", "array", "", ["/Ref", "/C"]],
        ["int", "type", "CV_64F", []],
        ["GpuMat::Allocator*", "allocator", "GpuMat::defaultAllocator()", []]
    ], "", ""],

    ["cv.cuda.GpuMat.table", "void", ["/Call=::cvextra::pushtable_GpuMat", `/Expr=L, ${ self }, $0`], [
        ["bool", "nested", "false", []],
    ], "", ""],
];
