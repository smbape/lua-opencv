/* eslint-disable no-magic-numbers */
const version = process.env.npm_package_version || require("../package.json").version;
const fs = require("node:fs");
const sysPath = require("node:path");
const waterfall = require("async/waterfall");
const { convertExpression } = require("node-autoit-binding-utils/src/autoit-expression-converter");

const [VERSION_MAJOR, VERSION_MINOR] = version.split(".");

const knwon_ids = require("./ids");
const FileUtils = require("./FileUtils");
const CoClass = require("./CoClass");
const {
    makeExpansion,
    useNamespaces,
    removeNamespaces,
    getTypeDef
} = require("./alias");
const {
    PTR,
    SIMPLE_ARGTYPE_DEFAULTS
} = require("./constants");

const proto = {
    makeDependent(type, coclass, options) {
        const cpptype = this.getCppType(type, coclass, options);
        this.processType(cpptype, coclass, options);
    },

    processType(cpptype, coclass, options) {
        if (cpptype.startsWith("std::vector<")) {
            this.processType(cpptype.slice("vector<".length, -">".length), coclass, options);
            this.add_vector(cpptype, coclass, options);
        }
    },

    add_vector(cpptype, parent, options) {
        const fqn = getTypeDef(cpptype, options);
        if (this.classes.has(fqn)) {
            return;
        }

        const coclass = this.getCoClass(fqn, options);
        coclass.include = parent;
        coclass.is_vector = true;

        coclass.addMethod([`${ fqn }.new`, cpptype, [`/Call=${ cpptype }`], [], "", ""], options);

        coclass.addMethod([`${ fqn }.new`, cpptype, [`/Call=${ cpptype }`], [
            ["size_t", "size", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.new`, cpptype, [`/Call=${ cpptype }`], [
            [cpptype, "other", "", []],
        ], "", ""], options);
    },
};

const getTernary = (...args) => {
    const [is_optional] = args.slice(-2);
    if (!is_optional) {
        args = args.slice(0, -2);
    }

    const text = [];

    for (let i = 0; i < args.length; i += 2) {
        const has_more = i + 2 < args.length;

        if (has_more) {
            text.push(args[i], "?");
        }

        text.push(args[i + 1]);

        if (has_more) {
            text.push(`:\n${ " ".repeat(3) }`);
        }
    }

    return text.join(" ");
};

const returnTuple_ = (expr, cpptype) => {
    if (!cpptype.startsWith("std::tuple<")) {
        return expr;
    }

    const types = CoClass.getTupleTypes(cpptype.slice("std::tuple<".length, -">".length));
    return `[&]{
        ${ returnTuple(expr, types).split("\n").join(`\n${ " ".repeat(8) }`) }
    }()`.replace(/^ {4}/mg, "").trim();
};

const returnTuple = (expr, types) => {
    return `
        sol::table res = sol::state_view(ts).create_table();
        auto t = ${ expr };
        ${ types.map((cpptype, i) => `res[${ i + 1 }] = ${
            returnTuple_(`std::get<${ i }>(t)`, cpptype).split("\n").join(`\n${ " ".repeat(8) }`)
        };`).join(`\n${ " ".repeat(8) }`) }
        return res;
    `.replace(/^ {8}/mg, "").trim();
};

const variant_types = new Set();

class LuaGenerator {
    static proto = proto;

    static getRegisterFn(coclass) {
        return `register_${ coclass.getClassName() }`;
    }

    static getMetaFunctionName(fname) {
        const meta_functions = new Map([
            ["sol::meta_function::call", "__call"],
            ["sol::meta_function::call_function", "__call"],
            ["sol::meta_function::addition", "__add"],
            ["sol::meta_function::subtraction", "__sub"],
            ["sol::meta_function::multiplication", "__mul"],
            ["sol::meta_function::division", "__div"],
            ["sol::meta_function::bitwise_not", "__bnot"],
            ["sol::meta_function::bitwise_and", "__band"],
            ["sol::meta_function::bitwise_or", "__bor"],
            ["sol::meta_function::bitwise_xor", "__bxor"],
            ["sol::meta_function::equal_to", "__eq"],
            ["sol::meta_function::less_than", "__lt"],
            ["sol::meta_function::less_than_or_equal_to", "__le"],
        ]);
        return meta_functions.has(fname) ? meta_functions.get(fname) : fname;
    }

    static getLuaFn(fname) {
        const meta_functions = new Map([
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
        return meta_functions.has(fname) ? meta_functions.get(fname) : fname;
    }

    static getBinaryOperator(fname) {
        const meta_functions = new Map([
            ["sol::meta_function::addition", "+"],
            ["sol::meta_function::subtraction", "-"],
            ["sol::meta_function::multiplication", "*"],
            ["sol::meta_function::division", "/"],
            ["sol::meta_function::bitwise_and", "&"],
            ["sol::meta_function::bitwise_or", "|"],
            ["sol::meta_function::bitwise_xor", "^"],
        ]);
        return meta_functions.get(fname);
    }

    static returnVariant(expr, cpptype, options) {
        if (!options.variantTypeReg || !options.variantTypeReg.test(cpptype)) {
            if (cpptype.startsWith("std::tuple<")) {
                return returnTuple_(expr, cpptype);
            }

            if (!cpptype.includes("std::tuple<")) {
                return expr;
            }
        }

        const signature = `auto return_as_impl(${ cpptype }& obj, sol::state_view& lua)`;
        if (!options.implemented || !options.implemented.test(signature, options)) {
            if (!variant_types.has(cpptype)) {
                variant_types.add(cpptype);
                console.log(`variant type not implemented : "${ signature }"`);
            }
        } else {
            expr = `return_as_impl(${ expr }, lua)`;
        }

        return expr;
    }

    static writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const dynamic_get = [];
        const dynamic_set = [];

        const dyn_type_t = coclass.isStatic() ? "sol::table&" : `${ fqn }*`;
        const dyn_type = coclass.isStatic() ? "sol::table" : `${ fqn }*`;
        const dyn_ns = coclass.isStatic() ? "ns" : "self";

        for (const [name, property] of coclass.properties.entries()) {
            const {type, modifiers} = property;
            const cpptype = processor.getCppType(type, coclass, options);
            const is_static = modifiers.includes("/S") || coclass.isStatic();
            const obj = `${ is_static ? `${ fqn }::` : "self->" }`;
            const enum_fqn = processor.getEnumType(type, coclass, options);
            const is_enum_class = enum_fqn && processor.classes.has(enum_fqn) && processor.classes.get(enum_fqn).is_enum_class;
            const as_type = !enum_fqn ? cpptype : is_enum_class ? enum_fqn : "int";
            const is_class = processor.classes.has(cpptype) && !processor.enums.has(cpptype);

            let propname = name;
            let getter, setter;
            let has_propget = is_static || modifiers.includes("/Enum") || modifiers.includes("/R") || modifiers.includes("/RW");
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
            let r_tuple_types = false;

            if (has_propget) {
                let has_expr = false;

                if (cpptype.startsWith("std::tuple<")) {
                    r_tuple_types = CoClass.getTupleTypes(cpptype.slice("std::tuple<".length, -">".length));
                }

                rexpr = `${ obj }${ getter ? `${ getter }()` : propname }`;

                if (!getter && enum_fqn && !is_enum_class) {
                    rexpr = `static_cast<${ as_type }>(${ rexpr })`;
                }

                for (const modifier of modifiers) {
                    if (modifier.startsWith("/RExpr=")) {
                        rexpr = makeExpansion(modifier.slice("/RExpr=".length), rexpr);
                        has_expr = true;
                    }
                }

                if (rexpr.endsWith("::this")) {
                    rexpr = `module[${ rexpr.split("::").map(part => JSON.stringify(part)).join("][") }]`;
                } else {
                    for (const modifier of modifiers) {
                        if (modifier.startsWith("/Cast=")) {
                            rexpr = `${ modifier.slice("/Cast=".length) }(${ rexpr })`;
                            has_expr = true;
                        }
                    }

                    if (!has_expr) {
                        rexpr = LuaGenerator.returnVariant(rexpr, cpptype, options);
                    }
                }
            }

            let in_val;

            if (has_propput) {
                if (setter) {
                    setter = `${ obj }${ setter }`;
                }

                in_val = "*maybe_value";

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
                    auto maybe_value = maybe<${ as_type }>(value);
                    if (maybe_value) {
                        ${ in_val.split("\n").join(`\n${ " ".repeat(24) }`) };
                    } else {
                        luaL_error(lua.lua_state(), "Unexpected value type");
                    }
                `.replace(/^ {20}/mg, "").trim();
            }

            const key_name = is_key ? name : `"${ name }"`;

            if (is_key || coclass.isStatic()) {
                if (r_tuple_types) {
                    dynamic_get.push(`
                        { ${ key_name }, [] (${ dyn_type_t } ${ dyn_ns }, sol::this_state& ts) {
                            ${ returnTuple(rexpr, r_tuple_types).split("\n").join(`\n${ " ".repeat(28) }`) }
                        }}
                    `.replace(/^ {24}/mg, "").trim());
                } else {
                    dynamic_get.push(`
                        { ${ key_name }, [] (${ dyn_type_t } ${ dyn_ns }, sol::this_state& ts) {
                            ${ /\blua\b/.test(rexpr) ? "sol::state_view lua(ts);" : "" }
                            return sol::object(ts, sol::in_place, ${ rexpr });
                        }}
                    `.replace(/^ {24}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));
                }

                if (wexpr) {
                    dynamic_set.push(`
                        { ${ key_name }, [] (${ dyn_type_t } ${ dyn_ns }, sol::stack_object& value, sol::this_state& ts) {
                            sol::state_view lua(ts);
                            ${ wexpr.split("\n").join(`\n${ " ".repeat(28) }`) }
                        }}
                    `.replace(/^ {24}/mg, "").trim());
                }
            } else if (!r_tuple_types && !is_class && rexpr === `self->${ propname }` && in_val === `self->${ propname } = *maybe_value` && /^[.\w]+$/.test(propname)) {
                contentRegister.push(`exports[${ key_name }] = &::${ fqn }::${ propname };`);
            } else {
                const args = [];

                if (!is_static) {
                    args.push(`::${ fqn }* self`);
                }

                args.push("sol::this_state ts");

                const getter_decl = `[] (${ args.join(", ") })`;
                const getter_setter = [];

                if (r_tuple_types) {
                    getter_setter.push(`
                        ${ getter_decl } {
                            ${ returnTuple(rexpr, r_tuple_types).split("\n").join(`\n${ " ".repeat(28) }`) }
                        }
                    `.replace(/^ {24}/mg, "").trim());
                } else {
                    getter_setter.push(`
                        ${ getter_decl } {
                            ${ /\blua\b/.test(rexpr) ? "sol::state_view lua(ts);" : "" }
                            return ${ rexpr };
                        }
                    `.replace(/^ {24}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));
                }

                if (wexpr) {
                    const setter_args = [];

                    if (!is_static) {
                        setter_args.push(`::${ fqn }* self`);
                    }

                    setter_args.push(...[
                        "sol::stack_object value",
                        "sol::this_state ts",
                    ]);

                    getter_setter.push(`
                        [] (${ setter_args.join(", ") }) {
                            sol::state_view lua(ts);
                            ${ wexpr.split("\n").join(`\n${ " ".repeat(28) }`) }
                        }
                    `.replace(/^ {24}/mg, "").trim());
                }

                contentRegister.push(`
                    exports[${ key_name }] = sol::property(${ getter_setter.join(", ").split("\n").join(`\n${ " ".repeat(20) }`) });
                `.replace(/^ {20}/mg, "").trim());
                contentRegister.push("");
            }


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

            processor.docs.push([
                "```cpp",
                cppsignature.join(" "),
                // "",
                "lua:",
                `${ " ".repeat(4) }[${ attributes.join(", ") }] ${ is_static ? "" : "o" }${ coclass.name }.${ name }`,
                "```",
                ""
            ].join("\n").replace(/\s*\( {2}\)/g, "()"));
        }

        if (dynamic_get.length !== 0 || coclass.isStatic()) {
            const not_found = coclass.isStatic() ? "return ns[sol::metatable_key][key];" : `
                sol::state_view lua(ts);

                if (maybe_string_key) {
                    const std::string& k = *maybe_string_key;
                    luaL_error(lua.lua_state(), "attribute %s is not defined", k);
                } else if (maybe_int_key) {
                    const int& k = *maybe_int_key;
                    luaL_error(lua.lua_state(), "attribute %i is not defined", k);
                } else {
                    luaL_error(lua.lua_state(), "attribute is not defined");
                }

                return sol::lua_nil;
            `.replace(/^ {16}/mg, "").trim();

            // https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_dynamic_get_set.cpp
            contentRegisterPrivate.push("", `
                std::map<std::variant<std::string, int>, std::function<sol::object(${ dyn_type_t }, sol::this_state&)>> getters({
                    ${ dynamic_get.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                // we use stack_object for the arguments because we
                // know the values from Lua will remain on Lua's stack,
                // so long we we don't mess with it
                sol::object dynamic_get(${ dyn_type } ns, sol::stack_object key, sol::this_state ts) {
                    auto maybe_string_key = key.as<sol::optional<std::string>>();
                    if (maybe_string_key) {
                        const std::string& k = *maybe_string_key;
                        if (getters.count(k)) {
                            return getters[k](ns, ts);
                        }
                    }

                    auto maybe_int_key = key.as<sol::optional<int>>();
                    if (maybe_int_key) {
                        const int& k = *maybe_int_key;
                        if (getters.count(k)) {
                            return getters[k](ns, ts);
                        }
                    }

                    ${ not_found.split("\n").join(`\n${ " ".repeat(20) }`) }
                }
            `.replace(/^ {16}/mg, "").trim());
        }

        if (dynamic_set.length !== 0 || coclass.isStatic()) {
            const not_found = coclass.isStatic() ? "ns[sol::metatable_key][key] = value;" : `
                sol::state_view lua(ts);

                if (maybe_string_key) {
                    const std::string& k = *maybe_string_key;
                    luaL_error(lua.lua_state(), "attribute %s is not defined", k);
                } else if (maybe_int_key) {
                    const int& k = *maybe_int_key;
                    luaL_error(lua.lua_state(), "attribute %i is not defined", k);
                } else {
                    luaL_error(lua.lua_state(), "attribute is not defined");
                }
            `.replace(/^ {16}/mg, "").trim();

            // https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_dynamic_get_set.cpp
            contentRegisterPrivate.push("", `
                std::map<std::variant<std::string, int>, std::function<void(${ dyn_type_t }, sol::stack_object&, sol::this_state&)>> setters({
                    ${ dynamic_set.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                // we use stack_object for the arguments because we
                // know the values from Lua will remain on Lua's stack,
                // so long we we don't mess with it
                void dynamic_set(${ dyn_type } ns, sol::stack_object key, sol::stack_object value, sol::this_state ts) {
                    auto maybe_string_key = key.as<sol::optional<std::string>>();
                    if (maybe_string_key) {
                        const std::string& k = *maybe_string_key;
                        if (setters.count(k)) {
                            setters[k](ns, value, ts);
                            return;
                        }
                    }

                    auto maybe_int_key = key.as<sol::optional<int>>();
                    if (maybe_int_key) {
                        const int& k = *maybe_int_key;
                        if (setters.count(k)) {
                            setters[k](ns, value, ts);
                            return;
                        }
                    }

                    ${ not_found.split("\n").join(`\n${ " ".repeat(20) }`) }
                }
            `.replace(/^ {16}/mg, "").trim());
        }

        return [dynamic_get, dynamic_set];
    }

    static writeMethods(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const { shared_ptr } = options;
        const indent = " ".repeat(4);

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
            let has_static = false;
            let has_instance = false;
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

                const callargs = [];
                const retval = [];
                const overload = [];
                const precondition = [`argc + kwargs.size() > ${ argc }`];

                if (!coclass.isStatic() && !func_modifiers.includes("/S")) {
                    has_instance = true;
                    precondition.push("!self");
                } else {
                    has_static = true;
                }

                overload.push(`
                    if (${ precondition.join(" || ") }) {
                        // too many parameters
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
                    const is_optional = defval !== "" || is_out_arg && !is_in_arg;

                    let is_array = false;
                    let arrtype = "";
                    let arg_suffix = "_impl";

                    if (argtype === "InputArray") {
                        is_array = true;
                        arg_suffix = "_array";
                        arrtype = "InputArray";
                        argtype = "Mat";
                    } else if (argtype === "InputOutputArray") {
                        is_array = true;
                        arg_suffix = "_array";
                        arrtype = "InputOutputArray";
                        argtype = "Mat";
                    } else if (argtype === "OutputArray") {
                        is_array = true;
                        arg_suffix = "_array";
                        arrtype = "OutputArray";
                        argtype = "Mat";
                    } else if (argtype === "InputArrayOfArrays") {
                        is_array = true;
                        arg_suffix = "_arrays";
                        arrtype = "InputArray";
                        argtype = "std::vector<cv::Mat>";
                    } else if (argtype === "InputOutputArrayOfArrays") {
                        is_array = true;
                        arg_suffix = "_arrays";
                        arrtype = "InputOutputArray";
                        argtype = "std::vector<cv::Mat>";
                    } else if (argtype === "OutputArrayOfArrays") {
                        is_array = true;
                        arg_suffix = "_arrays";
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
                    const maybe = `maybe${ arg_suffix }`;
                    const optional_t = is_array ? argtype.startsWith("std::vector") ? "OptionalArrays" : "OptionalArray" : "std::shared_ptr";
                    const optional_name = `maybe_${ argname }_opt`;

                    if (is_out_arg) {
                        if (is_array) {
                            retval.push([j, getTernary(...[
                                `${ argname }_is_nil`,
                                `sol::object(ts, sol::in_place, ${ argname }_default)`,
                                `${ argname }_positional`,
                                `vargs.get<sol::object>(${ i })`,
                                `${ argname }_kwarg`,
                                `kwargs.at("${ argname }")`,
                                is_optional,
                                `sol::object(ts, sol::in_place, ${ argname }_default)`,
                            ]), "sol::object"]);
                        } else if (is_by_ref) {
                            retval.push([j, getTernary(...[
                                `${ argname }_positional || ${ argname }_kwarg`,
                                is_optional ? `sol::object(ts, sol::in_place, ${ optional_name })` : `{ ts, sol::in_place, ${ optional_name } }`,
                                is_optional,
                                `sol::object(ts, sol::in_place, ${ argname }_default)`,
                            ]), "sol::object"]);
                        } else {
                            retval.push([j, argname, cpptype]);
                        }
                    }

                    overload.push("", `
                        // =========================
                        // get argument ${ argname }
                        // =========================
                        ${ optional_t }<${ var_type }> ${ optional_name };
                        bool ${ argname }_positional = false;
                        bool ${ argname }_kwarg = false;
                        if (argc > ${ i }) {
                            // positional parameter
                            ${ optional_name } = std::move(${ maybe }(vargs.get<sol::object>(${ i }), static_cast<${ var_type }*>(nullptr)));
                            if (!${ optional_name }) {
                                goto overload${ overload_id };
                            }

                            // should not be a named parameter
                            if (maybe_kwargs && kwargs.count("${ argname }")) {
                                goto overload${ overload_id };
                            }

                            ${ argname }_positional = true;
                        }
                        else if (kwargs.count("${ argname }")) {
                            // named parameter
                            ${ optional_name } = std::move(${ maybe }(kwargs.at("${ argname }"), static_cast<${ var_type }*>(nullptr)));
                            if (!${ optional_name }) {
                                goto overload${ overload_id };
                            }
                            usedkw++;
                            ${ argname }_kwarg = true;
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

                    if (is_optional || is_array && is_out_arg) {
                        const ref = defval !== "" && is_by_ref && !defval.includes("(") && !defval.includes("'") && !defval.includes("\"") && !/^(?:\.|\d+\.)\d+$/.test(defval) ? "&" : "";
                        overload.push(`${ ref || is_out_arg ? "" : "static " }${ cpptype }${ ref } ${ argname }_default${ defval !== "" ? ` = ${ defval }` : "" };`);
                    }

                    if (is_array) {
                        const ternary = [];

                        if (is_out_arg) {
                            overload.push(`
                                bool ${ argname }_is_nil = ${ argname }_positional && vargs.get<sol::object>(${ i }) == sol::lua_nil ||
                                    ${ argname }_kwarg && kwargs.at("${ argname }") == sol::lua_nil;
                            `.replace(/^ {32}/mg, "").trim());

                            ternary.push(...[
                                `${ argname }_is_nil`,
                                `std::move(${ maybe }(std::make_shared<${ arr_cpptype }>(${ argname }_default)))`,
                            ]);
                        }

                        ternary.push(...[
                            optional_name,
                            optional_name,
                            is_optional,
                            `std::move(${ maybe }(std::make_shared<${ arr_cpptype }>(${ argname }_default)))`,
                        ]);

                        overload.push(`
                            auto ${ argname }_${ arrtype } = std::move(${ getTernary(...ternary) });
                            auto& ${ argname } = *${ argname }_${ arrtype };
                        `.replace(/^ {28}/mg, "").trim());
                    } else if (is_shared_ptr) {
                        overload.push(`
                            auto ${ argname } = ${ getTernary(
                                    optional_name,
                                    `reference_internal(*${ optional_name }, static_cast<${ cpptype }*>(nullptr))`,
                                    is_optional,
                                    `${ argname }_default`
                                ) };
                        `.replace(/^ {28}/mg, "").trim());
                    } else {
                        overload.push(`
                            auto& ${ argname } = ${ getTernary(
                                    optional_name,
                                    `*${ optional_name }`,
                                    is_optional,
                                    `${ argname }_default`
                                ) };
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
                    if (usedkw != kwargs.size()) {
                        goto overload${ overload_id };
                    }
                `.replace(/^ {20}/mg, "").trim());

                let callee;
                const path = name.split(is_constructor ? "::" : ".");
                const is_static = func_modifiers.includes("/S") || coclass.isStatic();
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
                    const return_type = processor.getCppType(return_value_type ? return_value_type : fqn, coclass, options);
                    retval.push([-1, callee, return_type]);
                } else {
                    overload.push("", `${ callee };`);
                }

                if (retval.length !== 0) {
                    retval.sort(([a], [b]) => a - b);

                    overload.push("", `
                        sol::variadic_results vres;
                        ${ retval
                            .map(([, result, cpptype]) => {
                                if (cpptype === "sol::object") {
                                    return `vres.push_back(${ result });`;
                                }

                                const is_shared_ptr = cpptype.startsWith(`${ shared_ptr }<`);

                                if (is_shared_ptr) {
                                    cpptype = cpptype.slice(`${ shared_ptr }<`.length, -">".length);
                                }

                                const is_class = processor.classes.has(cpptype) && !processor.enums.has(cpptype);

                                if (is_class && !is_constructor && !result.startsWith("reference_internal")) {
                                    // shared_ptr other than std::shared_ptr is not yet supported
                                    if (is_shared_ptr && !"std::shared_ptr" !== shared_ptr) {
                                        result = `std::shared_ptr<${ cpptype }>(${ result })`;
                                    } else {
                                        result = `std::make_shared<${ cpptype }>(std::move(${ result }))`;
                                    }
                                } else if (cpptype.endsWith("*")) {
                                    result = `reference_internal(${ result })`;
                                } else if (fname !== "new") {
                                    result = LuaGenerator.returnVariant(result, cpptype, options);
                                }

                                return `vres.push_back({ ts, sol::in_place, ${ result } });`;
                            })
                            .join(`\n${ " ".repeat(24) }`) }
                        return vres;
                    `.replace(/^ {24}/mg, "").trim());
                } else if (!has_body) {
                    // body is responsible of the return
                    overload.push("", "return sol::variadic_results();");
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

            const is_by_ref = is_constructor || !has_static || !has_instance ? "" : "&";
            const lua_args = [`sol::this_state${ is_by_ref } ts`, `sol::variadic_args${ is_by_ref } vargs`];
            if (!is_constructor && has_instance) {
                lua_args.unshift(`::${ fqn }* self`);
            }

            const lua_fname = `Lua_${ ename.replaceAll("::", "_") }`;

            let start = 0;
            while (contentFunction[start] === "" && start + 1 < contentFunction.length) {
                start++;
            }

            contentRegisterPrivate.push(`
                auto ${ lua_fname }(${ lua_args.join(", ") }) {
                    sol::state_view lua(ts);

                    if (is_call_garbage_collect()) {
                        lua.collect_garbage();
                    }

                    auto vargc = vargs.size();
                    auto maybe_kwargs = vargc == 0 ? empty_kwargs_ptr : maybe<NamedParameters>(vargs.get<sol::object>(vargc - 1));
                    auto& kwargs = maybe_kwargs ? *maybe_kwargs : empty_kwargs;
                    const int argc = maybe_kwargs ? vargc - 1 : vargc;

                    ${ contentFunction.slice(start).join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }

                    luaL_error(lua.lua_state(), "Overload resolution failed");
                    // LUA_MODULE_THROW("Overload resolution failed");
                    return sol::variadic_results();
                }
            `.replace(/^ {16}/mg, "").trim());

            // new line
            contentRegisterPrivate.push("");

            if (is_constructor) {
                contentRegister.push(`// ${ fqn }.new(...) -- dot syntax, no "self" value`);
                contentRegister.push(`exports[sol::meta_function::construct] = sol::factories(::${ lua_fname });`, "");
                contentRegister.push(`// ${ fqn }(...) syntax, only`);
                contentRegister.push(`exports[sol::call_constructor] = sol::factories(::${ lua_fname });`, "");
            } else {
                const definitions = [];

                if (has_instance && has_static) {
                    definitions.push(`[] (::${ fqn }* self, sol::this_state ts, sol::variadic_args vargs) {
                        return ::${ lua_fname }(self, ts, vargs);
                    }`.replace(/^ {20}/mg, "").trim());

                    definitions.push(`[] (sol::this_state ts, sol::variadic_args vargs) {
                        return ::${ lua_fname }(static_cast<::${ fqn }*>(nullptr), ts, vargs);
                    }`.replace(/^ {20}/mg, "").trim());
                } else if (coclass.isStatic() && (ename === "sol::meta_function::call" || ename === "sol::meta_function::call_function")) {
                    definitions.push(`[] (sol::stack_object exports, sol::this_state ts, sol::variadic_args vargs) {
                        return ::${ lua_fname }(ts, vargs);
                    }`.replace(/^ {20}/mg, "").trim());
                } else {
                    const by_ref = coclass.isStatic() ? "" : "&";
                    definitions.push(`${ by_ref }::${ lua_fname }`);
                }

                const overload = definitions.length === 1 ? definitions[0] : `sol::overload(${ definitions.join(", ") })`;

                if (ename.startsWith("sol::meta_function::")) {
                    contentRegister.push(`exports[${ ename }] = ${ overload };`);
                } else {
                    contentRegister.push(`exports.set_function("${ ename }", ${ overload });`);
                    if (coclass.is_vector && fname === "new") {
                        contentRegister.push(`exports[sol::meta_function::call] = [] (sol::stack_object exports, sol::this_state ts, sol::variadic_args vargs) {
                            return ::${ lua_fname }(ts, vargs);
                        };`.replace(/^ {24}/mg, "").trim());
                    }
                }
            }
        }
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
        const mname = LuaGenerator.getMetaFunctionName(ename);
        const {fqn} = coclass;
        const [name, return_value_type, func_modifiers, list_of_arguments] = decl;
        const argc = list_of_arguments.length;
        const is_static = func_modifiers.includes("/S") || coclass.isStatic();

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
            outstr = `<${ fqn.replaceAll("::", ".") } object>`;
        } else if (outlist.length !== 0) {
            outstr = outlist.join(", ");
        } else {
            outstr = "None";
        }

        const caller = is_static ? fqn.replaceAll("::", ".") : `o${ coclass.name }`;
        const is_call_fn = fname === "sol::meta_function::call" || fname === "sol::meta_function::call_function" || fname === "operator()";

        let description = is_call_fn ?
            `${ caller }( ${ argstr } ) -> ${ outstr }` :
            `${ caller }${ is_static ? "." : ":" }${ is_constructor ? "new" : mname }( ${ argstr } ) -> ${ outstr }`;

        if (is_constructor || coclass.is_vector && fname === "new") {
            description += `\n    ${ caller }( ${ argstr } ) -> ${ outstr }`;
        }

        const op = LuaGenerator.getBinaryOperator(ename);
        if (op) {
            const args = argnamelist.slice(0, firstoptarg);
            if (!is_static) {
                args.unshift("self");
            }

            const argstr = args.join(` ${ op } `);
            description += `\n    ${ argstr } -> ${ outstr }`;
        }

        let cppsignature = `${ processor.getCppType(return_value_type, coclass, options) } ${ name.replaceAll(".", "::") }`;

        if (is_static && !coclass.isStatic()) {
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
            registrations.push(`${ registerFn }(lua, module);`);

            const registers = [`void ${ registerFn }(sol::state_view& lua, sol::table& module);`];

            if (!coclass.isStatic()) {
                registers.push("", `
                    template <>
                    struct is_usertype<::${ fqn }> : std::true_type { };
                `.replace(/^ {20}/mg, "").trim());
            }

            files.set(sysPath.join(options.output, fileHdr), `
                #pragma once
                #include <lua_generated_pch.hpp>

                namespace LUA_MODULE_NAME {
                    ${ registers.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                }
            `.replace(/^ {16}/mg, "").trim());

            const contentCpp = [`#include <${ fileHdr }>\n`];
            const contentRegisterPrivate = [];
            const contentRegister = [];

            let ns = "module";
            for (let i = 0; i < coclass.path.length - 1; i++) {
                if (i === 0) {
                    ns = `${ ns }[sol::create_if_nil]`;
                }
                ns = `${ ns }["${ coclass.path[i] }"]`;
            }

            if (coclass.path.length > 1) {
                const parent = processor.classes.get(coclass.path.slice(0, -1).join("::"));

                if (!parent || !parent.isStatic()) {
                    contentRegister.push(`sol::table ns = ${ ns };`);
                } else {
                    contentRegister.push(`sol::table ns = ${ ns }[sol::metatable_key];`);
                }

                ns = "ns";
            }

            if (coclass.is_enum_class) {
                contentRegister.push(`
                    ${ ns }.new_enum<${ fqn }>("${ coclass.name }", {
                        ${ Array.from(coclass.properties.keys()).map(name => `{ "${ name }", ${ fqn }::${ name } }`).join(`,\n${ " ".repeat(24) }`) }
                    });
                `.replace(/^ {20}/mg, "").trim());

                contentCpp.push(`
                    namespace LUA_MODULE_NAME {
                        void ${ registerFn }(sol::state_view& lua, sol::table& module) {
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
                    sol::table exports = lua.create_table();
                    sol::table proxy = lua.create_table();
                    proxy[sol::metatable_key] = exports;
                    ${ ns }["${ coclass.name }"] = proxy;
                `.replace(/^ {20}/mg, "").trim());
            } else {
                contentRegister.push(`sol::usertype<::${ fqn }> exports = ${ ns }.new_usertype<::${ fqn }>("${ coclass.name }");`);
                if (coclass.parents.size !== 0) {
                    contentRegister.push(`\nexports.set(sol::base_classes, sol::base_list<::${ Array.from(coclass.parents).join(", ::") }>());`);
                }
            }

            // new line
            contentRegister.push("");
            contentRegister.push(...namespaces);
            contentRegister.push("");

            const [dynamic_get, dynamic_set] = LuaGenerator.writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options);

            if (dynamic_get.length !== 0 || coclass.isStatic()) {
                contentRegister.push("exports[sol::meta_function::index] = &dynamic_get;");
            }

            if (dynamic_set.length !== 0 || coclass.isStatic()) {
                contentRegister.push("exports[sol::meta_function::new_index] = &dynamic_set;");
            }

            if (coclass.properties.size !== 0 && coclass.methods.size !== 0) {
                // new line
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

            contentCpp.push(`
                namespace LUA_MODULE_NAME {
                    void ${ registerFn }(sol::state_view& lua, sol::table& module) {
                        ${ contentRegister.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                    }
                }
            `.replace(/^ {16}/mg, "").trim());

            files.set(sysPath.join(options.output, fileCpp), contentCpp.join("\n").replace(/^[^\S\r\n]+$/mg, ""));

            // TODO : add method signature
            //        variantTypeReg => table

            processor.docs.splice(docid, 0, `## ${ fqn.replaceAll("_", "\\_") }\n`);
        }

        files.set(sysPath.join(options.output, "register_all.hpp"), `
            #pragma once
            ${ registrationsHdr.join(`\n${ " ".repeat(12) }`) }

            namespace LUA_MODULE_NAME {
                void register_all(sol::state_view& lua, sol::table& module);
            }
        `.replace(/^ {12}/mg, "").trim());

        files.set(sysPath.join(options.output, "register_all.cpp"), `
            #include <register_all.hpp>

            namespace LUA_MODULE_NAME {
                void register_all(sol::state_view& lua, sol::table& module) {
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
                ].join("\n").trim().replace(/[^\S\n]+$/mg, "")
            );

            files.set(
                sysPath.join(options.output, "lua_generated_pch.hpp"),
                [
                    "#pragma once\n",
                    "#include <lua_generated_include.hpp>",
                    "#include <registration.hpp>",
                    "#include <register_all.hpp>",
                ].join("\n").trim().replace(/[^\S\n]+$/mg, "")
            );
        }

        // registration.cpp
        // call all registration methods

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
