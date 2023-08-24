module.exports = ({ self, self_get, shared_ptr }) => {
    const declarations = [];

    for (const [tp, type] of [
        ["float", "cv::Point2f"],
        ["double", "cv::Point2d"],
        ["int", "cv::Point"],
    ]) {
        const path = type.split("::");
        const cname = path.join(".");
        const name = path[path.length - 1];
        const ctor = path.concat([name]).join(".");

        declarations.push([`class ${ cname }`, "", ["/Simple"], [
            // Public Attributes
            [tp, "x", "", ["/RW"]],
            [tp, "y", "", ["/RW"]],
            [tp, "1", "", ["/RW", "=x", "/Key"]],
            [tp, "2", "", ["/RW", "=y", "/Key"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [], "", ""]);

        declarations.push([ctor, "", [], [
            [tp, "x", "", []],
            [tp, "y", "", []],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type.replace("Point", "Size"), "sz", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type === "cv::Point" ? "cv::Vec2i" : type.replace("Point", "Vec"), "v", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.dot`, tp, [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.ddot`, "double", [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.cross`, "double", [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.inside`, "bool", [], [
            [type.replace("Point", "Rect"), "r", "", ["/Ref", "/C"]],
        ], "", ""]);
    }

    for (const [tp, type] of [
        ["float", "cv::Point3f"],
        ["double", "cv::Point3d"],
        ["int", "cv::Point3i"],
    ]) {
        const path = type.split("::");
        const cname = path.join(".");
        const name = path[path.length - 1];
        const ctor = path.concat([name]).join(".");

        declarations.push([`class ${ cname }`, "", ["/Simple"], [
            // Public Attributes
            [tp, "x", "", ["/RW"]],
            [tp, "y", "", ["/RW"]],
            [tp, "z", "", ["/RW"]],
            [tp, "1", "", ["/RW", "=x", "/Key"]],
            [tp, "2", "", ["/RW", "=y", "/Key"]],
            [tp, "3", "", ["/RW", "=z", "/Key"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [], "", ""]);

        declarations.push([ctor, "", [], [
            [tp, "x", "", []],
            [tp, "y", "", []],
            [tp, "z", "", []],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type.replace("Point", "Vec"), "v", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.dot`, tp, [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.ddot`, "double", [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.cross`, type, [], [
            [type, "pt", "", ["/Ref", "/C"]],
        ], "", ""]);
    }

    return declarations;
};
