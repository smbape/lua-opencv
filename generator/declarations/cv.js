module.exports = [
    ["class cv._InputArray", "", [], [], "", ""],
    ["class cv._OutputArray", "", [], [], "", ""],
    ["class cv._InputOutputArray", "", [], [], "", ""],

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

    // ["cv.FileNode.asVariant", "_variant_t", ["/Call=::autoit::fileNodeAsVariant", "/Expr=*__self->get()"], [], "", ""],

    ["cv.", "", ["/Properties"], [
        ["int", "CV_8U", "", ["/RExpr=CV_8U"]],
        ["int", "CV_8S", "", ["/RExpr=CV_8S"]],
        ["int", "CV_16U", "", ["/RExpr=CV_16U"]],
        ["int", "CV_16S", "", ["/RExpr=CV_16S"]],
        ["int", "CV_32S", "", ["/RExpr=CV_32S"]],
        ["int", "CV_32F", "", ["/RExpr=CV_32F"]],
        ["int", "CV_64F", "", ["/RExpr=CV_64F"]],
        ["int", "CV_16F", "", ["/RExpr=CV_16F"]],

        ["int", "CV_MAT_DEPTH_MASK", "", ["/RExpr=CV_MAT_DEPTH_MASK"]],

        ["int", "CV_8UC1", "", ["/RExpr=CV_8UC1"]],
        ["int", "CV_8UC2", "", ["/RExpr=CV_8UC2"]],
        ["int", "CV_8UC3", "", ["/RExpr=CV_8UC3"]],
        ["int", "CV_8UC4", "", ["/RExpr=CV_8UC4"]],

        ["int", "CV_8SC1", "", ["/RExpr=CV_8SC1"]],
        ["int", "CV_8SC2", "", ["/RExpr=CV_8SC2"]],
        ["int", "CV_8SC3", "", ["/RExpr=CV_8SC3"]],
        ["int", "CV_8SC4", "", ["/RExpr=CV_8SC4"]],

        ["int", "CV_16UC1", "", ["/RExpr=CV_16UC1"]],
        ["int", "CV_16UC2", "", ["/RExpr=CV_16UC2"]],
        ["int", "CV_16UC3", "", ["/RExpr=CV_16UC3"]],
        ["int", "CV_16UC4", "", ["/RExpr=CV_16UC4"]],

        ["int", "CV_16SC1", "", ["/RExpr=CV_16SC1"]],
        ["int", "CV_16SC2", "", ["/RExpr=CV_16SC2"]],
        ["int", "CV_16SC3", "", ["/RExpr=CV_16SC3"]],
        ["int", "CV_16SC4", "", ["/RExpr=CV_16SC4"]],

        ["int", "CV_32SC1", "", ["/RExpr=CV_32SC1"]],
        ["int", "CV_32SC2", "", ["/RExpr=CV_32SC2"]],
        ["int", "CV_32SC3", "", ["/RExpr=CV_32SC3"]],
        ["int", "CV_32SC4", "", ["/RExpr=CV_32SC4"]],

        ["int", "CV_32FC1", "", ["/RExpr=CV_32FC1"]],
        ["int", "CV_32FC2", "", ["/RExpr=CV_32FC2"]],
        ["int", "CV_32FC3", "", ["/RExpr=CV_32FC3"]],
        ["int", "CV_32FC4", "", ["/RExpr=CV_32FC4"]],

        ["int", "CV_64FC1", "", ["/RExpr=CV_64FC1"]],
        ["int", "CV_64FC2", "", ["/RExpr=CV_64FC2"]],
        ["int", "CV_64FC3", "", ["/RExpr=CV_64FC3"]],
        ["int", "CV_64FC4", "", ["/RExpr=CV_64FC4"]],

        ["int", "CV_16FC1", "", ["/RExpr=CV_16FC1"]],
        ["int", "CV_16FC2", "", ["/RExpr=CV_16FC2"]],
        ["int", "CV_16FC3", "", ["/RExpr=CV_16FC3"]],
        ["int", "CV_16FC4", "", ["/RExpr=CV_16FC4"]],
    ], "", ""],

    ["cv.CV_MAT_DEPTH", "int", ["/Call=CV_MAT_DEPTH"], [
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
];
