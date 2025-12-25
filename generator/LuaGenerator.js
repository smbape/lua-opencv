/* eslint-disable no-magic-numbers */
const fs = require("node:fs");
const sysPath = require("node:path");
const waterfall = require("async/waterfall");

const knwon_ids = require("./ids");
const FileUtils = require("./FileUtils");
const {
    makeExpansion,
    useNamespaces,
    getTypeDef,
    removeConstQualifiers
} = require("./alias");
const {
    PTR,
    SIMPLE_ARGTYPE_DEFAULTS
} = require("./constants");

const CoClass = require("./CoClass");
const vectors = require("./vectors");

const proto = {
    preprocess(config, options) {
        for (const vector of vectors) {
            this.add_vector(vector, null, options);
        }

        if (options.vectors) {
            for (const vector of options.vectors) {
                this.add_vector(vector, null, options);
            }
        }
    },

    makeDependent(type, coclass, options) {
        const cpptype = this.getCppType(type, coclass, options);

        if (cpptype.startsWith("std::map<")) {
            this.add_map(cpptype, coclass, options);
        } else if (cpptype.startsWith("std::vector<")) {
            this.add_vector(cpptype, coclass, options);
        } else if (cpptype.includes("<") && cpptype.endsWith(">")) {
            const pos = cpptype.indexOf("<");
            const types = CoClass.getTupleTypes(cpptype.slice(pos + 1, -">".length));
            for (const itype of types) {
                this.makeDependent(itype, coclass, options);
            }
        }

        if (typeof options.makeDependent === "function") {
            options.makeDependent(this, cpptype, coclass, options);
        }
    },

    add_map(cpptype, parent, options) {
        if (cpptype.endsWith("*")) {
            cpptype = cpptype.replace(/\*+$/, "");
        }

        if (!cpptype.startsWith("std::map<") || !cpptype.endsWith(">")) {
            throw new Error(`invalid map type ${ cpptype }`);
        }

        const fqn = getTypeDef(cpptype, options);
        if (this.classes.has(fqn) && this.getCoClass(fqn, options).is_stdmap) {
            return;
        }

        const [key_type, value_type] = CoClass.getTupleTypes(cpptype.slice("std::map<".length, -">".length));

        const { self } = options;

        this.typedefs.set(fqn, cpptype);

        const coclass = this.getCoClass(fqn, options);
        coclass.include = parent;
        coclass.is_simple = true;
        coclass.is_class = true;
        coclass.is_stdmap = true;

        coclass.addMethod([`${ fqn }.${ coclass.name }`, "", [], [], "", ""], options);

        coclass.addMethod([`${ fqn }.${ coclass.name }`, "", [], [
            [cpptype, "other", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.new`, `std::shared_ptr<${ coclass.name }>`, ["/Call=lua_map_new", "/S"], [
            [`std::vector<std::pair<${ key_type }, ${ value_type }>>`, "pairs", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.table`, "void", ["/Call=lua_push", `/Expr=L, ${ self }`], [], "", ""], options);

        coclass.addMethod([`${ fqn }.size`, "size_t", ["=sol::meta_function::length"], [], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::index`, "void", ["/Call=lua_map_method__index", `/Expr=L, ${ self }, $0`], [
            [key_type, "key", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.insert_or_assign`, "void", ["=sol::meta_function::new_index"], [
            [key_type, "key", "", []],
            [value_type, "value", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.erase`, "size_t", [], [
            [key_type, "key", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.erase`, "size_t", ["=delete"], [
            [key_type, "key", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.erase`, "size_t", ["=remove"], [
            [key_type, "key", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.swap`, "void", [], [
            [cpptype, "other", "", ["/Ref"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.operator=`, "void", ["=copy"], [
            [cpptype, "other", "", ["/Ref", "/C"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.merge`, "void", [], [
            [fqn, "other", "", []],
        ], "", ""], options);

        // Lookup
        coclass.addMethod([`${ fqn }.count`, "size_t", [], [
            [key_type, "key", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.count`, "bool", ["=contains", "/WrapAs=static_cast<bool>"], [
            [key_type, "key", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.count`, "bool", ["=has", "/WrapAs=static_cast<bool>"], [
            [key_type, "key", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.keys`, "void", ["/Call=lua_map_method_keys", `/Expr=${ self }, $0`], [
            [`std::vector<${ key_type }>`, "keys", "", ["/O", "/Ref"]],
        ], "", ""], options);

        this.addDependencies(coclass, options);
    },

    add_vector(cpptype, parent, options) {
        if (cpptype.endsWith("*")) {
            cpptype = cpptype.replace(/\*+$/, "");
        }

        if (!cpptype.startsWith("std::vector<") || !cpptype.endsWith(">")) {
            throw new Error(`invalid vector type ${ cpptype }`);
        }

        const fqn = getTypeDef(cpptype, options);

        if (this.classes.has(fqn) && this.getCoClass(fqn, options).is_vector) {
            return;
        }

        const vtype = cpptype.slice("std::vector<".length, -">".length);

        const { shared_ptr } = options;
        const is_ptr = vtype.endsWith("*");
        const is_shared_ptr = vtype.startsWith(`${ shared_ptr }<`);
        const is_by_ref = !is_ptr && !is_shared_ptr && this.classes.has(vtype) && !this.enums.has(vtype);

        const { self } = options;

        this.typedefs.set(fqn, cpptype);

        const coclass = this.getCoClass(fqn, options);
        coclass.include = parent;
        coclass.is_simple = true;
        coclass.is_class = true;
        coclass.is_vector = true;

        coclass.addMethod([`${ fqn }.${ coclass.name }`, "", [], [], "", ""], options);

        coclass.addMethod([`${ fqn }.${ coclass.name }`, "", [], [
            ["size_t", "size", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.${ coclass.name }`, "", [], [
            [cpptype, "other", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.${ coclass.name }`, "", [], [
            [`${ vtype }*`, "data", "", []],
            ["size_t", "count", "", ["/Expr=data + count"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.${ coclass.name }`, "", [], [
            [`${ vtype }*`, "first", "", []],
            [`${ vtype }*`, "last", "", []],
        ], "", ""], options);

        if (vtype !== "bool") {
            coclass.addMethod([`${ fqn }.data`, "void*", ["/WrapAs=static_cast<void*>"], [], "", ""], options);
            coclass.addMethod([`${ fqn }.data`, "void*", ["=ptr", "/Expr=", "/Output=$0 + i", "/WrapAs=static_cast<void*>"], [
                ["std::ptrdiff_t", "i", "0", []],
            ], "", ""], options);
            coclass.addMethod([`${ fqn }.ptr`, "void*", ["/S", "/Call=static_cast<void*>", "/Expr=$1 + $2"], [
                ["void*", "ptr", "", [`/Cast=static_cast<${ vtype }*>`]],
                ["std::ptrdiff_t", "i", "0", []],
            ], "", ""], options);
            coclass.addMethod([`${ fqn }.get`, `${ vtype }${ is_by_ref ? "*" : "" }`, ["/S", `/Call=${ is_by_ref ? "" : "*" }`, "/Expr=$1 + $2"], [
                ["void*", "ptr", "", [`/Cast=static_cast<${ vtype }*>`]],
                ["std::ptrdiff_t", "i", "", []],
            ], "", ""], options);
            coclass.addMethod([`${ fqn }.set`, "void", ["/S", "/Call=*", "/Expr=$1 + $2", "/Output=$0 = value"], [
                ["void*", "ptr", "", [`/Cast=static_cast<${ vtype }*>`]],
                ["std::ptrdiff_t", "i", "", []],
                [vtype, "value", "", []],
            ], "", ""], options);
        }

        coclass.addMethod([`${ fqn }.front`, vtype, [], [], "", ""], options);
        coclass.addMethod([`${ fqn }.back`, vtype, [], [], "", ""], options);
        coclass.addMethod([`${ fqn }.empty`, "bool", ["/C"], [], "", ""], options);
        coclass.addProperty(["size_t", "sizeof_value_type", "", ["/S", "/C", `/RExpr=sizeof(${ vtype })`]]);
        coclass.addMethod([`${ fqn }.size`, "size_t", ["=sizeof", `/Output=$0 * sizeof(${ vtype })`], [], "", ""], options);
        coclass.addMethod([`${ fqn }.max_size`, "size_t", [], [], "", ""], options);

        coclass.addMethod([`${ fqn }.reserve`, "void", [], [
            ["size_t", "new_cap", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.capacity`, "size_t", [], [], "", ""], options);
        coclass.addMethod([`${ fqn }.shrink_to_fit`, "void", [], [], "", ""], options);
        coclass.addMethod([`${ fqn }.clear`, "void", [], [], "", ""], options);
        coclass.addMethod([`${ fqn }.pop_back`, "void", [], [], "", ""], options);

        coclass.addMethod([`${ fqn }.resize`, "void", [], [
            ["size_t", "count", "", []],
        ], "", ""], options);
        coclass.addMethod([`${ fqn }.resize`, "void", [], [
            ["size_t", "count", "", []],
            [vtype, "value", "", ["/C", "/Ref"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.swap`, "void", [], [
            [cpptype, "other", "", ["/Ref"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.operator=`, "void", ["=copy"], [
            [cpptype, "other", "", ["/Ref", "/C"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::index`, `${ vtype }${ is_by_ref ? "*" : "" }`, [`/Call=${ is_by_ref ? "&" : "" }lua_vector_method__index`, `/Expr=L, ${ self }, $0`], [
            ["size_t", "index", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::new_index`, "void", ["/Call=lua_vector_method__newindex", `/Expr=L, ${ self }, $0`], [
            ["size_t", "index", "", []],
            [vtype, "value", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.table`, "void", ["/Call=lua_push", `/Expr=L, ${ self }`], [], "", ""], options);

        coclass.addMethod([`${ fqn }.size`, "size_t", ["=sol::meta_function::length"], [], "", ""], options);

        this.addDependencies(coclass, options);
    },
};

const toCamelCase = str => {
    return str.toLowerCase().replace(/(?:^([a-z])|[^a-z\d_]+([a-z])|[^a-z\d_]+$)/g, (match, begin, middle, end) => {
        if (begin) {
            return begin.toUpperCase();
        }

        if (middle) {
            return middle.toUpperCase();
        }

        return "";
    });
};

const meta_functions = new Map([
    ["sol::meta_function::index", "__index"],
    ["sol::meta_function::new_index", "__newindex"],
    ["sol::meta_function::mode", "__mode"],
    ["sol::meta_function::call", "__call"],
    ["sol::meta_function::call_function", "__call"],
    ["sol::meta_function::metatable", "__metatable", ],
    ["sol::meta_function::to_string", "__tostring", ],
    ["sol::meta_function::length", "__len", ],
    ["sol::meta_function::unary_minus", "__unm", ],
    ["sol::meta_function::addition", "__add", ],
    ["sol::meta_function::subtraction", "__sub", ],
    ["sol::meta_function::multiplication", "__mul", ],
    ["sol::meta_function::division", "__div", ],
    ["sol::meta_function::modulus", "__mod", ],
    ["sol::meta_function::power_of", "__pow", ],
    ["sol::meta_function::involution", "__pow"],
    ["sol::meta_function::concatenation", "__concat"],
    ["sol::meta_function::equal_to", "__eq"],
    ["sol::meta_function::less_than", "__lt"],
    ["sol::meta_function::less_than_or_equal_to", "__le"],
    ["sol::meta_function::garbage_collect", "__gc"],
    ["sol::meta_function::floor_division", "__idiv"],
    ["sol::meta_function::bitwise_left_shift", "__shl"],
    ["sol::meta_function::bitwise_right_shift", "__shr"],
    ["sol::meta_function::bitwise_not", "__bnot"],
    ["sol::meta_function::bitwise_and", "__band"],
    ["sol::meta_function::bitwise_or", "__bor"],
    ["sol::meta_function::bitwise_xor", "__bxor"],
    ["sol::meta_function::pairs", "__pairs"],
    ["sol::meta_function::ipairs", "__ipairs"],
]);

const meta_names = new Map(Array.from(meta_functions.entries()).map(([key, value]) => [value, key]));

const meta_operators = new Map([
    ["operator()", "sol::meta_function::call"],
    ["operator+", "sol::meta_function::addition"],
    ["operator-", "sol::meta_function::subtraction"],
    ["operator*", "sol::meta_function::multiplication"],
    ["operator/", "sol::meta_function::division"],
    ["operator~", "sol::meta_function::bitwise_not"],
    ["operator&", "sol::meta_function::bitwise_and"],
    ["operator|", "sol::meta_function::bitwise_or"],
    ["operator^", "sol::meta_function::bitwise_xor"],
    ["operator==", "sol::meta_function::equal_to"],
    ["operator<", "sol::meta_function::less_than"],
    ["operator<=", "sol::meta_function::less_than_or_equal_to"],
]);

const meta_binaries_op = new Map([
    ["sol::meta_function::addition", "+"],
    ["sol::meta_function::subtraction", "-"],
    ["sol::meta_function::multiplication", "*"],
    ["sol::meta_function::division", "/"],
    ["sol::meta_function::bitwise_and", "&"],
    ["sol::meta_function::bitwise_or", "|"],
    ["sol::meta_function::bitwise_xor", "^"],
]);

const writePropertyDoc = (processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput) => {
    const { fqn } = coclass;

    // generate docs header
    processor.docs.push(`### ${ coclass.name }.${ name }\n`.replaceAll("_", "\\_"));

    const cppsignature = [];

    if (modifiers.includes("/S") && !coclass.isStatic()) {
        cppsignature.push("static");
    }

    if (modifiers.includes("/C")) {
        cppsignature.push("const");
    }

    cppsignature.push(cpptype);

    if (propname !== "this") {
        cppsignature.push(`${ fqn }::${ propname }`);
    }

    const attributes = [];

    if (has_propget) {
        attributes.push("propget");
    }

    if (has_propput) {
        attributes.push("propput");
    }

    const isStatic = modifiers.includes("/S") || coclass.isStatic();

    processor.docs.push([
        "```cpp",
        cppsignature.join(" "),
        // "",
        "lua:",
        `${ " ".repeat(4) }[${ attributes.join(", ") }] ${ isStatic ? "" : "o" }${ coclass.name }.${ name }`,
        "```",
        ""
    ].join("\n").replace(/\s*\( {2}\)/g, "()"));
};

const getProgId = (id, { progids }) => {
    return progids && progids.has(id) ? progids.get(id) : id;
};

const getPropname = (propname, modifiers) => {
    for (const modifier of modifiers.slice().sort((a, b) => {
        if (a.startsWith("/idlname=")) {
            return 1;
        }

        if (b.startsWith("/idlname=")) {
            return -1;
        }

        if (a[0] === "=") {
            return -1;
        }

        if (b[0] === "=") {
            return 1;
        }

        return 0;
    })) {
        if (modifier[0] === "=") {
            propname = modifier.slice(1);
        } else if (modifier.startsWith("/idlname=")) {
            propname = modifier.slice("/idlname=".length);
        }
    }
    return propname;
};

class LuaGenerator {
    static proto = proto;

    static getRegisterFn(coclass) {
        return `register_${ coclass.getClassName() }`;
    }

    static getMetaMethod(fname) {
        return meta_functions.has(fname) ? meta_functions.get(fname) : fname;
    }

    static getMetaName(fname) {
        return meta_names.has(fname) ? meta_names.get(fname) : fname;
    }

    static getLuaFn(fname) {
        fname = this.getMetaName(fname);
        return meta_operators.has(fname) ? meta_operators.get(fname) : fname;
    }

    static getBinaryOperator(fname) {
        return meta_binaries_op.get(fname);
    }

    // eslint-disable-next-line complexity
    static writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const dynamic_get = [];
        const dynamic_set = [];
        const registeri = contentRegister.length;

        for (const [fname, overloads] of coclass.methods.entries()) {
            const ename = LuaGenerator.getLuaFn(fname);
            const luaFn = `Lua_${ ename.replaceAll("::", "_") }`;

            let rexpr = false;
            let wexpr = false;
            let name = null;

            for (const decl of overloads) {
                const [, , func_modifiers] = decl;

                if (func_modifiers.includes("/attr=propget")) {
                    name = getPropname(fname, func_modifiers);
                    rexpr = true;
                    break;
                } else if (func_modifiers.includes("/attr=propput")) {
                    name = getPropname(fname, func_modifiers);
                    wexpr = false;
                    break;
                }
            }

            const key_name = JSON.stringify(name);

            if (rexpr || wexpr) {
                if (dynamic_get.length === 0 && dynamic_set.length === 0) {
                    contentRegisterPrivate.push("");
                }
                contentRegisterPrivate.push(`int ${ luaFn }(lua_State* L);`);
            }

            if (rexpr) {
                dynamic_get.push(`{ ${ key_name }, ${ luaFn } }`);
            }

            if (wexpr) {
                dynamic_set.push(`{ ${ key_name }, ${ luaFn } }`);
            }
        }

        for (const [name, property] of coclass.properties.entries()) {
            const {type, modifiers} = property;
            const cpptype = processor.getCppType(type, coclass, options);
            const isStatic = modifiers.includes("/S") || coclass.isStatic();
            const obj = `${ isStatic ? `${ fqn }::` : "self->" }`;
            const is_enum = modifiers.includes("/Enum");

            let propname = name;
            let getter;
            let has_propget = isStatic || is_enum || modifiers.includes("/R") || modifiers.includes("/RW");
            let has_propput = modifiers.includes("/W") || modifiers.includes("/RW");

            for (const modifier of modifiers) {
                if (modifier[0] === "=") {
                    propname = modifier.slice(1);
                } else if (modifier.startsWith("/idlname=")) {
                    propname = modifier.slice("/idlname=".length);
                } else if (modifier.startsWith("/R=")) {
                    getter = modifier.slice("/R=".length);
                    has_propget = true;
                } else if (modifier.startsWith("/RExpr=")) {
                    has_propget = true;
                } else if (modifier.startsWith("/W=")) {
                    has_propput = true;
                } else if (modifier.startsWith("/WExpr=")) {
                    has_propput = true;
                }
            }

            if (!has_propput) {
                has_propget = true;
            }

            let rexpr = null;
            let wexpr = null;

            if (has_propget) {
                rexpr = `${ obj }${ getter ? `${ getter }()` : propname }`;

                if (modifiers.includes("/Ref")) {
                    rexpr = `&(${ rexpr })`;
                }

                for (const modifier of modifiers) {
                    if (modifier.startsWith("/RExpr=")) {
                        rexpr = makeExpansion(modifier.slice("/RExpr=".length), rexpr);
                    }
                }

                for (const modifier of modifiers) {
                    if (modifier.startsWith("/Cast=")) {
                        rexpr = `${ modifier.slice("/Cast=".length) }(${ rexpr })`;
                    }
                }

                if (is_enum || isStatic && modifiers.includes("/C")) {
                    if (registeri === contentRegister.length) {
                        contentRegister.push(""); // new line
                    }
                    contentRegister.push(`lua_pushliteral(L, "${ name }"); lua_push(L, ${ rexpr }); lua_rawset(L, -3);`);
                    writePropertyDoc(processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput);
                    continue;
                }

                if (propname === "this") {
                    if (`${ fqn }::${ name }` === cpptype) {
                        continue;
                    }

                    if (registeri === contentRegister.length) {
                        contentRegister.push(""); // new line
                    }

                    const typedef = getTypeDef(cpptype, options);
                    const path = [];

                    if (processor.classes.has(typedef)) {
                        path.push(...getProgId(processor.classes.get(typedef).path.join("."), options).split("."));
                    } else {
                        path.push(...cpptype.split("::"));
                    }

                    while (path.length !== 0 && path[0] === "") {
                        path.shift();
                    }

                    const id = getProgId(path.join("."), options);

                    let thisIndex, moduleIndex;

                    if (coclass.isStatic()) {
                        if (coclass.progid) {
                            moduleIndex = -2;
                            thisIndex = -1;
                        } else {
                            moduleIndex = -2;
                            thisIndex = moduleIndex;
                        }
                    } else if (coclass.progid && !coclass.progid.includes(".")) {
                        moduleIndex = -3;
                        thisIndex = -1;
                    } else {
                        moduleIndex = -2;
                        thisIndex = -1;
                    }

                    let cpptypeIndex = moduleIndex;
                    if (id) {
                        if (moduleIndex !== -1) {
                            contentRegister.push(`lua_pushvalue(L, ${ moduleIndex }); // push the module`); thisIndex--; moduleIndex--;
                        }

                        contentRegister.push(`lua_rawget_create_if_nil(L, { ${ id.split(".").map(part => JSON.stringify(part)).join(", ") } }); // push ${ cpptype }`);
                        thisIndex--;
                        moduleIndex--;
                        cpptypeIndex = -1;
                    }

                    contentRegister.push(`lua_pushliteral(L, "${ name }");`); thisIndex--; moduleIndex--; cpptypeIndex--;
                    contentRegister.push(`lua_pushvalue(L, ${ cpptypeIndex }); // push ${ cpptype }`); thisIndex--; moduleIndex--; cpptypeIndex--;
                    contentRegister.push(`lua_rawset(L, ${ thisIndex }); // set ${ fqn }::${ name }`); thisIndex += 2; moduleIndex += 2; cpptypeIndex += 2;

                    if (id) {
                        contentRegister.push(`lua_pop(L, 1); // pop ${ cpptype }`); thisIndex++; moduleIndex++; cpptypeIndex++;

                        if (moduleIndex !== -1) {
                            contentRegister.push("lua_pop(L, 1); // pop the module"); thisIndex++; moduleIndex++; cpptypeIndex++;
                        }
                    }

                    contentRegister.push(""); // new line

                    writePropertyDoc(processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput);
                    continue;
                }

                rexpr = `lua_push(L, ${ rexpr });`;
            }

            if (has_propput) {
                const overloads = [];

                let c_setter;
                let c_type = cpptype;

                for (const modifier of modifiers) {
                    if (modifier.startsWith("/W=")) {
                        c_setter = modifier.slice("/W=".length);
                    } else if (modifier.startsWith("/WType=")) {
                        c_type = modifier.slice("/WType=".length);
                    } else if (modifier.startsWith("/WExpr=")) {
                        overloads.push([c_setter, c_type, modifier.slice("/WExpr=".length)]);
                    }
                }

                if (overloads.length === 0) {
                    overloads.push([c_setter, c_type]);
                }

                wexpr = [];
                const cpptypes = [];

                for (const [o_setter, o_type, o_expr] of overloads) {
                    const wtype = removeConstQualifiers(processor.getCppType(o_type, coclass, options));

                    cpptypes.push(wtype);

                    let in_val = "value";

                    if (o_expr) {
                        in_val = makeExpansion(o_expr, in_val);
                        in_val = in_val.replace(/\$(?:value\b|\{[^\S\n]*value[^\S\n]*\})/g, "value");
                    } else if (o_setter) {
                        in_val = `${ obj }${ o_setter }(${ in_val })`;
                    } else {
                        const lvalue = `${ obj }${ propname }`;
                        let rvalue = in_val;

                        if (o_type !== cpptype) {
                            rvalue = `static_cast<${ cpptype }>(${ rvalue })`;
                        }

                        in_val = `${ lvalue } = ${ rvalue }`;
                    }

                    wexpr.push(`
                    {
                        auto value_holder = lua_to(L, 3, static_cast<${ wtype }*>(nullptr), is_valid);
                        if (is_valid) {
                            decltype(auto) value = extract_holder(value_holder, static_cast<${ wtype }*>(nullptr));
                            ${ in_val.split("\n").join(`\n${ " ".repeat(28) }`) };
                            return 0;
                        }
                    }
                    `.replace(/^ {20}/mg, "").trim());
                }

                if (wexpr.length === 1) {
                    wexpr[0] = wexpr[0].slice(1, -1).trim().replace(/^ {4}/mg, "");
                }

                wexpr.push(`return luaL_typeerror(L, 3, ${ JSON.stringify(cpptypes.join("|")) });`);

                wexpr = wexpr.join("\n\n");
            }

            const key_name = JSON.stringify(name);
            const obj_decl = [];

            if (!isStatic) {
                obj_decl.push(`
                    bool is_valid;
                    auto self = lua_to(L, 1, static_cast<${ fqn }*>(nullptr), is_valid);
                    if (!is_valid) {
                        return luaL_typeerror(L, 1, ${ JSON.stringify(fqn) });
                    }
                `.replace(/^ {20}/mg, "").trim());
                obj_decl.push("");
            }

            if (rexpr) {
                if (rexpr.includes("is_valid") && !obj_decl[0].includes("bool is_valid;")) {
                    obj_decl.unshift("bool is_valid;");
                }

                dynamic_get.push(`
                    { ${ key_name }, [](lua_State* L) {
                        ${ obj_decl.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                        ${ rexpr.split("\n").join(`\n${ " ".repeat(24) }`) }
                        return 1;
                    }}
                `.replace(/^ {20}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));
            }

            if (wexpr) {
                if (wexpr.includes("is_valid") && !obj_decl[0].includes("bool is_valid;")) {
                    obj_decl.unshift("bool is_valid;");
                }

                dynamic_set.push(`
                    { ${ key_name }, [](lua_State* L) {
                        ${ obj_decl.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                        ${ wexpr.split("\n").join(`\n${ " ".repeat(24) }`) }
                    }}
                `.replace(/^ {20}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));
            }

            writePropertyDoc(processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput);
        }

        if (!coclass.isStatic()) {
            contentRegisterPrivate.push("", `
                std::map<std::string, std::function<int(lua_State*)>> getters({
                    ${ dynamic_get.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });
            `.replace(/^ {16}/mg, "").trim().replace(/\{\s+\}/mg, "{}"));
        } else if (dynamic_get.length !== 0) {
            contentRegisterPrivate.push("", `
                const std::map<std::string, std::function<int(lua_State*)>> getters({
                    ${ dynamic_get.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                int dynamic_get(lua_State* L) {
                    bool is_valid;
                    const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr), is_valid);
                    if (is_valid && getters.count(k)) {
                        return getters.at(k)(L);
                    }
                    return lua_missing_declaration(L);
                }
            `.replace(/^ {16}/mg, "").trim());
        }

        if (!coclass.isStatic()) {
            contentRegisterPrivate.push("", `
                std::map<std::string, std::function<int(lua_State*)>> setters({
                    ${ dynamic_set.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });
            `.replace(/^ {16}/mg, "").trim().replace(/\{\s+\}/mg, "{}"));
        } else if (dynamic_set.length !== 0) {
            contentRegisterPrivate.push("", `
                const std::map<std::string, std::function<int(lua_State*)>> setters({
                    ${ dynamic_set.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                void dynamic_set(lua_State* L) {
                    bool is_valid;
                    const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr), is_valid);
                    if (is_valid && setters.count(k)) {
                        return setters.at(k)(L);
                    }

                    // set the value on the module
                    lua_pushvalue(L, 2); // push the key
                    lua_pushvalue(L, 3); // push the value
                    lua_rawset(L, 1);
                    return 0;
                }
            `.replace(/^ {16}/mg, "").trim());
        }

        if (coclass.isStatic()) {
            const index_methods = [];

            if (dynamic_get.length !== 0) {
                index_methods.push("{\"__index\", dynamic_get}, // when we access an absent field in an instance");
            } else {
                index_methods.push("{\"__index\", lua_missing_declaration}, // when we access an absent field in an instance");
            }

            if (dynamic_set.length !== 0) {
                index_methods.push("{\"__newindex\", dynamic_set}, // when we assign a value to an absent field in an instance");
            }

            if (index_methods.length !== 0) {
                contentRegisterPrivate.push("", `
                    const struct luaL_Reg index_methods[] = {
                        ${ index_methods.join(`\n${ " ".repeat(24) }`) }
                        {NULL, NULL} // Sentinel
                    };
                `.replace(/^ {20}/mg, "").trim());

                const path = getProgId(coclass.path.join("."), options).split(".");
                while (path.length !== 0 && path[0] === "") {
                    path.shift();
                }

                const body = ["lua_pushfuncs(L, index_methods);"];

                if (path.length !== 0) {
                    body.unshift("lua_getmetatable(L, -1);");
                    body.push("lua_pop(L, 1);");
                }

                contentRegister.push("", body.join("\n"));
            }
        }
    }

    static isConstructor(func_modifiers) {
        return func_modifiers.includes("/CO") && !func_modifiers.some(modifier => modifier[0] === "=");
    }

    static Errors = {
        argc: (fname, overload_id, overloads, not_found, variadic, argc, offset, largc) => {
            if ( overloads.length !== 1 || !not_found.includes("Overload resolution failed")) {
                return `goto overload${ overload_id };`;
            }

            if (variadic) {
                return `LUAL_MODULE_ERROR_RETURN(L, "bad number of arguments to '${ fname }' (expecting at least ${ argc + offset - 1 }, given " << (${ largc }) << ")");`;
            }

            return `LUAL_MODULE_ERROR_RETURN(L, "bad number of arguments to '${ fname }' (expecting at most ${ argc + offset }, given " << (${ largc } + kwargc) << ")");`;
        },

        kwarg: (fname, overload_id, overloads, not_found, pos, argname) => {
            if ( overloads.length !== 1 || !not_found.includes("Overload resolution failed")) {
                return `goto overload${ overload_id };`;
            }

            return `LUAL_MODULE_ERROR_RETURN(L, "bad argument ${ pos } to '${ fname }' (already provided as named parameter '${ argname }'");`;
        },

        kwarg_type: (fname, overload_id, overloads, not_found, pos, type, vargc, argname) => {
            if ( overloads.length !== 1 || !not_found.includes("Overload resolution failed")) {
                return `goto overload${ overload_id };`;
            }

            return [
                `Keywords::push(L, ${ vargc }, "${ argname }");`,
                `LUAL_MODULE_ERROR(L, "bad argument '${ argname }' to '${ fname }' (cannot convert '" << ::LUA_MODULE_NAME::internal::LuaTypeName(L, -1) << "' to '${ type }')");`,
                "lua_pop(L, 1);",
                `return lua_gettop(L) - ${ vargc };`,
            ].join("\n");
        },

        type: (fname, overload_id, overloads, not_found, pos, type, arg) => {
            if ( overloads.length !== 1 || !not_found.includes("Overload resolution failed")) {
                return `goto overload${ overload_id };`;
            }

            return `LUAL_MODULE_ERROR_RETURN(L, "bad argument ${ pos } to '${ fname }' (cannot convert '" << ::LUA_MODULE_NAME::internal::LuaTypeName(L, ${ arg }) << "' to '${ type }')");`;
        },

        mandatory: (fname, overload_id, overloads, not_found, argname) => {
            if ( overloads.length !== 1 || !not_found.includes("Overload resolution failed")) {
                return `goto overload${ overload_id };`;
            }

            return `LUAL_MODULE_ERROR_RETURN(L, "missing argument ${ argname } to '${ fname }'");`;
        },

        unknown: (fname, overload_id, overloads, not_found) => {
            if ( overloads.length !== 1 || !not_found.includes("Overload resolution failed")) {
                return `goto overload${ overload_id };`;
            }

            return `LUAL_MODULE_ERROR_RETURN(L, "unknown named parameters to '${ fname }'");`;
        },
    };

    // eslint-disable-next-line complexity
    static writeMethods(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const { shared_ptr } = options;
        const indent = " ".repeat(4);
        const methods = [];
        const meta_methods = [];
        const cname = options.cname ? options.cname : "create";
        const hasDC = coclass.modifiers?.includes("/DC");
        const largc = options.largc || "__argc__";
        const vargc = options.vargc || "__vargc__";

        for (const fname of Array.from(coclass.methods.keys()).sort((a, b) => {
            if (a === cname) {
                return -1;
            }

            if (b === cname) {
                return 1;
            }

            return a > b ? 1 : a < b ? -1 : 0;
        })) {
            const dname = LuaGenerator.getMetaMethod(fname);
            const ename = LuaGenerator.getLuaFn(fname);
            const overloads = coclass.methods.get(fname);
            const contentFunction = [];
            const luaFn = `Lua_${ ename.replaceAll("::", "_") }`;
            const isProperty = contentRegisterPrivate.includes(`int ${ luaFn }(lua_State* L);`);
            const hasConstructor = overloads.some(([, , func_modifiers]) => LuaGenerator.isConstructor(func_modifiers));

            // generate docs header
            processor.docs.push(`### ${ fqn.replaceAll("::", ".") }.${ hasConstructor ? cname : fname }\n`.replaceAll("_", "\\_"));

            let isConstructor = false;
            let overload_id = 0;
            let argcMax = 0;
            const argnames = new Set();

            let not_found = "LUAL_MODULE_ERROR_RETURN(L, \"Overload resolution failed\")";
            if (!coclass.isStatic()) {
                if (ename === "__index" || ename === "sol::meta_function::index") {
                    not_found = `return lua_class__index<0, ::${ [fqn, ...coclass.parents].join(", ::") }>(L)`;
                } else if (ename === "__newindex" || ename === "sol::meta_function::new_index") {
                    not_found = `return lua_class__newindex<0, ::${ [fqn, ...coclass.parents].join(", ::") }>(L)`;
                }
            }

            for (const decl of overloads) {
                overload_id++;

                const [name, return_value_type, func_modifiers] = decl;
                const list_of_arguments = decl[3].slice();
                const variadic = list_of_arguments.length !== 0 && list_of_arguments.at(-1)[0] === "...";

                // http://lua-users.org/lists/lua-l/2010-01/msg00160.html
                // The documentation at https://www.lua.org/manual/5.1/manual.html#2.8 states
                // that the #-operator will eventually call the __len metamethod with h(op)
                // however, for userdata with a metatable and the __len metamethod is called with h(op, nil)
                if (fname === "sol::meta_function::length" || fname === "__len") {
                    const { AnyObject } = options;
                    func_modifiers.push("/Expr=");
                    list_of_arguments.push([AnyObject, "unused", `${ AnyObject }()`, ["/ignore"]]);
                }

                isConstructor = LuaGenerator.isConstructor(func_modifiers);
                const argc = list_of_arguments.length - (variadic ? 1 : 0);
                argcMax = Math.max(argcMax, argc);

                const in_args = new Array(argc).fill(false);
                const out_args = new Array(argc).fill(false);
                const out_array_args = new Array(argc).fill(false);

                const outlist = [];

                if (return_value_type !== "" && return_value_type !== "void") {
                    outlist.push("retval");
                } else if (isConstructor) {
                    outlist.push("self");
                }

                for (let j = 0; j < argc; j++) {
                    const [argtype, argname, , arg_modifiers] = list_of_arguments[j];
                    const is_in_array = /^Input(?:Output)?Array(?:OfArrays)?$/.test(argtype);
                    const is_out_array = /^(?:Input)?OutputArray(?:OfArrays)?$/.test(argtype);
                    const is_in_out = arg_modifiers.includes("/IO");

                    argnames.add(argname);

                    in_args[j] = is_in_array || is_in_out || arg_modifiers.includes("/I");
                    out_args[j] = is_out_array || is_in_out || arg_modifiers.includes("/O");
                    out_array_args[j] = is_out_array;

                    if (out_args[j]) {
                        outlist.push(argname);
                    }
                }

                // the python api expects parameters in this order:
                // mandatory, OutputArray or optional parameter, /O parameter
                const getArgWeight = j => {
                    if (!in_args[j]) {
                        // is OutputArray
                        if (out_array_args[j]) {
                            return 2;
                        }

                        // out arg which is not an output array
                        if (out_args[j]) {
                            return 3;
                        }
                    }

                    // has a default value
                    if (list_of_arguments[j][2] !== "") {
                        return 2;
                    }

                    // is non optional value
                    return 1;
                };

                const indexes = Array.from(new Array(argc).keys()).sort((a, b) => {
                    const diff = getArgWeight(a) - getArgWeight(b);
                    return diff === 0 ? a - b : diff;
                });

                const isStatic = func_modifiers.includes("/S") || coclass.isStatic();
                const offset = (isStatic ? 0 : 1) + (isProperty ? 1 : 0);
                const callargs = [];
                const retval = [];
                const overload = [];
                const extractors = [];
                const precondition = [];

                if (!isStatic) {
                    precondition.push("!self");
                }

                if (variadic) {
                    precondition.push("has_kwargs");
                    precondition.push(`${ largc } < ${ argc + offset - 1 }`);
                } else {
                    precondition.push(`${ largc } + kwargc > ${ argc + offset }`);
                }

                overload.push(`
                    if (${ precondition.join(" || ") }) {
                        // wrong number of paramters
                        ${ LuaGenerator.Errors.argc(dname, overload_id, overloads, not_found, variadic, argc, offset, largc) }
                    }

                    int usedkw = 0;
                `.replace(/^ {20}/mg, "").trim());

                let firstoptarg = argc;

                for (let i = 0, is_first_optional = true; i < argc; i++) {
                    const j = indexes[i];
                    const [, argname, , arg_modifiers] = list_of_arguments[j];
                    let [argtype, , defval] = list_of_arguments[j];

                    if (arg_modifiers.includes("/ignore")) {
                        continue;
                    }

                    const is_ptr = argtype.endsWith("*");
                    const is_in_arg = in_args[j];
                    const is_out_arg = out_args[j];
                    let is_optional = defval !== "" || is_out_arg && !is_in_arg;

                    let is_array = false;
                    let arrtype = "";
                    let arg_suffix = "";

                    if (argtype === "InputArray") {
                        is_array = true;
                        arg_suffix = "array";
                        arrtype = "InputArray";
                        argtype = "Mat";
                    } else if (argtype === "InputOutputArray") {
                        is_array = true;
                        arg_suffix = "array";
                        arrtype = "InputOutputArray";
                        argtype = "Mat";
                    } else if (argtype === "OutputArray") {
                        is_array = true;
                        arg_suffix = "array";
                        arrtype = "OutputArray";
                        argtype = "Mat";
                    } else if (argtype === "InputArrayOfArrays") {
                        is_array = true;
                        arg_suffix = "arrays";
                        arrtype = "InputArray";
                        argtype = "std::vector<cv::Mat>";
                    } else if (argtype === "InputOutputArrayOfArrays") {
                        is_array = true;
                        arg_suffix = "arrays";
                        arrtype = "InputOutputArray";
                        argtype = "std::vector<cv::Mat>";
                    } else if (argtype === "OutputArrayOfArrays") {
                        is_array = true;
                        arg_suffix = "arrays";
                        arrtype = "OutputArray";
                        argtype = "std::vector<cv::Mat>";
                    }

                    if (is_array) {
                        defval = defval
                            .replace("InputArrayOfArrays", "std::vector<cv::Mat>")
                            .replace("InputOutputArrayOfArrays", "std::vector<cv::Mat>")
                            .replace("OutputArrayOfArrays", "std::vector<cv::Mat>")
                            .replace("InputArray", "Mat")
                            .replace("InputOutputArray", "Mat")
                            .replace("OutputArray", "Mat")
                            .replace("noArray", argtype);
                    } else {
                        defval = processor.fqnIndentifier(defval, coclass, options);
                    }

                    let callarg = argname;
                    let cpptype = processor.getCppType(argtype, coclass, options);

                    if ((is_in_arg || is_out_arg) && is_ptr && !PTR.has(argtype)) {
                        callarg = `&${ callarg }`;
                        argtype = argtype.slice(0, -1).trim();
                        defval = SIMPLE_ARGTYPE_DEFAULTS.has(argtype) ? SIMPLE_ARGTYPE_DEFAULTS.get(argtype) : "";
                    } else if (is_out_arg && cpptype.startsWith(`${ shared_ptr }<`)) {
                        callarg = `reference_internal(${ callarg }, static_cast<${ cpptype }*>(nullptr))`;
                        argtype = cpptype.slice(`${ shared_ptr }<`.length, -">".length);
                        defval = SIMPLE_ARGTYPE_DEFAULTS.has(argtype) ? SIMPLE_ARGTYPE_DEFAULTS.get(argtype) : "";
                    } else if (defval === "" && SIMPLE_ARGTYPE_DEFAULTS.has(argtype)) {
                        defval = SIMPLE_ARGTYPE_DEFAULTS.get(argtype);
                    } else if (defval.endsWith("()") && processor.getCppType(defval.slice(0, -"()".length), coclass, options) === cpptype) {
                        defval = "";
                    }

                    cpptype = removeConstQualifiers(processor.getCppType(argtype, coclass, options));

                    for (const modifier of arg_modifiers) {
                        if (modifier.startsWith("/Cast=")) {
                            callarg = `${ modifier.slice("/Cast=".length) }(${ callarg })`;
                        } else if (modifier.startsWith("/Expr=")) {
                            callarg = makeExpansion(modifier.slice("/Expr=".length), callarg);
                        }
                    }

                    if (arg_modifiers.includes("/RRef")) {
                        callarg = `std::move(${ callarg })`;
                    }

                    callargs[j] = callarg;

                    const arr_cpptype = processor.getCppType(arrtype, coclass, options);
                    const is_shared_ptr = cpptype.startsWith(`${ shared_ptr }<`);
                    const is_by_ref = !is_ptr && !is_shared_ptr && processor.classes.has(cpptype) && !processor.enums.has(cpptype);
                    const var_type = is_array ? arr_cpptype : cpptype;
                    const argi = i + offset;
                    const argn = `${ argi + 1 } + __top__`;
                    const nd_mat = arg_modifiers.includes("/ND");
                    const defarg = `default_${ argname }_value`;

                    if (is_out_arg && is_array) {
                        is_optional = true;
                    }

                    if (is_out_arg) {
                        if (is_array) {
                            retval.push([j, `lua_push(L, ${ argname }_${ arrtype });`]);
                        } else {
                            const lua_push_args = ["L", argname];

                            let deleter;

                            for (const modifier of arg_modifiers) {
                                if (modifier.startsWith("/Deleter=")) {
                                    deleter = modifier.slice("/Deleter=".length);
                                } else if (modifier.startsWith("/OutCast=")) {
                                    const cast = modifier.slice("/OutCast=".length);
                                    lua_push_args[1] = `${ cast }(${ lua_push_args[1] })`;
                                }
                            }

                            if (deleter) {
                                lua_push_args.push(deleter);
                            }

                            retval.push([j, `lua_push(${ lua_push_args.join(", ") });`]);
                        }
                    }

                    extractors.push("", `
                        // =========================
                        // extract argument ${ argname }
                        // =========================
                    `.replace(/^ {24}/mg, "").trim());

                    if (is_optional) {
                        extractors.push("is_valid = false;");
                        const ref = defval !== "" && is_by_ref && !defval.includes("(") && !defval.includes("'") && !defval.includes("\"") && !/^(?:\.|\d+\.)\d+$/.test(defval) ? "&" : "";
                        const copy = is_out_arg || defval === "" || defval.includes("(");
                        if (!is_array || defval !== "") {
                            extractors.push(`${ copy || ref ? "" : "static " }${ cpptype }${ ref } ${ defarg }${ defval !== "" && defval !== "{}" ? ` { ${ defval } }` : "" };`);
                        }
                    }

                    if (is_array) {
                        extractors.push(`
                            Optional${ arg_suffix[0].toUpperCase() + arg_suffix.slice(1) }<${ var_type }> ${ argname }_${ arrtype };

                            if (${ largc } > ${ argi }) {
                                // positional parameter
                                ${ argname }_${ arrtype } = lua_to${ arg_suffix }(L, ${ argn }, static_cast<${ var_type }*>(nullptr), is_valid${ arg_suffix === "arrays" && nd_mat ? ", true" : "" });
                                if (!is_valid) {
                                    ${ LuaGenerator.Errors.type(dname, overload_id, overloads, not_found, `#${ argn }`, var_type, argn) }
                                }

                                // should not be a named parameter
                                if (has_kwargs && Keywords::has(L, ${ vargc }, "${ argname }")) {
                                    ${ LuaGenerator.Errors.kwarg(dname, overload_id, overloads, not_found, `#${ argn }`, argname) }
                                }
                            }
                            else if (has_kwargs && Keywords::has(L, ${ vargc }, "${ argname }")) {
                                // named parameter
                                Keywords::push(L, ${ vargc }, "${ argname }");
                                ${ argname }_${ arrtype } = lua_to${ arg_suffix }(L, -1, static_cast<${ var_type }*>(nullptr), is_valid${ arg_suffix === "arrays" && nd_mat ? ", true" : "" });
                                lua_pop(L, 1);
                                if (!is_valid) {
                                    ${ LuaGenerator.Errors.kwarg_type(dname, overload_id, overloads, not_found, `kwargs.${ argname }`, var_type, vargc, argname).split("\n").join(`\n${ " ".repeat(36) }`) }
                                }
                                usedkw++;
                            }
                        `.replace(/^ {28}/mg, "").trim());

                        if (!is_optional) {
                            extractors.push(`
                                else {
                                    // mandatory parameter
                                    ${ LuaGenerator.Errors.mandatory(dname, overload_id, overloads, not_found, argname) }
                                }
                            `.replace(/^ {32}/mg, "").trim());
                        } else if (defval !== "") {
                            extractors.push(`
                                else {
                                    ${ argname }_${ arrtype } = ${ defarg };
                                }
                            `.replace(/^ {32}/mg, "").trim());
                        }

                        extractors.push("", `decltype(auto) ${ argname } = *${ argname }_${ arrtype };`);
                    } else if (is_shared_ptr) {
                        extractors.push(`
                            ${ cpptype } ${ argname };

                            if (${ largc } > ${ argi }) {
                                // positional parameter
                                ${ argname } = lua_to(L, ${ argn }, static_cast<${ var_type }*>(nullptr), is_valid);
                                if (!is_valid) {
                                    ${ LuaGenerator.Errors.type(dname, overload_id, overloads, not_found, `#${ argn }`, var_type, argn) }
                                }

                                // should not be a named parameter
                                if (has_kwargs && Keywords::has(L, ${ vargc }, "${ argname }")) {
                                    ${ LuaGenerator.Errors.kwarg(dname, overload_id, overloads, not_found, `#${ argn }`, argname) }
                                }
                            }
                            else if (has_kwargs && Keywords::has(L, ${ vargc }, "${ argname }")) {
                                // named parameter
                                Keywords::push(L, ${ vargc }, "${ argname }");
                                ${ argname } = lua_to(L, -1, static_cast<${ var_type }*>(nullptr), is_valid);
                                lua_pop(L, 1);
                                if (!is_valid) {
                                    ${ LuaGenerator.Errors.kwarg_type(dname, overload_id, overloads, not_found, `kwargs.${ argname }`, var_type, vargc, argname).split("\n").join(`\n${ " ".repeat(36) }`) }
                                }
                                usedkw++;
                            }
                        `.replace(/^ {28}/mg, "").trim());

                        if (!is_optional) {
                            extractors.push(`
                                else {
                                    // mandatory parameter
                                    ${ LuaGenerator.Errors.mandatory(dname, overload_id, overloads, not_found, argname) }
                                }
                            `.replace(/^ {32}/mg, "").trim());
                        } else {
                            extractors.push(`
                                else {
                                    ${ argname } = ${ defarg };
                                }
                            `.replace(/^ {32}/mg, "").trim());
                        }
                    } else {
                        extractors.push(`
                            using ${ toCamelCase(argname) }Holder = decltype(lua_to(L, ${ argn }, static_cast<${ var_type }*>(nullptr), is_valid));
                            ${ toCamelCase(argname) }Holder ${ argname }_holder;

                            if (${ largc } > ${ argi }) {
                                // positional parameter
                                ${ argname }_holder = lua_to(L, ${ argn }, static_cast<${ var_type }*>(nullptr), is_valid);
                                if (!is_valid) {
                                    ${ LuaGenerator.Errors.type(dname, overload_id, overloads, not_found, `#${ argn }`, var_type, argn) }
                                }

                                // should not be a named parameter
                                if (has_kwargs && Keywords::has(L, ${ vargc }, "${ argname }")) {
                                    ${ LuaGenerator.Errors.kwarg(dname, overload_id, overloads, not_found, `#${ argn }`, argname) }
                                }
                            }
                        `.replace(/^ {28}/mg, "").trim());

                        if (!variadic) {
                            extractors.push(`
                                else if (has_kwargs && Keywords::has(L, ${ vargc }, "${ argname }")) {
                                    // named parameter
                                    Keywords::push(L, ${ vargc }, "${ argname }");
                                    ${ argname }_holder = lua_to(L, -1, static_cast<${ var_type }*>(nullptr), is_valid);
                                    lua_pop(L, 1);
                                    if (!is_valid) {
                                        ${ LuaGenerator.Errors.kwarg_type(dname, overload_id, overloads, not_found,
                                            `kwargs.${ argname }`, var_type, vargc, argname).split("\n").join(`\n${ " ".repeat(40) }`) }
                                    }
                                    usedkw++;
                                }
                            `.replace(/^ {32}/mg, "").trim());
                        }

                        if (!is_optional) {
                            extractors.push(`
                                else {
                                    // mandatory parameter
                                    ${ LuaGenerator.Errors.mandatory(dname, overload_id, overloads, not_found, argname) }
                                }
                            `.replace(/^ {32}/mg, "").trim());
                        }

                        extractors.push("", `
                            decltype(auto) ${ argname } = ${ is_optional ? `!is_valid ? ${ defarg } : ` : "" }extract_holder(${ argname }_holder, static_cast<${ var_type }*>(nullptr));
                        `.replace(/^ {28}/mg, "").trim());
                    }

                    if (is_optional && is_first_optional) {
                        firstoptarg = Math.min(firstoptarg, i);
                        is_first_optional = false;
                    }
                }

                overload.push(...extractors, "", `
                    // =========================
                    // call ${ name.replaceAll(".", "::") }
                    // =========================
                `.replace(/^ {20}/mg, "").trim());

                if (!variadic) {
                    overload.push(`
                        // unknown named parameters
                        if (usedkw != kwargc) {
                            ${ LuaGenerator.Errors.unknown(dname, overload_id, overloads, not_found) }
                        }
                    `.replace(/^ {24}/mg, "").trim());
                }

                let callee;
                const path = name.split(isConstructor ? "::" : ".");
                let is_operator = !variadic && /^operator\s*(?:[+\-*/%^&|!=<>]=?|[~,]|(?:<<|>>)=?|&&|\|\||\+\+|--|->\*?)$/.test(path[path.length - 1]);
                const operator = is_operator ? path[path.length - 1].slice("operator".length).trim() : null;

                if (isStatic) {
                    callee = path.join("::");
                } else {
                    callee = "self";

                    for (const modifier of func_modifiers) {
                        if (modifier.startsWith("/Cast=")) {
                            callee = `${ modifier.slice("/Cast=".length) }(${ callee })`;
                        } else if (modifier.startsWith("/Prop=")) {
                            callee = `${ callee }->${ modifier.slice("/Prop=".length) }`;
                        }
                    }

                    if (callargs.length === 0 && is_operator) {
                        callee = `${ operator }(*${ callee })`;
                    } else if (callargs.length === 1 && is_operator) {
                        callee = `(*${ callee }) ${ operator } `;
                    } else {
                        callee = `${ callee }->${ path[path.length - 1] }`;
                        is_operator = false;
                    }
                }

                let expr = callargs.join(", ");
                let has_expr = false;
                let has_call = false;

                for (const modifier of func_modifiers) {
                    if (modifier.startsWith("/Expr=")) {
                        expr = makeExpansion(modifier.slice("/Expr=".length), ...callargs);
                        has_expr = true;
                    } else if (modifier.startsWith("/Call=")) {
                        callee = makeExpansion(modifier.slice("/Call=".length), callee);
                        has_call = true;
                    }
                }

                if (variadic) {
                    callee = options.variadic(return_value_type, callee, expr, offset, callargs);
                } else if (isStatic && is_operator && callargs.length === 2 && !has_expr && !has_call) {
                    callee = `(${ callargs.join(`) ${ operator } (`) })`;
                } else if (!is_operator || expr) {
                    callee = `${ callee }(${ expr })`;
                }

                if (isConstructor && !has_call) {
                    const pos = callee.indexOf("(");
                    callee = `std::make_shared<${ callee.slice(0, pos) }>${ callee.slice(pos) }`;
                } else if (func_modifiers.includes("/Ref")) {
                    callee = `reference_internal(${ callee })`;
                }

                for (const modifier of func_modifiers) {
                    if (modifier.startsWith("/WrapAs=")) {
                        callee = `${ modifier.slice("/WrapAs=".length) }(${ callee })`;
                    } else if (modifier.startsWith("/Output=")) {
                        callee = makeExpansion(modifier.slice("/Output=".length), callee);
                    }
                }

                let has_body = false;
                for (const modifier of func_modifiers) {
                    if (modifier.startsWith("/Body=")) {
                        callee = makeExpansion(modifier.slice("/Body=".length), callee);
                        has_body = true;
                    }
                }

                const exception = typeof options.exception === "string" ? options.exception : "std::exception";

                if (has_body || return_value_type === "void") {
                    overload.push("", `
                        try {
                            ${ callee.trim().split("\n").join(`\n${ " ".repeat(28) }`) };
                        } catch ( ${ exception }& e ) {
                            LUAL_MODULE_ERROR_RETURN(L, e.what());
                        }
                    `.replace(/^ {24}/mg, "").trim());

                    // body is responsible of pushing returned values
                    if (has_body) {
                        retval.length = 0;
                    }
                } else {
                    const lua_push_args = ["L", callee.trim()];

                    let deleter;
                    let default_constructor;
                    for (const modifier of func_modifiers) {
                        if (modifier.startsWith("/Deleter=")) {
                            deleter = modifier.slice("/Deleter=".length);
                        } else if (modifier.startsWith("/DC=")) {
                            default_constructor = modifier.slice("/DC=".length);
                        }
                    }

                    if (deleter) {
                        lua_push_args.push(deleter);
                    }

                    const call = [];

                    if (isConstructor && default_constructor) {
                        callee = `auto self = ${ lua_push_args[1] };`;
                        call.push(...callee.split("\n"), ...default_constructor.trim().split("\n"));
                        lua_push_args[1] = "self";
                    }

                    call.push(`lua_push(${ lua_push_args.join(", ") });`);

                    retval.push([-1, `
                        try {
                            ${ call.join("\n").split("\n").join(`\n${ " ".repeat(28) }`) }
                        } catch ( ${ exception }& e ) {
                            LUAL_MODULE_ERROR_RETURN(L, e.what());
                        }
                    `.replace(/^ {24}/mg, "").trim()]);
                }

                if (retval.length !== 0) {
                    retval.sort(([a], [b]) => a - b);
                    overload.push("", retval.map(([, result]) => result).join("\n"));
                }

                if (!isConstructor || !hasDC) {
                    overload.push("", `return lua_gettop(L) - ${ vargc };`);
                }

                contentFunction.push("");

                contentFunction.push("{");

                let constraints;
                for (const modifier of func_modifiers) {
                    if (modifier.startsWith("/Requires=")) {
                        constraints = modifier.slice("/Requires=".length);
                    }
                }

                if (constraints) {
                    contentFunction[contentFunction.length - 1] = `if constexpr (requires${ constraints }) {`;
                }

                contentFunction.push(indent + overload.join("\n").split("\n").join(`\n${ indent }`));

                contentFunction.push("}");

                contentFunction.push(`overload${ overload_id }:`);

                if (variadic) {
                    indexes.push(argc);
                }

                LuaGenerator.writeMethodDocs(
                    processor,
                    coclass,
                    fname,
                    decl,
                    outlist,
                    indexes,
                    firstoptarg,
                    isConstructor,
                    options
                );
            }

            let start = 0;
            while (contentFunction[start] === "" && start + 1 < contentFunction.length) {
                start++;
            }

            const body = `
                auto ${ vargc } = lua_gettop(L);
                auto has_kwargs = ${ vargc } != 0 && usertype_info<Keywords>::lua_userdata_is(L, ${ vargc });
                auto kwargc = has_kwargs ? Keywords::size(L, ${ vargc }) : 0;
                const int ${ largc } = (has_kwargs ? ${ vargc } - 1 : ${ vargc }) - __top__;

                ${ contentFunction.slice(start).join("\n").split("\n").join(`\n${ " ".repeat(16) }`) }
            `.replace(/^ {12}/mg, "").replace(/(?:^\n|\s+$)/g, "");

            // Reset function content
            contentFunction.length = 0;

            contentFunction.push(`int ${ luaFn }(lua_State* L) {`);

            if ((isConstructor && hasDC) || /\b(?:is_valid|self)\b/.test(body)) {
                contentFunction.push("    bool is_valid;", "");
            }

            if (!isConstructor && /\bself\b/.test(body)) {
                contentFunction.push(`    auto self = lua_to(L, 1 + __top__, static_cast<${ fqn }*>(nullptr), is_valid);`, "");
            }

            if (isConstructor && hasDC) {
                contentRegisterPrivate.push(`${ shared_ptr }<${ fqn }> Lua_new(lua_State* L, const size_t __top__, bool& is_valid) {`);
                contentRegisterPrivate.push("    is_valid = true;", "");
                contentRegisterPrivate.push(body
                    .replace(/lua_push\(L, (.+)\);$/mg, "return $1;")
                    .replace(/(?<indent>[^\S\n]+)LUAL_MODULE_ERROR_RETURN\((?<error>.+)\);$/mg, `$<indent>${ [
                        "is_valid = false;",
                        "LUAL_MODULE_ERROR($<error>);",
                        `return ${ shared_ptr }<${ fqn }>();`,
                    ].join("\n$<indent>") }`)
                );
                contentRegisterPrivate.push("    is_valid = false;");
                contentRegisterPrivate.push(`    return ${ shared_ptr }<${ fqn }>();`);
                contentRegisterPrivate.push("}", "");

                contentFunction.push(`
                    auto ${ vargc } = lua_gettop(L);
                    auto ptr = Lua_new(L, 0, is_valid);
                    if (!is_valid) {
                        ${ not_found };
                    }

                    lua_push(L, ptr);
                    return lua_gettop(L) - ${ vargc };
                `.replace(/^ {16}/mg, "").replace(/(?:^\n|\s+$)/g, ""));
            } else {
                contentFunction.push(body);
                contentFunction.push(`    ${ not_found };`);
            }

            contentFunction.push("}");

            let expression = contentFunction.join("\n");

            if (!isConstructor || !hasDC) {
                expression = expression.replaceAll(" + __top__", "").replaceAll(" - __top__", "");
            }

            // new line
            contentRegisterPrivate.push(expression, "");

            if (ename.startsWith("sol::meta_function::")) {
                const meta_method = LuaGenerator.getMetaMethod(ename);
                methods.push([meta_method, luaFn]);
            } else if (!isProperty) {
                methods.push([ename, luaFn]);
            }

            if (isConstructor || fname === cname) {
                meta_methods.push(["__call", "__call_constructor"]);
            }
        }

        contentRegisterPrivate.push("", `
            const struct luaL_Reg methods[] = {
                ${ methods.map(([ename, fname]) => `{"${ ename }", ${ fname }}`).concat(["{NULL, NULL} // Sentinel"]).join(`,\n${ " ".repeat(16) }`) }
            };

            const struct luaL_Reg meta_methods[] = {
                ${ meta_methods.map(([ename, fname]) => `{"${ ename }", ${ fname }}`).concat(["{NULL, NULL} // Sentinel"]).join(`,\n${ " ".repeat(16) }`) }
            };
        `.replace(/^ {12}/mg, "").trim());
    }

    static getDocCppType(processor, type, coclass, options) {
        let doctype;

        if (type.endsWith("*")) {
            doctype = `${ this.getDocCppType(processor, type.slice(0, -1).trim(), coclass, options) }*`;
        } else if (processor.typedefs.has(type) && processor.typedefs.get(type) != null && processor.typedefs.get(type).startsWith("struct ")) {
            doctype = type;
        } else {
            doctype = processor.getCppType(type, coclass, options);
        }

        return typeof options.getDocCppType === "function" ? options.getDocCppType(processor, doctype, coclass, options) : doctype;
    }

    static writeMethodDocs(
        processor,
        coclass,
        fname,
        decl,
        outlist,
        indexes,
        firstoptarg,
        isConstructor,
        options
    ) {
        const ename = LuaGenerator.getLuaFn(fname);
        const meta_method = LuaGenerator.getMetaMethod(ename);
        const {fqn} = coclass;
        const [name, return_value_type, func_modifiers, list_of_arguments] = decl;
        const argc = list_of_arguments.length;
        const isStatic = func_modifiers.includes("/S") || coclass.isStatic();
        const cppfqn = processor.hasTypeDef(fqn) ? processor.typedefs.get(fqn) : fqn;
        const cname = options.cname ? options.cname : "create";

        firstoptarg = Math.min(firstoptarg, argc);

        // generate docs body
        const argnamelist = Array.from(new Array(argc).keys()).map(i => {
            const j = indexes[i];
            const arg = list_of_arguments[j];
            return arg[0] === "..." ? arg[0] : arg[1];
        });

        let argstr = argnamelist.slice(0, firstoptarg).join(", ");
        argstr = [argstr].concat(argnamelist.slice(firstoptarg)).join("[, ");
        argstr += "]".repeat(argc - firstoptarg);
        if (argstr.startsWith("[, ")) {
            argstr = `[${ argstr.slice("[, ".length) }`;
        }

        let outstr;

        if (isConstructor) {
            outstr = `<${ cppfqn } object>`;
        } else if (outlist.length !== 0) {
            outstr = outlist.join(", ");
        } else {
            outstr = "None";
        }

        const caller = isStatic ? fqn.replaceAll("::", ".") : `o${ coclass.name }`;
        const is_call_fn = fname === "sol::meta_function::call" || fname === "sol::meta_function::call_function" || fname === "operator()";

        let description = is_call_fn ?
            `${ caller }( ${ argstr } ) -> ${ outstr }` :
            `${ caller }${ isStatic ? "." : ":" }${ isConstructor ? cname : meta_method }( ${ argstr } ) -> ${ outstr }`;

        if (isConstructor || coclass.is_vector && fname === cname) {
            description += `\n    ${ caller }( ${ argstr } ) -> ${ outstr }`;
        }

        const op = LuaGenerator.getBinaryOperator(ename);
        if (op) {
            const args = argnamelist.slice(0, firstoptarg);
            if (!isStatic) {
                args.unshift("self");
            }
            description += `\n    ${ args.join(` ${ op } `) } -> ${ outstr }`;
        }

        let cppsignature = `${ LuaGenerator.getDocCppType(processor, return_value_type, coclass, options) } ${ name.replaceAll(".", "::") }`;

        if (isConstructor) {
            cppsignature = cppfqn;
        }

        if (func_modifiers.includes("/C") && coclass.isStatic()) {
            cppsignature = `const ${ cppsignature }`;
        }

        if (!isConstructor && isStatic && !coclass.isStatic()) {
            cppsignature = `static ${ cppsignature }`;
        }

        let maxlength = 0;

        const typelist = list_of_arguments.map(([argtype, , , arg_modifiers]) => {
            let str = "";

            if (arg_modifiers.includes("/C")) {
                str += "const ";
            }

            const is_in_array = /^Input(?:Output)?Array(?:OfArrays)?$/.test(argtype);
            const is_out_array = /^(?:Input)?OutputArray(?:OfArrays)?$/.test(argtype);
            str += is_in_array || is_out_array ? argtype : LuaGenerator.getDocCppType(processor, argtype, coclass, options);

            if (arg_modifiers.includes("/Ref")) {
                str += "&";
            } else if (arg_modifiers.includes("/RRef")) {
                str += "&&";
            }
            maxlength = Math.max(maxlength, str.length);
            return str;
        });

        cppsignature = `${ cppsignature }( ${ list_of_arguments.map(([, argname, defval, arg_modifiers], i) => {
            let str = typelist[i] + " ".repeat(maxlength + 1 - typelist[i].length) + argname;
            if (defval !== "" && !arg_modifiers.includes("/IO") && !arg_modifiers.includes("/O")) {
                str += ` = ${ defval }`;
            }
            return str;
        }).join(`,\n${ " ".repeat(cppsignature.length + "( ".length) }`) } )`;

        if (func_modifiers.includes("/C") && !coclass.isStatic()) {
            cppsignature = `${ cppsignature } const`;
        }

        cppsignature += ";";

        processor.docs.push([
            "```cpp",
            cppsignature,
            // "",
            "lua:",
            " ".repeat(4) + description,
            "```",
            ""
        ].join("\n").replace(/\s*\( {2}\)/g, "()").replace(/\s*\[ +\]/g, "[]"));
    }

    generate(processor, configuration, options, cb) {
        const { generated_include } = configuration;

        const files = new Map();
        const registrationsHdr = [];
        const registrations = [];

        for (const fqn of Array.from(processor.classes.keys()).sort((a, b) => {
            if (a.startsWith("VectorOf") !== b.startsWith("VectorOf")) {
                return a.startsWith("VectorOf") ? 1 : -1;
            }

            if (a.startsWith("VariantOf") !== b.startsWith("VariantOf")) {
                return a.startsWith("VariantOf") ? 1 : -1;
            }

            if (a.startsWith("SharedPtrOf") !== b.startsWith("SharedPtrOf")) {
                return a.startsWith("SharedPtrOf") ? 1 : -1;
            }

            if (a.startsWith("PairOf") !== b.startsWith("PairOf")) {
                return a.startsWith("PairOf") ? 1 : -1;
            }

            if (a.startsWith("MapOf") !== b.startsWith("MapOf")) {
                return a.startsWith("MapOf") ? 1 : -1;
            }

            if (a.startsWith("CvVariantOf") !== b.startsWith("CvVariantOf")) {
                return a.startsWith("CvVariantOf") ? 1 : -1;
            }

            return a > b ? 1 : a < b ? -1 : 0;
        })) {
            const docid = processor.docs.length;

            const coclass = processor.classes.get(fqn);
            const fileCpp = coclass.getCPPFileName(options);
            const fileHdr = `${ fileCpp.slice(0, -".cpp".length) }.hpp`;
            const registerFn = LuaGenerator.getRegisterFn(coclass);
            const hasConstructor = Array.from(coclass.methods.values()).some(overloads => {
                return overloads.some(([, , func_modifiers]) => LuaGenerator.isConstructor(func_modifiers));
            });

            registrationsHdr.push(`#include <${ fileHdr }>`);
            registrations.push(`${ registerFn }(L);`);

            const registers = [`void ${ registerFn }(lua_State* L);`];
            const contentDecl = [];
            const path = getProgId(coclass.path.join("."), options).split(".");
            while (path.length !== 0 && path[0] === "") {
                path.shift();
            }
            coclass.progid = path.join(".");

            if (!coclass.isStatic()) {
                const decl = `
                    static int metatable;
                    static const void* signature;
                    static const struct luaL_Reg* methods;
                    static const struct luaL_Reg* meta_methods;
                    static const std::map<std::string, std::function<int(lua_State*)>> getters;
                    static const std::map<std::string, std::function<int(lua_State*)>> setters;
                    static std::shared_ptr<${ fqn }> lua_userdata_to(lua_State* L, int index, bool& is_valid);
                `.replace(/^ {20}/mg, "").trim().split("\n");

                const impl = `
                    int usertype_info<${ fqn }>::metatable = LUA_REFNIL;
                    const void* usertype_info<${ fqn }>::signature;
                    const struct luaL_Reg* usertype_info<${ fqn }>::methods = ::methods;
                    const struct luaL_Reg* usertype_info<${ fqn }>::meta_methods = ::meta_methods;
                    const std::map<std::string, std::function<int(lua_State*)>> usertype_info<${ fqn }>::getters(std::move(::getters));
                    const std::map<std::string, std::function<int(lua_State*)>> usertype_info<${ fqn }>::setters(std::move(::setters));

                    std::shared_ptr<${ fqn }> usertype_info<${ fqn }>::lua_userdata_to(lua_State* L, int index, bool& is_valid) {
                        return lua_userdata_signature_to<::${ [fqn, ...coclass.parents].join(", ::") }>(L, index, is_valid);
                    }
                `.replace(/^ {20}/mg, "").trim().split("\n");

                if (hasConstructor && coclass.modifiers?.includes("/DC")) {
                    decl.push(...`
                        static std::shared_ptr<${ fqn }> Lua_new(lua_State* L, const size_t __top__, bool& is_valid);
                    `.replace(/^ {24}/mg, "").trim().split("\n"));

                    impl.push("", ...`
                        std::shared_ptr<${ fqn }> usertype_info<${ fqn }>::Lua_new(lua_State* L, const size_t __top__, bool& is_valid) {
                            return ::Lua_new(L, __top__, is_valid);
                        }
                    `.replace(/^ {24}/mg, "").trim().split("\n"));
                }

                if (processor.derives.has(fqn)) {
                    decl.push(...`
                        static std::unordered_set<const void*> derives;
                        static std::unordered_map<std::type_index, std::function<int(lua_State*, const std::shared_ptr<${ fqn }>&)>> derives_pushers;
                    `.replace(/^ {24}/mg, "").trim().split("\n"));

                    impl.push(...`
                        std::unordered_set<const void*> usertype_info<${ fqn }>::derives;
                        std::unordered_map<std::type_index, std::function<int(lua_State*, const std::shared_ptr<${ fqn }>&)>> usertype_info<${ fqn }>::derives_pushers;
                    `.replace(/^ {24}/mg, "").trim().split("\n"));
                }

                registers.push("", `
                    template<>
                    struct is_usertype<${ fqn }> : std::true_type { };

                    template <>
                    struct usertype_info<${ fqn }> {
                        ${ decl.join(`\n${ " ".repeat(24) }`) }
                    };
                `.replace(/^ {20}/mg, "").trim());

                contentDecl.push(impl.join("\n"));
            } else if (processor.derives.has(fqn)) {
                registers.push("", `
                    template<>
                    struct is_basetype<${ fqn }> : std::true_type { };

                    template <>
                    struct basetype_info<${ fqn }> {
                        static int push(lua_State* L);
                    };
                `.replace(/^ {20}/mg, "").trim());

                contentDecl.push(`
                    int basetype_info<${ fqn }>::push(lua_State* L) {
                        return lua_rawget_create_if_nil(L, { ${ path.map(part => JSON.stringify(part)).join(", ") } });
                    }
                `.replace(/^ {20}/mg, "").trim());
            }

            files.set(sysPath.join(options.output, fileHdr), `
                #pragma once
                #include <lua_generated_include.hpp>
                #include <luadef.hpp>

                namespace LUA_MODULE_NAME {
                    ${ registers.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                }
            `.replace(/^ {16}/mg, "").trim());

            const contentCpp = ["#include <lua_generated_pch.hpp>", ""]; // GCC: Only one precompiled header can be used in a particular compilation.
            const contentRegisterPrivate = [];
            const contentRegister = [];

            if (coclass.is_enum_class) {
                if (path.length !== 0) {
                    contentRegister.push(`lua_rawget_create_if_nil(L, { ${ path.map(part => JSON.stringify(part)).join(", ") } });`, "");
                }

                contentRegister.push(Array.from(coclass.properties.keys()).map(name => `lua_pushliteral(L, "${ name }"); lua_push(L, ${ fqn }::${ name }); lua_rawset(L, -3);`).join("\n"));

                if (path.length !== 0) {
                    contentRegister.push("", "lua_pop(L, 1);");
                }

                contentCpp.push(`
                    namespace LUA_MODULE_NAME {
                        void ${ registerFn }(lua_State* L) {
                            ${ contentRegister.join("\n").split("\n").join(`\n${ " ".repeat(28) }`) }
                        }
                    }
                `.replace(/^ {20}/mg, "").trim());

                files.set(sysPath.join(options.output, fileCpp), contentCpp.join("\n").replace(/^[^\S\r\n]+$/mg, ""));
                continue;
            }

            const namespaces = [];
            useNamespaces(namespaces, "push", processor, coclass);

            if (coclass.isStatic()) {
                if (path.length !== 0) {
                    contentRegister.push(`lua_rawget_create_if_nil(L, { ${ path.map(part => JSON.stringify(part)).join(", ") } }); // push static class table`);
                }

                contentRegister.push(`
                    lua_pushfuncs(L, methods);

                    // push static class metatable
                    if (!lua_getmetatable(L, -1)) {
                        // metatable = {}
                        lua_newtable(L);

                        // setmetatable(module, metatable)
                        lua_setmetatable(L, -2);

                        // metatable = getmetatable(module)
                        lua_getmetatable(L, -1);
                    }

                    lua_pushfuncs(L, meta_methods);
                `.replace(/^ {20}/mg, "").trim());

                if (path.length !== 0) {
                    contentRegister.push("", "lua_pop(L, 1); // pop static class metatable");
                }
            } else {
                const name = path[path.length - 1];

                if (path.length > 1) {
                    contentRegister.push(`lua_rawget_create_if_nil(L, { ${ path.slice(0, -1).map(part => JSON.stringify(part)).join(", ") } });  // push parent class metatable`);
                }

                contentRegister.push(`lua_register_class<::${ [fqn, ...coclass.parents].join(", ::") }>(L, "${ name }");`);

                const parents = [...coclass.parents];

                // denormalize parents
                for (const parent of parents) {
                    if (processor.classes.has(parent) && !processor.classes.get(parent).isStatic()) {
                        contentRegister.push(`
                            usertype_info<::${ parent }>::derives.insert(usertype_info<${ fqn }>::signature);
                            usertype_info<::${ parent }>::derives_pushers[std::type_index(typeid(${ fqn }))] = std::move([] (lua_State* L, const std::shared_ptr<::${ parent }>& ptr) {
                                return lua_push(L, std::reinterpret_pointer_cast<${ fqn }>(ptr));
                            });
                        `.replace(/^ {28}/mg, "").trim());
                    }

                    if (processor.bases.has(parent)) {
                        for (const base of processor.bases.get(parent)) {
                            parents.push(base);
                        }
                    }
                }

                contentRegister.push(`lua_pushliteral(L, "${ name }");`);
                contentRegister.push("lua_rawget(L, -2); // push class metatable");
            }

            LuaGenerator.writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options);

            if (!coclass.isStatic() || coclass.properties.size !== 0 && coclass.methods.size !== 0) {
                // new line
                contentRegisterPrivate.push("");
                contentRegister.push("");
            }

            LuaGenerator.writeMethods(processor, coclass, contentRegisterPrivate, contentRegister, options);

            if (contentRegisterPrivate.length !== 0) {
                let start = 0;
                while (contentRegisterPrivate[start] === "" && start + 1 < contentRegisterPrivate.length) {
                    start++;
                }

                contentCpp.push(`
                    namespace {
                        using namespace LUA_MODULE_NAME;
                        ${ namespaces.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }

                        ${ contentRegisterPrivate.slice(start).join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                    }
                `.replace(/^ {20}/mg, "").trim());
                contentCpp.push("");
            }

            contentRegister.push(`lua_pop(L, 1); // pop ${ !coclass.isStatic() ? "class metatable" : path.length !== 0 ? "static class table" : "static class metatable" }`);

            if (path.length > 1 && !coclass.isStatic()) {
                contentRegister.push("lua_pop(L, 1); // pop parent metatable");
            }

            contentCpp.push(`
                namespace LUA_MODULE_NAME {
                    ${ contentDecl.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }

                    void ${ registerFn }(lua_State* L) {
                        ${ contentRegister.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                    }
                }
            `.replace(/^ {16}/mg, "").trim());

            files.set(sysPath.join(options.output, fileCpp), contentCpp.join("\n").replace(/^[^\S\r\n]+$/mg, ""));

            processor.docs.splice(docid, 0, `## ${ fqn.replaceAll("_", "\\_") }\n`);
        }

        files.set(sysPath.join(options.output, "register_all.hpp"), `
            #pragma once
            ${ registrationsHdr.join(`\n${ " ".repeat(12) }`) }

            namespace LUA_MODULE_NAME {
                void register_all(lua_State* L);
            }
        `.replace(/^ {12}/mg, "").trim());

        files.set(sysPath.join(options.output, "register_all.cpp"), `
            #include <register_all.hpp>

            namespace LUA_MODULE_NAME {
                void register_all(lua_State* L) {
                    ${ registrations.join(`\n${ " ".repeat(20) }`) }
                }
            }
        `.replace(/^ {12}/mg, "").trim());

        if (options.hdr !== false) {
            files.set(
                sysPath.join(options.output, "lua_generated_include.hpp"),
                [
                    "#pragma once\n",
                    ...generated_include,

                    "",

                    ...Array.from(processor.typedefs).filter(([fqn]) => processor.hasTypeDef(fqn)).map(([fqn, cpptype]) => {
                        const parts = fqn.split("::");
                        const last = parts.length - 1;
                        const begin = new Array(last);
                        const end = new Array(last);
                        for (let i = 0; i < last; i++) {
                            const indent = " ".repeat(4 * i);
                            begin[i] = `${ indent }namespace ${ parts[i] } {`;
                            end[last - 1 - i] = `${ indent }}`;
                        }

                        const name = parts[last];
                        const indent = " ".repeat(4 * (last));
                        return begin.concat(`${ indent }using ${ name } = ${ cpptype };`, end).join("\n");
                    })
                ].join("\n").trim().replace(/[^\S\n]+$/mg, "")
            );

            files.set(
                sysPath.join(options.output, "lua_generated_pch.hpp"),
                [
                    "#pragma once\n",
                    "#include <registration.hpp>",
                ].join("\n").trim().replace(/[^\S\n]+$/mg, "")
            );
        }

        for (const fqn of Object.keys(knwon_ids)) {
            if (!processor.classes.has(fqn) || processor.classes.get(fqn).noidl) {
                delete knwon_ids[fqn];
            }
        }
        files.set(sysPath.join(__dirname, "ids.json"), JSON.stringify(knwon_ids, null, 4));

        const docs = sysPath.resolve(options.output, "..", "docs", "docs.md");

        if (options.toc !== false) {
            processor.docs.unshift("");

            try {
                fs.accessSync(docs, fs.constants.R_OK);
                const content = fs.readFileSync(docs).toString();
                const start = content.indexOf("<!-- START doctoc ");
                const endpos = content.indexOf("<!-- END doctoc ", start + 1);
                const prev_doctoc = content.slice(start, content.indexOf(" -->", endpos + 1) + " -->".length);
                processor.docs.unshift(prev_doctoc);
            } catch (err) {
                processor.docs.unshift("<!-- END doctoc -->");
                processor.docs.unshift("<!-- START doctoc -->");
            }

            processor.docs.unshift("");
            processor.docs.unshift("## Table Of Contents");
        }

        processor.docs.unshift("");
        processor.docs.unshift(`
            # Lua ${ options.APP_NAME } Binding
        `.replace(/^ {12}/mg, "").trim());

        files.set(docs, processor.docs.join("\n"));

        waterfall([
            next => {
                FileUtils.writeFiles(files, options, next);
            },

            next => {
                FileUtils.deleteFiles(options.output, files, options, next);
            },
        ], cb);
    }
}

module.exports = LuaGenerator;
