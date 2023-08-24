module.exports = ({ self, self_get, shared_ptr }) => {
    const declarations = [];

    for (const [tp, type] of [
        ["float", "cv::Size2f"],
        ["double", "cv::Size2d"],
        ["int", "cv::Size"],
    ]) {
        const path = type.split("::");
        const cname = path.join(".");
        const name = path[path.length - 1];
        const ctor = path.concat([name]).join(".");

        declarations.push([`class ${ cname }`, "", ["/Simple"], [
            // Public Attributes
            [tp, "width", "", ["/RW"]],
            [tp, "height", "", ["/RW"]],
            [tp, "1", "", ["/RW", "=width", "/Key"]],
            [tp, "2", "", ["/RW", "=height", "/Key"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [], "", ""]);

        declarations.push([ctor, "", [], [
            [tp, "width", "", []],
            [tp, "height", "", []],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [type, "sz", "", ["/Ref", "/C"]],
        ], "", ""]);

        declarations.push([`${ cname }.area`, tp, [], [], "", ""]);
        declarations.push([`${ cname }.aspectRatio`, "double", [], [], "", ""]);
        declarations.push([`${ cname }.empty`, "bool", [], [], "", ""]);
    }

    return declarations;
};
