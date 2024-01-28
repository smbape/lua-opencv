module.exports = [
    ["class cv.Range", "", ["/Simple"], [
        ["int", "start", "", ["/RW"]],
        ["int", "end", "", ["/RW"]],
        ["cv::Range", "Ellipsis", "", ["/R", "/S", "/RExpr=cvextra::Ellipsis"]],
    ], "", ""],

    ["cv.Range.Range", "", [], [], "", ""],

    ["cv.Range.Range", "", [], [
        ["int", "start", "", []],
        ["int", "end", "", []],
    ], "", ""],

    ["cv.Range.size", "int", [], [], "", ""],
    ["cv.Range.empty", "bool", [], [], "", ""],

    ["cv.Range.all", "Range", ["/S"], [], "", ""],
];
