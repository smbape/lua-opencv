/* eslint-disable no-magic-numbers */
const version = process.env.npm_package_version || require("../package.json").version;
const fs = require("node:fs");
const sysPath = require("node:path");
const waterfall = require("async/waterfall");

const [VERSION_MAJOR, VERSION_MINOR] = version.split(".");

const knwon_ids = require("./ids");
const FileUtils = require("./FileUtils");
const {
    makeExpansion,
    useNamespaces,
    getTypeDef
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
        }
    },

    add_map(cpptype, parent, options) {
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

        coclass.addMethod([`${ fqn }.new`, `std::shared_ptr<${ coclass.name }>`, ["/Call=lua_map_new", "/S"], [
            [`std::vector<std::pair<${ key_type }, ${ value_type }>>`, "pairs", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.size`, "size_t", ["=sol::meta_function::length"], [], "", ""], options);
        coclass.addMethod([`${ fqn }.table`, "void", ["/Call=lua_push", `/Expr=L, ${ self }`], [], "", ""], options);

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
        const fqn = getTypeDef(cpptype, options);
        if (this.classes.has(fqn) && this.getCoClass(fqn, options).is_vector) {
            return;
        }

        const vtype = cpptype.slice("std::vector<".length, -">".length);

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

        coclass.addMethod([`${ fqn }.size`, "size_t", ["=sol::meta_function::length"], [], "", ""], options);
        coclass.addMethod([`${ fqn }.table`, "void", ["/Call=lua_push", `/Expr=L, ${ self }`], [], "", ""], options);

        coclass.addMethod([`${ fqn }.at`, vtype, ["=sol::meta_function::index", "/Call=lua_vector_method__index", `/Expr=L, ${ self }, $0`], [
            ["size_t", "index", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.at`, vtype, ["=sol::meta_function::index", "/Call=lua_vector_method__index", `/Expr=L, ${ self }, $0`], [
            ["std::string", "index", "", ["/Ref", "/C"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::new_index`, "void", ["/Call=lua_vector_method__newindex", `/Expr=L, ${ self }, $0`], [
            ["size_t", "index", "", []],
            [vtype, "value", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::new_index`, "void", ["/Call=lua_vector_method__newindex", `/Expr=L, ${ self }, $0`], [
            ["std::string", "index", "", ["/Ref", "/C"]],
            [vtype, "value", "", []],
        ], "", ""], options);

        this.addDependencies(coclass, options);
    },
};

const conditional = (...args) => {
    const [is_optional] = args.slice(-2);
    if (!is_optional) {
        args = args.slice(0, -2);
    }

    const text = [];

    for (let i = 0; i < args.length; i += 2) {
        if (!args[i + 1]) {
            continue;
        }

        const has_more = i + 2 < args.length;
        const condition = text.length === 0 ? `if (${ args[i] })` : has_more ? `else if (${ args[i] })` : "else";

        text.push(`
            ${ condition } {
                ${ args[i + 1].split("\n").join(`\n${ " ".repeat(16) }`) }
            }
        `.replace(/^ {12}/mg, "").trim());
    }

    return text.join(" ");
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

const generatePropertyDoc = (processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput) => {
    const { fqn } = coclass;

    // generate docs header
    processor.docs.push(`### ${ coclass.name }.${ name }\n`.replaceAll("_", "\\_"));

    const cppsignature = [];
    if (modifiers.includes("/S") && !coclass.isStatic()) {
        cppsignature.push("static");
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

    const is_static = modifiers.includes("/S") || coclass.isStatic();

    processor.docs.push([
        "```cpp",
        cppsignature.join(" "),
        // "",
        "lua:",
        `${ " ".repeat(4) }[${ attributes.join(", ") }] ${ is_static ? "" : "o" }${ coclass.name }.${ name }`,
        "```",
        ""
    ].join("\n").replace(/\s*\( {2}\)/g, "()"));
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

    static writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const dynamic_get = [];
        const dynamic_set = [];
        const registeri = contentRegister.length;

        for (const [name, property] of coclass.properties.entries()) {
            const {type, modifiers} = property;
            const cpptype = processor.getCppType(type, coclass, options);
            const is_static = modifiers.includes("/S") || coclass.isStatic();
            const obj = `${ is_static ? `${ fqn }::` : "self->" }`;
            const enum_fqn = processor.getEnumType(type, coclass, options);
            const is_enum_class = enum_fqn && processor.classes.has(enum_fqn) && processor.classes.get(enum_fqn).is_enum_class;
            const as_type = enum_fqn ? enum_fqn : cpptype;
            const is_enum = modifiers.includes("/Enum");

            let propname = name;
            let getter, setter;
            let has_propget = is_static || is_enum || modifiers.includes("/R") || modifiers.includes("/RW");
            let has_propput = modifiers.includes("/W") || modifiers.includes("/RW");
            let is_key = false;

            for (const modifier of modifiers) {
                if (modifier[0] === "=") {
                    propname = modifier.slice(1);
                } else if (modifier[0] === "/idlname=") {
                    propname = modifier.slice("/idlname=".length);
                } else if (modifier.startsWith("/R=")) {
                    getter = modifier.slice("/R=".length);
                    has_propget = true;
                } else if (modifier.startsWith("/RExpr=")) {
                    has_propget = true;
                } else if (modifier.startsWith("/W=")) {
                    setter = modifier.slice("/W=".length);
                    has_propput = true;
                } else if (modifier.startsWith("/WExpr=")) {
                    has_propput = true;
                } else if (modifier === "/Key") {
                    is_key = true;
                }
            }

            if (!has_propput) {
                has_propget = true;
            }

            let rexpr = null;
            let wexpr = null;

            if (has_propget) {
                rexpr = `${ obj }${ getter ? `${ getter }()` : propname }`;

                for (const modifier of modifiers) {
                    if (modifier.startsWith("/RExpr=")) {
                        rexpr = makeExpansion(modifier.slice("/RExpr=".length), rexpr);
                    }
                }

                if (is_enum) {
                    if (registeri === contentRegister.length) {
                        contentRegister.push(""); // new line
                    }
                    contentRegister.push(`lua_pushliteral(L, "${ name }"); lua_push(L, ${ rexpr }); lua_rawset(L, -3);`);
                    generatePropertyDoc(processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput);
                    continue;
                }

                if (rexpr.endsWith("::this")) {
                    rexpr = `lua_rawget_create_if_nil(L, { ${ rexpr.split("::").map(part => JSON.stringify(part)).join(", ") } })`;
                } else {
                    for (const modifier of modifiers) {
                        if (modifier.startsWith("/Cast=")) {
                            rexpr = `${ modifier.slice("/Cast=".length) }(${ rexpr })`;
                        }
                    }
                    rexpr = `lua_push(L, ${ rexpr });`;
                }
            }

            let in_val;

            if (has_propput) {
                if (setter) {
                    setter = `${ obj }${ setter }`;
                }

                in_val = `extract_holder(value, static_cast<${ as_type }*>(nullptr))`;

                if (enum_fqn && !is_enum_class) {
                    // input must be int and cast as enum
                    in_val = `static_cast<${ enum_fqn }>(${ in_val })`;
                }

                let has_expr = false;
                for (const modifier of modifiers) {
                    if (modifier.startsWith("/WExpr=")) {
                        in_val = makeExpansion(modifier.slice("/WExpr=".length), in_val);
                        has_expr = true;
                    }
                }

                if (!has_expr) {
                    in_val = setter ? `${ setter }(${ in_val })` : `${ obj }${ propname } = ${ in_val }`;
                }

                wexpr = `
                    if (!lua_is(L, 3, static_cast<${ as_type }*>(nullptr))) {
                        return luaL_typeerror(L, 3, ${ JSON.stringify(cpptype) });
                    }
                    auto value = lua_to(L, 3, static_cast<${ as_type }*>(nullptr));
                    ${ in_val.split("\n").join(`\n${ " ".repeat(24) }`) };
                    return 0;
                `.replace(/^ {20}/mg, "").trim();
            }

            const key_name = is_key ? name : `"${ name }"`;
            const obj_decl = [];

            if (!is_static) {
                obj_decl.push(`
                    if (!lua_is(L, 1, static_cast<::${ fqn }*>(nullptr))) {
                        return luaL_typeerror(L, 1, ${ JSON.stringify(fqn) });
                    }
                    ::${ fqn }* self = lua_to(L, 1, static_cast<::${ fqn }*>(nullptr)).get();
                `.replace(/^ {20}/mg, "").trim());
                obj_decl.push("");
            }

            dynamic_get.push(`
                { ${ key_name }, [](lua_State* L) {
                    ${ obj_decl.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                    ${ rexpr.split("\n").join(`\n${ " ".repeat(20) }`) }
                    return 1;
                }}
            `.replace(/^ {16}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));

            if (wexpr) {
                dynamic_set.push(`
                    { ${ key_name }, [](lua_State* L) {
                        ${ obj_decl.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                        ${ wexpr.split("\n").join(`\n${ " ".repeat(24) }`) }
                    }}
                `.replace(/^ {20}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));
            }

            generatePropertyDoc(processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput);
        }

        if (!coclass.isStatic()) {
            contentRegisterPrivate.push("", `
                std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> getters({
                    ${ dynamic_get.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });
            `.replace(/^ {16}/mg, "").trim().replace(/\{\s+\}/mg, "{}"));
        } else if (dynamic_get.length !== 0) {
            contentRegisterPrivate.push("", `
                const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> getters({
                    ${ dynamic_get.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                int dynamic_get(lua_State* L) {
                    if (lua_is(L, 2, static_cast<std::string*>(nullptr))) {
                        const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr));
                        if (getters.count(k)) {
                            return getters.at(k)(L);
                        }
                    }

                    if (lua_is(L, 2, static_cast<int*>(nullptr))) {
                        const int k = lua_to(L, 2, static_cast<int*>(nullptr));
                        if (getters.count(k)) {
                            return getters.at(k)(L);
                        }
                    }

                    return 0;
                }
            `.replace(/^ {16}/mg, "").trim());
        }

        if (!coclass.isStatic()) {
            contentRegisterPrivate.push("", `
                std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> setters({
                    ${ dynamic_set.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });
            `.replace(/^ {16}/mg, "").trim().replace(/\{\s+\}/mg, "{}"));
        } else if (dynamic_set.length !== 0) {
            contentRegisterPrivate.push("", `
                const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> setters({
                    ${ dynamic_set.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                void dynamic_set(lua_State* L) {
                    if (lua_is(L, 2, static_cast<std::string*>(nullptr))) {
                        const std::string k = lua_to(L, 2, static_cast<std::string*>(nullptr));
                        if (setters.count(k)) {
                            return setters.at(k)(L);
                        }
                    }

                    if (lua_is(L, 2, static_cast<int*>(nullptr))) {
                        const int k = lua_to(L, 2, static_cast<int*>(nullptr));
                        if (setters.count(k)) {
                            return setters.at(k)(L);
                        }
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

                contentRegister.push("", `
                    lua_getmetatable(L, -1);
                    lua_pushfuncs(L, index_methods);
                    lua_pop(L, 1);
                `.replace(/^ {20}/mg, "").trim());
            }
        }
    }

    static writeMethods(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const { shared_ptr } = options;
        const indent = " ".repeat(4);
        const methods = [];
        const meta_methods = [];

        for (const fname of Array.from(coclass.methods.keys()).sort((a, b) => {
            if (a === "new") {
                return -1;
            }

            if (b === "new") {
                return 1;
            }

            return a > b ? 1 : a < b ? -1 : 0;
        })) {
            const ename = LuaGenerator.getLuaFn(fname);
            const overloads = coclass.methods.get(fname);
            const contentFunction = [];

            const has_constructor = overloads.some(([, , func_modifiers]) => func_modifiers.includes("/CO") && !func_modifiers.some(modifier => modifier[0] === "="));

            // generate docs header
            processor.docs.push(`### ${ fqn.replaceAll("::", ".") }.${ has_constructor ? "new" : fname }\n`.replaceAll("_", "\\_"));

            let is_constructor = false;
            let overload_id = 0;
            let argc_max = 0;
            const argnames = new Set();

            for (const decl of overloads) {
                overload_id++;

                const [name, return_value_type, func_modifiers, list_of_arguments] = decl;
                is_constructor = func_modifiers.includes("/CO") && !func_modifiers.some(modifier => modifier[0] === "=");
                const argc = list_of_arguments.length;
                argc_max = Math.max(argc_max, argc);

                const in_args = new Array(argc).fill(false);
                const out_args = new Array(argc).fill(false);
                const out_array_args = new Array(argc).fill(false);

                const outlist = [];

                if (return_value_type !== "" && return_value_type !== "void") {
                    outlist.push("retval");
                } else if (is_constructor) {
                    outlist.push("self");
                }

                for (let j = 0; j < argc; j++) {
                    const [argtype, argname, defval, arg_modifiers] = list_of_arguments[j];
                    const is_ptr = argtype.endsWith("*");
                    const is_in_array = /^Input(?:Output)?Array(?:OfArrays)?$/.test(argtype);
                    const is_out_array = /^(?:Input)?OutputArray(?:OfArrays)?$/.test(argtype);
                    const is_in_out = arg_modifiers.includes("/IO");

                    argnames.add(argname);

                    in_args[j] = is_in_array || is_in_out || arg_modifiers.includes("/I");
                    out_args[j] = is_out_array || is_in_out || arg_modifiers.includes("/O") ||
                        is_ptr && defval === "" && SIMPLE_ARGTYPE_DEFAULTS.has(argtype.slice(0, -1));
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

                const is_static = func_modifiers.includes("/S") || coclass.isStatic();
                const callargs = [];
                const retval = [];
                const overload = [];
                const precondition = [];

                if (!is_static) {
                    precondition.push("!self");
                }

                if (ename.startsWith("sol::meta_function::") && overloads.length === 1) {
                    // there are no default arguments with meta methods
                    // the number of arguments should be strict
                    // however, some lua meta methods have extra implementation detail arguments
                    // https://web.archive.org/web/20230310005851/http://lua-users.org/lists/lua-l/2010-01/msg00149.html
                    // therefore, validate that at least the number of expected parameters are present
                    precondition.push(`argc + kwargc < ${ is_static ? argc : argc + 1 }`);
                } else {
                    precondition.push(`argc + kwargc > ${ is_static ? argc : argc + 1 }`);
                }

                overload.push(`
                    if (${ precondition.join(" || ") }) {
                        // wrong number of paramters
                        goto overload${ overload_id };
                    }

                    int usedkw = 0;
                `.replace(/^ {20}/mg, "").trim());

                let firstoptarg = argc;

                for (let i = 0, is_first_optional = true; i < argc; i++) {
                    const j = indexes[i];
                    const [, argname, , arg_modifiers] = list_of_arguments[j];
                    let [argtype, , defval] = list_of_arguments[j];

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
                    }

                    let callarg = argname;
                    let cpptype = processor.getCppType(argtype, coclass, options);

                    if (is_out_arg && is_ptr && !PTR.has(argtype)) {
                        callarg = `&${ callarg }`;
                        argtype = argtype.slice(0, -1);
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

                    cpptype = processor.getCppType(argtype, coclass, options);

                    if (cpptype === "char*") {
                        cpptype = "std::string";
                        callarg = `${ callarg }.c_str()`;
                    }

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
                    const in_type = is_shared_ptr ? cpptype.slice(`${ shared_ptr }<`.length, -">".length) : cpptype;
                    const var_type = is_array ? arr_cpptype : in_type;
                    const lua_is = `lua_is${ arg_suffix }`;
                    const lua_to = `lua_to${ arg_suffix }`;
                    const argi = is_static ? i : i + 1;
                    const argn = argi + 1;

                    if (is_out_arg && is_array) {
                        is_optional = true;
                    }

                    if (is_out_arg) {
                        if (is_array) {
                            retval.push([j, `lua_push(L, ${ argname }_${ arrtype });`]);
                        } else {
                            retval.push([j, `lua_push(L, ${ argname });`]);
                        }
                    }

                    overload.push("", `
                        // =========================
                        // get argument ${ argname }
                        // =========================
                        bool ${ argname }_positional = false;
                        bool ${ argname }_kwarg = false;
                        if (argc > ${ argi }) {
                            // positional parameter
                            ${ argname }_positional = ${ lua_is }(L, ${ argn }, static_cast<${ var_type }*>(nullptr));
                            if (!${ argname }_positional) {
                                goto overload${ overload_id };
                            }

                            // should not be a named parameter
                            if (has_kwargs && Keywords::has(L, vargc, "${ argname }")) {
                                goto overload${ overload_id };
                            }
                        }
                        else if (has_kwargs && Keywords::has(L, vargc, "${ argname }")) {
                            // named parameter
                            Keywords::push(L, vargc, "${ argname }");
                            ${ argname }_kwarg = ${ lua_is }(L, -1, static_cast<${ var_type }*>(nullptr));
                            lua_pop(L, 1);
                            if (!${ argname }_kwarg) {
                                goto overload${ overload_id };
                            }
                            usedkw++;
                        }
                    `.replace(/^ {24}/mg, "").trim());

                    if (!is_optional) {
                        overload.push(`
                            else {
                                // mandatory parameter
                                goto overload${ overload_id };
                            }
                        `.replace(/^ {28}/mg, "").trim());
                    }

                    overload.push(""); // new line

                    if (is_optional) {
                        const ref = defval !== "" && is_by_ref && !defval.includes("(") && !defval.includes("'") && !defval.includes("\"") && !/^(?:\.|\d+\.)\d+$/.test(defval) ? "&" : "";
                        const copy = ref || is_out_arg || defval === "" || defval.includes("(");
                        overload.push(`${ copy ? "" : "static " }${ cpptype }${ ref } ${ argname }_default${ defval !== "" ? ` = ${ defval }` : "" };`);
                    }

                    if (is_array) {
                        overload.push(`
                            Optional${ arg_suffix[0].toUpperCase() + arg_suffix.slice(1) }<${ var_type }> ${ argname }_${ arrtype };
                            ${ conditional(...[
                                `${ argname }_positional`,
                                `${ argname }_${ arrtype } = ${ lua_to }(L, ${ argn }, static_cast<${ var_type }*>(nullptr));`,
                                `${ argname }_kwarg`,
                                [
                                    `Keywords::push(L, vargc, "${ argname }");`,
                                    `${ argname }_${ arrtype } = ${ lua_to }(L, -1, static_cast<${ var_type }*>(nullptr));`,
                                    "lua_pop(L, 1);",
                                ].join("\n"),
                                is_optional,
                                false,
                            ]).split("\n").join(`\n${ " ".repeat(28) }`) }

                            decltype(auto) ${ argname } = *${ argname }_${ arrtype };
                        `.replace(/^ {28}/mg, "").trim());
                    } else if (is_shared_ptr) {
                        overload.push(`
                            ${ cpptype } ${ argname };
                            ${ conditional(...[
                                `${ argname }_positional`,
                                `${ argname } = ${ lua_to }(L, ${ argn }, static_cast<${ var_type }*>(nullptr));`,
                                `${ argname }_kwarg`,
                                [
                                    `Keywords::push(L, vargc, "${ argname }");`,
                                    `${ argname } = ${ lua_to }(L, -1, static_cast<${ var_type }*>(nullptr));`,
                                    "lua_pop(L, 1);",
                                ].join("\n"),
                                is_optional,
                                `${ argname } = ${ argname }_default;`,
                            ]).split("\n").join(`\n${ " ".repeat(28) }`) }
                        `.replace(/^ {28}/mg, "").trim());
                    } else {
                        overload.push(`
                            if (${ argname }_kwarg) {
                                Keywords::push(L, vargc, "${ argname }");
                            }

                            using ${ argname }_Holder = decltype(lua_to(L, ${ argn }, static_cast<${ var_type }*>(nullptr)));
                            ${ argname }_Holder ${ argname }_holder;

                            ${ conditional(...[
                                `${ argname }_positional`,
                                `${ argname }_holder = ${ lua_to }(L, ${ argn }, static_cast<${ var_type }*>(nullptr));`,
                                `${ argname }_kwarg`,
                                `${ argname }_holder = ${ lua_to }(L, -1, static_cast<${ var_type }*>(nullptr));`,
                                is_optional,
                                false,
                            ]).split("\n").join(`\n${ " ".repeat(28) }`) }

                            decltype(auto) ${ argname } = extract_holder(${ argname }_holder, ${
                                is_optional ? `${ argname }_default, !${ argname }_positional && !${ argname }_kwarg` :
                                `static_cast<${ var_type }*>(nullptr)`
                            });

                            if (${ argname }_kwarg) {
                                lua_pop(L, 1);
                            }
                        `.replace(/^ {28}/mg, "").trim());
                    }

                    if (is_optional && is_first_optional) {
                        firstoptarg = Math.min(firstoptarg, i);
                        is_first_optional = false;
                    }
                }

                overload.push("", `
                    // =========================
                    // call ${ name.replaceAll(".", "::") }
                    // =========================

                    // unknown named parameters
                    if (usedkw != kwargc) {
                        goto overload${ overload_id };
                    }
                `.replace(/^ {20}/mg, "").trim());

                let callee;
                const path = name.split(is_constructor ? "::" : ".");
                let is_operator = false;

                if (is_static) {
                    callee = `::${ path.join("::") }`;
                } else {
                    callee = "self";

                    for (const modifier of func_modifiers) {
                        if (modifier.startsWith("/Cast=")) {
                            callee = `${ modifier.slice("/Cast=".length) }(${ callee })`;
                        } else if (modifier.startsWith("/Prop=")) {
                            callee = `${ callee }->${ modifier.slice("/Prop=".length) }`;
                        }
                    }

                    // [+\-*/%\^&|!=<>]=?|[~,]|(?:<<|>>)=?|&&|\|\||\+\+|--|->\*?
                    if (callargs.length === 0 && /^operator\s*(?:[+\-*/%\^&|!=<>]=?|[~,]|(?:<<|>>)=?|&&|\|\||\+\+|--|->\*?)$/.test(path[path.length - 1])) {
                        const operator = path[path.length - 1].slice("operator".length).trim();
                        callee = `${ operator }(*${ callee })`;
                        is_operator = true;
                    } else if (callargs.length === 1 && /^operator\s*(?:[+\-*/%\^&|!=<>]=?|[~,]|(?:<<|>>)=?|&&|\|\||\+\+|--|->\*?)$/.test(path[path.length - 1])) {
                        const operator = path[path.length - 1].slice("operator".length).trim();
                        callee = `(*${ callee }) ${ operator } `;
                        is_operator = true;
                    } else {
                        callee = `${ callee }->${ path[path.length - 1] }`;
                    }
                }

                let expr = callargs.join(", ");
                let has_call = false;

                for (const modifier of func_modifiers) {
                    if (modifier.startsWith("/Expr=")) {
                        expr = makeExpansion(modifier.slice("/Expr=".length), expr);
                    } else if (modifier.startsWith("/Call=")) {
                        callee = makeExpansion(modifier.slice("/Call=".length), callee);
                        has_call = true;
                    }
                }

                if (!is_operator || expr) {
                    callee = `${ callee }(${ expr })`;
                }

                if (is_constructor && !has_call) {
                    callee = `std::shared_ptr<${ fqn }>(new ${ callee })`;
                } else if (func_modifiers.includes("/Ref")) {
                    callee = `reference_internal(${ callee })`;
                }

                for (const modifier of func_modifiers) {
                    if (modifier.startsWith("/WrapAs=")) {
                        callee = `${ modifier.slice("/WrapAs=".length) }(${ callee })`;
                    }
                }

                for (const modifier of func_modifiers) {
                    if (modifier.startsWith("/Output=")) {
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

                if (has_body) {
                    overload.push("", `${ callee };`);

                    // body is responsible of return
                    retval.length = 0;
                } else if (return_value_type !== "void") {
                    retval.push([-1, `lua_push(L, ${ callee });`]);
                } else {
                    overload.push("", `${ callee };`);
                }

                if (retval.length !== 0) {
                    retval.sort(([a], [b]) => a - b);
                    overload.push("", retval.map(([, result]) => result).join("\n"));
                    overload.push("", "return lua_gettop(L) - vargc;");
                } else if (!has_body) {
                    // body is responsible of the return
                    overload.push("", "return lua_gettop(L) - vargc;");
                }

                contentFunction.push("");

                contentFunction.push("{");

                contentFunction.push(indent + overload.join("\n").split("\n").join(`\n${ indent }`));

                contentFunction.push("}");

                contentFunction.push(`overload${ overload_id }:`);

                LuaGenerator.writeMethodDocs(
                    processor,
                    coclass,
                    fname,
                    decl,
                    outlist,
                    indexes,
                    firstoptarg,
                    is_constructor,
                    options
                );
            }

            const lua_fname = `Lua_${ ename.replaceAll("::", "_") }`;

            let start = 0;
            while (contentFunction[start] === "" && start + 1 < contentFunction.length) {
                start++;
            }

            contentRegisterPrivate.push(`auto ${ lua_fname }(lua_State* L) {`);

            if (!coclass.isStatic()) {
                contentRegisterPrivate.push(`    ::${ fqn }* self = lua_is(L, 1, static_cast<${ fqn }*>(nullptr)) ? lua_to(L, 1, static_cast<${ fqn }*>(nullptr)).get() : nullptr;`, "");
            }

            // usertype_push_metatable<T>(L);
            let not_found = "LUAL_MODULE_ERROR_RETURN(L, \"Overload resolution failed\")";
            if (!coclass.isStatic()) {
                if (ename === "__index" || ename === "sol::meta_function::index") {
                    not_found = `return lua_class__index<0, ::${ [fqn, ...coclass.parents].join(", ::") }>(L)`;
                } else if (ename === "__newindex" || ename === "sol::meta_function::new_index") {
                    not_found = `return lua_class__newindex<0, ::${ [fqn, ...coclass.parents].join(", ::") }>(L)`;
                }
            }

            contentRegisterPrivate.push(`
                    if (is_call_garbage_collect()) {
                        lua_gc(L, LUA_GCCOLLECT, 0);
                    }

                    auto vargc = lua_gettop(L);
                    auto has_kwargs = vargc != 0 && lua_is(L, vargc, static_cast<Keywords*>(nullptr));
                    auto kwargc = has_kwargs ? Keywords::size(L, vargc) : 0;
                    const int argc = has_kwargs ? vargc - 1 : vargc;

                    ${ contentFunction.slice(start).join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }

                    ${ not_found };
                }
            `.replace(/^ {16}/mg, "").replace(/(?:^\n|\s+$)/, ""));

            // new line
            contentRegisterPrivate.push("");

            if (ename.startsWith("sol::meta_function::")) {
                const meta_method = LuaGenerator.getMetaMethod(ename);
                methods.push([meta_method, lua_fname]);
            } else {
                methods.push([ename, lua_fname]);
            }

            if (is_constructor) {
                meta_methods.push(["__call", "__call_constructor"]);
            }
        }

        if (!coclass.isStatic()) {
            methods.unshift(["__gc", `lua_method__gc<::${ fqn }>`]);
            methods.unshift(["isinstance", `lua_method_isinstance<::${ fqn }>`]);
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

    static writeMethodDocs(
        processor,
        coclass,
        fname,
        decl,
        outlist,
        indexes,
        firstoptarg,
        is_constructor,
        options
    ) {
        const ename = LuaGenerator.getLuaFn(fname);
        const meta_method = LuaGenerator.getMetaMethod(ename);
        const {fqn} = coclass;
        const [name, return_value_type, func_modifiers, list_of_arguments] = decl;
        const argc = list_of_arguments.length;
        const is_static = func_modifiers.includes("/S") || coclass.isStatic();
        const cppfqn = processor.typedefs.has(fqn) ? processor.typedefs.get(fqn) : fqn;

        // generate docs body
        const argnamelist = indexes.map(j => list_of_arguments[j][1]);

        let argstr = argnamelist.slice(0, firstoptarg).join(", ");
        argstr = [argstr].concat(argnamelist.slice(firstoptarg)).join("[, ");
        argstr += "]".repeat(argc - firstoptarg);
        if (argstr.startsWith("[, ")) {
            argstr = `[${ argstr.slice("[, ".length) }`;
        }

        let outstr;

        if (is_constructor) {
            outstr = `<${ cppfqn } object>`;
        } else if (outlist.length !== 0) {
            outstr = outlist.join(", ");
        } else {
            outstr = "None";
        }

        const caller = is_static ? fqn.replaceAll("::", ".") : `o${ coclass.name }`;
        const is_call_fn = fname === "sol::meta_function::call" || fname === "sol::meta_function::call_function" || fname === "operator()";

        let description = is_call_fn ?
            `${ caller }( ${ argstr } ) -> ${ outstr }` :
            `${ caller }${ is_static ? "." : ":" }${ is_constructor ? "new" : meta_method }( ${ argstr } ) -> ${ outstr }`;

        if (is_constructor || coclass.is_vector && fname === "new") {
            description += `\n    ${ caller }( ${ argstr } ) -> ${ outstr }`;
        }

        const op = LuaGenerator.getBinaryOperator(ename);
        if (op) {
            const args = argnamelist.slice(0, firstoptarg);
            if (!is_static) {
                args.unshift("self");
            }
            description += `\n    ${ args.join(` ${ op } `) } -> ${ outstr }`;
        }

        let cppsignature = `${ processor.typedefs.has(return_value_type) ? processor.typedefs.get(return_value_type) : processor.getCppType(return_value_type, coclass, options) } ${ name.replaceAll(".", "::") }`;

        if (is_constructor) {
            cppsignature = cppfqn;
        } else if (is_static && !coclass.isStatic()) {
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
            str += is_in_array || is_out_array ? argtype : processor.getCppType(argtype, coclass, options);

            if (arg_modifiers.includes("/Ref")) {
                str += "&";
            } else if (arg_modifiers.includes("/RRef")) {
                str += "&&";
            }
            maxlength = Math.max(maxlength, str.length);
            return str;
        });

        cppsignature = `${ cppsignature }( ${ list_of_arguments.map(([, argname, defval], i) => {
            let str = typelist[i] + " ".repeat(maxlength + 1 - typelist[i].length) + argname;
            if (defval !== "") {
                str += ` = ${ defval }`;
            }
            return str;
        }).join(`,\n${ " ".repeat(cppsignature.length + "( ".length) }`) } )`;

        if (func_modifiers.includes("/C")) {
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
        const { APP_NAME } = options;

        const files = new Map();
        const registrationsHdr = [];
        const registrations = [];

        for (const fqn of Array.from(processor.classes.keys()).sort()) {
            const docid = processor.docs.length;

            const coclass = processor.classes.get(fqn);
            const fileCpp = coclass.getCPPFileName(options);
            const fileHdr = `${ fileCpp.slice(0, -".cpp".length) }.hpp`;
            const registerFn = LuaGenerator.getRegisterFn(coclass);

            registrationsHdr.push(`#include <${ fileHdr }>`);
            registrations.push(`${ registerFn }(L);`);

            const registers = [`void ${ registerFn }(lua_State* L);`];
            const contentDecl = [];

            if (!coclass.isStatic()) {
                registers.push("", `
                    template<>
                    struct is_usertype<::${ fqn }> : std::true_type { };

                    template <>
                    struct usertype_info<::${ fqn }> {
                        static int metatable;
                        static const void* signature;
                        static std::set<const void*> derives;
                        static const struct luaL_Reg* methods;
                        static const struct luaL_Reg* meta_methods;
                        static const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> getters;
                        static const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> setters;
                    };
                `.replace(/^ {20}/mg, "").trim());

                contentDecl.push(`
                    int usertype_info<::${ fqn }>::metatable = LUA_REFNIL;
                    const void* usertype_info<::${ fqn }>::signature;
                    std::set<const void*> usertype_info<::${ fqn }>::derives;
                    const struct luaL_Reg* usertype_info<::${ fqn }>::methods = ::methods;
                    const struct luaL_Reg* usertype_info<::${ fqn }>::meta_methods = ::meta_methods;
                    const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> usertype_info<::${ fqn }>::getters(std::move(::getters));
                    const std::map<std::variant<std::string, int>, std::function<int(lua_State*)>> usertype_info<::${ fqn }>::setters(std::move(::setters));
                `.replace(/^ {20}/mg, "").trim());
            } else if (processor.derives.has(fqn)) {
                registers.push("", `
                    template<>
                    struct is_basetype<::${ fqn }> : std::true_type { };

                    template <>
                    struct basetype_info<::${ fqn }> {
                        static int push(lua_State* L);
                    };
                `.replace(/^ {20}/mg, "").trim());

                contentDecl.push(`
                    int basetype_info<::${ fqn }>::push(lua_State* L) {
                        return lua_rawget_create_if_nil(L, { ${ coclass.path.map(part => JSON.stringify(part)).join(", ") } });
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

            const contentCpp = ["#include <registration.hpp>", ""];
            const contentRegisterPrivate = [];
            const contentRegister = [];

            if (coclass.is_enum_class) {
                contentRegister.push(`
                    lua_rawget_create_if_nil(L, { ${ coclass.path.map(part => JSON.stringify(part)).join(", ") } });

                    ${ Array.from(coclass.properties.keys()).map(name => `lua_pushliteral(L, "${ name }"); lua_push(L, ::${ fqn }::${ name }); lua_rawset(L, -3);`).join(`\n${ " ".repeat(20) }`) }

                    lua_pop(L, 1);
                `.replace(/^ {20}/mg, "").trim());

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
                contentRegister.push(`
                    lua_rawget_create_if_nil(L, { ${ coclass.path.map(part => JSON.stringify(part)).join(", ") } });
                    lua_pushfuncs(L, methods);

                    // metatable = {}
                    lua_newtable(L);
                    lua_pushfuncs(L, meta_methods);

                    // setmetatable(module, metatable)
                    lua_setmetatable(L, -2);
                `.replace(/^ {20}/mg, "").trim());
            } else {
                if (coclass.path.length > 1) {
                    contentRegister.push(`lua_rawget_create_if_nil(L, { ${ coclass.path.slice(0, -1).map(part => JSON.stringify(part)).join(", ") } });`);
                }

                contentRegister.push(`lua_register_class<::${ [fqn, ...coclass.parents].join(", ::") }>(L, "${ coclass.name }");`);

                const parents = [...coclass.parents];

                // denormalize parents
                for (const parent of parents) {
                    if (processor.classes.has(parent) && !processor.classes.get(parent).isStatic()) {
                        contentRegister.push(`usertype_info<::${ parent }>::derives.insert(usertype_info<::${ fqn }>::signature);`);
                    }

                    if (processor.bases.has(parent)) {
                        for (const base of processor.bases.get(parent)) {
                            parents.push(base);
                        }
                    }
                }

                contentRegister.push(`lua_pushliteral(L, "${ coclass.name }");`);
                contentRegister.push("lua_rawget(L, -2);");
            }

            LuaGenerator.writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options);

            if (coclass.properties.size !== 0 && coclass.methods.size !== 0) {
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

            contentRegister.push("lua_pop(L, 1);"); // pop metatable

            if (coclass.path.length > 1 && !coclass.isStatic()) {
                contentRegister.push("lua_pop(L, 1);"); // pop parent metatable
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
                    "#include <string>",
                    ...generated_include,

                    "",

                    ...Array.from(processor.typedefs).map(([fqn, cpptype]) => {
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
