module.exports = [
    ["cv.format", "void", ["/Output=ouput << cv::format(mtx, fmt)"], [
        ["std::string", "ouput", "", ["/O", "/Ref"]],
        ["InputArray", "mtx", "", []],
        ["Formatter::FormatType", "fmt", "Formatter::FMT_NUMPY", []],
    ], "", ""],

    ["cv.read", "void", ["=readMat"], [
        ["FileNode", "node", "", []],
        ["Mat", "mat", "", ["/O"]],
        ["Mat", "default_mat", "Mat()", []],
    ], "", ""],

    ["cv.read", "void", ["=readInt"], [
        ["FileNode", "node", "", []],
        ["int", "value", "", ["/O"]],
        ["int", "default_value", "", []],
    ], "", ""],

    ["cv.read", "void", ["=readFloat"], [
        ["FileNode", "node", "", []],
        ["float", "value", "", ["/O"]],
        ["float", "default_value", "", []],
    ], "", ""],

    ["cv.read", "void", ["=readDouble"], [
        ["FileNode", "node", "", []],
        ["double", "value", "", ["/O"]],
        ["double", "default_value", "", []],
    ], "", ""],

    ["cv.read", "void", ["=readString"], [
        ["FileNode", "node", "", []],
        ["String", "value", "", ["/O"]],
        ["String", "default_value", "", []],
    ], "", ""],

    ["cv.read", "void", ["=readKeyPoint"], [
        ["FileNode", "node", "", []],
        ["KeyPoint", "value", "", ["/O"]],
        ["KeyPoint", "default_value", "", []],
    ], "", ""],

    ["cv.read", "void", ["=readDMatch"], [
        ["FileNode", "node", "", []],
        ["DMatch", "value", "", ["/O"]],
        ["DMatch", "default_value", "", []],
    ], "", ""],

    ["cv.gemm", "void", ["=dot", "/Expr=src1, src2, 1.0, cv::noArray(), 0.0, dst"], [
        ["InputArray", "src1", "", []],
        ["InputArray", "src2", "", []],
        ["OutputArray", "dst", "", []],
    ], "", ""],

    ["cv.redirectError", "void", ["/Call=::cvextra::redirectError"], [
        ["lua_State*", "L", "", []],
        ["Function", "errCallback", "", ["/Ref", "/C"]],
        ["Object", "userdata", "lua_nil", ["/Ref", "/C"]],
    ], "", ""],

    ["cv.setMouseCallback", "void", ["/Call=::cvextra::setMouseCallback"], [
        ["lua_State*", "L", "", []],
        ["std::string", "winname", "", ["/Ref", "/C"]],
        ["Function", "onMouse", "", ["/Ref", "/C"]],
        ["Object", "userdata", "lua_nil", ["/Ref", "/C"]],
    ], "", ""],

    ["cv.createButton", "int", ["/Call=::cvextra::createButton"], [
        ["lua_State*", "L", "", []],
        ["std::string", "bar_name", "", ["/Ref", "/C"]],
        ["Function", "onChange", "", ["/Ref", "/C"]],
        ["Object", "userdata", "lua_nil", ["/Ref", "/C"]],
        ["int", "type", "QT_PUSH_BUTTON", []],
        ["bool", "initial_button_state", "false", []],
    ], "", ""],

    ["cv.createTrackbar", "int", ["/Call=::cvextra::createTrackbar"], [
        ["lua_State*", "L", "", []],
        ["std::string", "trackbarname", "", ["/Ref", "/C"]],
        ["std::string", "winname", "", ["/Ref", "/C"]],
        ["int", "value", "", []],
        ["int", "count", "", []],
        ["Function", "onChange", "", ["/Ref", "/C"]],
        ["Object", "userdata", "lua_nil", ["/Ref", "/C"]],
    ], "", ""],

    ["cv.createTrackbar", "int", ["/Call=::cvextra::createTrackbar"], [
        ["lua_State*", "L", "", []],
        ["std::string", "trackbarname", "", ["/Ref", "/C"]],
        ["std::string", "winname", "", ["/Ref", "/C"]],
        ["int", "value", "", []],
        ["int", "count", "", []],
    ], "", ""],

    ["cv.", "", ["/Properties"], [
        ["int", "CV_8U", "", ["/RExpr=CV_8U", "/C"]],
        ["int", "CV_8S", "", ["/RExpr=CV_8S", "/C"]],
        ["int", "CV_16U", "", ["/RExpr=CV_16U", "/C"]],
        ["int", "CV_16S", "", ["/RExpr=CV_16S", "/C"]],
        ["int", "CV_32S", "", ["/RExpr=CV_32S", "/C"]],
        ["int", "CV_32F", "", ["/RExpr=CV_32F", "/C"]],
        ["int", "CV_64F", "", ["/RExpr=CV_64F", "/C"]],
        ["int", "CV_16F", "", ["/RExpr=CV_16F", "/C"]],
        ["int", "CV_16BF", "", ["/RExpr=CV_16BF", "/C"]],
        ["int", "CV_Bool", "", ["/RExpr=CV_Bool", "/C"]],
        ["int", "CV_64U", "", ["/RExpr=CV_64U", "/C"]],
        ["int", "CV_64S", "", ["/RExpr=CV_64S", "/C"]],
        ["int", "CV_32U", "", ["/RExpr=CV_32U", "/C"]],
        ["int", "CV_DEPTH_CURR_MAX", "", ["/RExpr=CV_DEPTH_CURR_MAX", "/C"]],

        ["int", "CV_MAT_DEPTH_MASK", "", ["/RExpr=CV_MAT_DEPTH_MASK", "/C"]],

        ["int", "CV_8UC1", "", ["/RExpr=CV_8UC1", "/C"]],
        ["int", "CV_8UC2", "", ["/RExpr=CV_8UC2", "/C"]],
        ["int", "CV_8UC3", "", ["/RExpr=CV_8UC3", "/C"]],
        ["int", "CV_8UC4", "", ["/RExpr=CV_8UC4", "/C"]],

        ["int", "CV_8SC1", "", ["/RExpr=CV_8SC1", "/C"]],
        ["int", "CV_8SC2", "", ["/RExpr=CV_8SC2", "/C"]],
        ["int", "CV_8SC3", "", ["/RExpr=CV_8SC3", "/C"]],
        ["int", "CV_8SC4", "", ["/RExpr=CV_8SC4", "/C"]],

        ["int", "CV_16UC1", "", ["/RExpr=CV_16UC1", "/C"]],
        ["int", "CV_16UC2", "", ["/RExpr=CV_16UC2", "/C"]],
        ["int", "CV_16UC3", "", ["/RExpr=CV_16UC3", "/C"]],
        ["int", "CV_16UC4", "", ["/RExpr=CV_16UC4", "/C"]],

        ["int", "CV_16SC1", "", ["/RExpr=CV_16SC1", "/C"]],
        ["int", "CV_16SC2", "", ["/RExpr=CV_16SC2", "/C"]],
        ["int", "CV_16SC3", "", ["/RExpr=CV_16SC3", "/C"]],
        ["int", "CV_16SC4", "", ["/RExpr=CV_16SC4", "/C"]],

        ["int", "CV_32SC1", "", ["/RExpr=CV_32SC1", "/C"]],
        ["int", "CV_32SC2", "", ["/RExpr=CV_32SC2", "/C"]],
        ["int", "CV_32SC3", "", ["/RExpr=CV_32SC3", "/C"]],
        ["int", "CV_32SC4", "", ["/RExpr=CV_32SC4", "/C"]],

        ["int", "CV_32FC1", "", ["/RExpr=CV_32FC1", "/C"]],
        ["int", "CV_32FC2", "", ["/RExpr=CV_32FC2", "/C"]],
        ["int", "CV_32FC3", "", ["/RExpr=CV_32FC3", "/C"]],
        ["int", "CV_32FC4", "", ["/RExpr=CV_32FC4", "/C"]],

        ["int", "CV_64FC1", "", ["/RExpr=CV_64FC1", "/C"]],
        ["int", "CV_64FC2", "", ["/RExpr=CV_64FC2", "/C"]],
        ["int", "CV_64FC3", "", ["/RExpr=CV_64FC3", "/C"]],
        ["int", "CV_64FC4", "", ["/RExpr=CV_64FC4", "/C"]],

        ["int", "CV_16FC1", "", ["/RExpr=CV_16FC1", "/C"]],
        ["int", "CV_16FC2", "", ["/RExpr=CV_16FC2", "/C"]],
        ["int", "CV_16FC3", "", ["/RExpr=CV_16FC3", "/C"]],
        ["int", "CV_16FC4", "", ["/RExpr=CV_16FC4", "/C"]],

        ["int", "CV_64SC1", "", ["/RExpr=CV_64SC1", "/C"]],
        ["int", "CV_64SC2", "", ["/RExpr=CV_64SC2", "/C"]],
        ["int", "CV_64SC3", "", ["/RExpr=CV_64SC3", "/C"]],
        ["int", "CV_64SC4", "", ["/RExpr=CV_64SC4", "/C"]],

        ["int", "CV_64UC1", "", ["/RExpr=CV_64UC1", "/C"]],
        ["int", "CV_64UC2", "", ["/RExpr=CV_64UC2", "/C"]],
        ["int", "CV_64UC3", "", ["/RExpr=CV_64UC3", "/C"]],
        ["int", "CV_64UC4", "", ["/RExpr=CV_64UC4", "/C"]],

        ["int", "CV_BoolC1", "", ["/RExpr=CV_BoolC1", "/C"]],
        ["int", "CV_BoolC2", "", ["/RExpr=CV_BoolC2", "/C"]],
        ["int", "CV_BoolC3", "", ["/RExpr=CV_BoolC3", "/C"]],
        ["int", "CV_BoolC4", "", ["/RExpr=CV_BoolC4", "/C"]],

        ["int", "CV_32UC1", "", ["/RExpr=CV_32UC1", "/C"]],
        ["int", "CV_32UC2", "", ["/RExpr=CV_32UC2", "/C"]],
        ["int", "CV_32UC3", "", ["/RExpr=CV_32UC3", "/C"]],
        ["int", "CV_32UC4", "", ["/RExpr=CV_32UC4", "/C"]],

        ["int", "CV_16BFC1", "", ["/RExpr=CV_16BFC1", "/C"]],
        ["int", "CV_16BFC2", "", ["/RExpr=CV_16BFC2", "/C"]],
        ["int", "CV_16BFC3", "", ["/RExpr=CV_16BFC3", "/C"]],
        ["int", "CV_16BFC4", "", ["/RExpr=CV_16BFC4", "/C"]],

        ["int", "TM_EXACT", "", ["/RExpr=-1", "/C"]],
    ], "", ""],

    ["cv.CV_MAT_DEPTH", "int", ["/Call=CV_MAT_DEPTH"], [
        ["int", "flags", "", []],
    ], "", ""],

    ["cv.CV_IS_INT_TYPE", "int", ["/Call=CV_IS_INT_TYPE"], [
        ["int", "flags", "", []],
    ], "", ""],

    ["cv.CV_IS_FLOAT_TYPE", "int", ["/Call=CV_IS_FLOAT_TYPE"], [
        ["int", "flags", "", []],
    ], "", ""],

    ["cv.CV_MAKETYPE", "int", ["/Call=CV_MAKETYPE"], [
        ["int", "depth", "", []],
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_MAKE_TYPE", "int", ["/Call=CV_MAKE_TYPE"], [
        ["int", "depth", "", []],
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_8UC", "int", ["/Call=CV_8UC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_8SC", "int", ["/Call=CV_8SC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_16UC", "int", ["/Call=CV_16UC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_16SC", "int", ["/Call=CV_16SC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_32SC", "int", ["/Call=CV_32SC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_32FC", "int", ["/Call=CV_32FC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_64FC", "int", ["/Call=CV_64FC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_16FC", "int", ["/Call=CV_16FC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_64SC", "int", ["/Call=CV_64SC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_64SC", "int", ["/Call=CV_64SC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_BoolC", "int", ["/Call=CV_BoolC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_32UC", "int", ["/Call=CV_32UC"], [
        ["int", "cn", "", []],
    ], "", ""],

    ["cv.CV_16BFC", "int", ["/Call=CV_16BFC"], [
        ["int", "cn", "", []],
    ], "", ""],
];
