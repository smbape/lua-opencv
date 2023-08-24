module.exports = ({ self, self_get, shared_ptr }) => {
    const declarations = [];

    for (const [tp, type] of [
        ["float", "cv::Rect2f"],
        ["double", "cv::Rect2d"],
        ["int", "cv::Rect"],
    ]) {
        const path = type.split("::");
        const cname = path.join(".");
        const name = path[path.length - 1];
        const ctor = path.concat([name]).join(".");

        declarations.push([`class ${ cname }`, "", ["/Simple"], [
            // Public Attributes
            [tp, "x", "", ["/RW"]],
            [tp, "y", "", ["/RW"]],
            [tp, "width", "", ["/RW"]],
            [tp, "height", "", ["/RW"]],
            [tp, "1", "", ["/RW", "=x", "/Key"]],
            [tp, "2", "", ["/RW", "=y", "/Key"]],
            [tp, "3", "", ["/RW", "=width", "/Key"]],
            [tp, "4", "", ["/RW", "=height", "/Key"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [], "", ""]);

        declarations.push([ctor, "", [], [
            [tp, "x", "", []],
            [tp, "y", "", []],
            [tp, "width", "", []],
            [tp, "height", "", []],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type, "r", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type.replace("Rect", "Point"), "org", "", ["/Ref", "/C"]],
            [type.replace("Rect", "Size"), "sz", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type.replace("Rect", "Point"), "pt1", "", ["/Ref", "/C"]],
            [type.replace("Rect", "Point"), "pt2", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.tl`, type.replace("Rect", "Point"), [], [], "", ""]);
        declarations.push([`${ cname }.br`, type.replace("Rect", "Point"), [], [], "", ""]);
        declarations.push([`${ cname }.size`, type.replace("Rect", "Size"), [], [], "", ""]);
        declarations.push([`${ cname }.area`, tp, [], [], "", ""]);
        declarations.push([`${ cname }.empty`, "bool", [], [], "", ""]);

        declarations.push([`${ cname }.contains`, "bool", [], [
            [type.replace("Rect", "Point"), "pt", "", ["/Ref", "/C"]],
        ], "", ""]);
    }

    return declarations;
};
