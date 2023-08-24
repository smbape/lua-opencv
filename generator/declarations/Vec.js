module.exports = ({ self, self_get, shared_ptr }) => {
    const cpptypes = new Map([
        ["b", "uchar"],
        ["s", "short"],
        ["w", "ushort"],
        ["i", "int"],
        ["f", "float"],
        ["d", "double"],
    ]);

    const types = [];

    for (const _Tp of ["b", "s", "w"]) {
        for (const cn of [2, 3, 4]) { // eslint-disable-line no-magic-numbers
            types.push(`cv::Vec${ cn }${ _Tp }`);
        }
    }

    for (const cn of [2, 3, 4, 6, 8]) { // eslint-disable-line no-magic-numbers
        types.push(`cv::Vec${ cn }i`);
    }

    for (const _Tp of ["f", "d"]) {
        for (const cn of [2, 3, 4, 6]) { // eslint-disable-line no-magic-numbers
            types.push(`cv::Vec${ cn }${ _Tp }`);
        }
    }

    const declarations = [];

    for (const type of types) {
        const path = type.split("::");
        const cname = path.join(".");
        const name = path[path.length - 1];
        const ctor = path.concat([name]).join(".");

        const tp = cpptypes.get(type[type.length - 1]);
        const cn = parseInt(type[type.length - 2], 10);

        declarations.push([`class ${ cname }`, "", ["/Simple"], [...Array(cn).keys()].map(i => [tp, String(i + 1), "", ["/RW", `=operator[](${ i })`, "/Key"]]), "", ""]);

        declarations.push([ctor, "", [], [], "", ""]);

        declarations.push([ctor, "", [], [
            [`std::vector<${ tp }>`, "values", "", ["/Ref", "/C", "/Expr=values.data()"]],
        ], "", ""]);

        declarations.push([`${ cname }.all`, type, ["/S"], [
            [tp, "alpha", "", []],
        ], "", ""]);

        declarations.push([`${ cname }.ones`, type, ["/S"], [], "", ""]);
        declarations.push([`${ cname }.zeros`, type, ["/S"], [], "", ""]);

        declarations.push([`${ cname }.randn`, type, ["/S"], [
            [tp, "a", "", []],
            [tp, "b", "", []],
        ], "", ""]);

        declarations.push([`${ cname }.randu`, type, ["/S"], [
            [tp, "a", "", []],
            [tp, "b", "", []],
        ], "", ""]);

        declarations.push([`${ cname }.mul`, type, [], [
            [type, "v", "", ["/Ref", "/C"]],
        ], "", ""]);

        if (cn === 3) {
            declarations.push([`${ cname }.cross`, type, [], [
                [type, "v", "", ["/Ref", "/C"]],
            ], "", ""]);
        }
    }

    return declarations;
};
