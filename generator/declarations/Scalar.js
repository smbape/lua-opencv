module.exports = ({ self, self_get, shared_ptr }) => {
    const declarations = [];

    for (const [tp, type] of [
        ["double", "cv::Scalar"],
    ]) {
        const path = type.split("::");
        const cname = path.join(".");
        const name = path[path.length - 1];
        const ctor = path.concat([name]).join(".");

        declarations.push([`class ${ cname }`, "", ["/Simple"], [
            // Public Attributes
            [tp, "1", "", ["/RW", "=operator[](0)", "/Key"]],
            [tp, "2", "", ["/RW", "=operator[](1)", "/Key"]],
            [tp, "3", "", ["/RW", "=operator[](2)", "/Key"]],
            [tp, "4", "", ["/RW", "=operator[](3)", "/Key"]],
        ], "", ""]);

        declarations.push([ctor, "", [], [
            [tp, "v0", "0", []],
            [tp, "v1", "0", []],
            [tp, "v2", "0", []],
            [tp, "v3", "0", []],
        ], "", ""]);

        declarations.push([`${ cname }.all`, type, ["/S"], [
            [tp, "v0", "", []],
        ], "", ""]);

        declarations.push([`${ cname }.mul`, type, [], [
            [type, "a", "", []],
            ["double", "scale", "1", []],
        ], "", ""]);

        declarations.push([`${ cname }.conj`, type, [], [], "", ""]);
        declarations.push([`${ cname }.isReal`, "bool", [], [], "", ""]);
    }

    return declarations;
};
