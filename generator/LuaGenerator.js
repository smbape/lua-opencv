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
const {makeExpansion, useNamespaces, removeNamespaces} = require("./alias");
const {
    PTR,
    SIMPLE_ARGTYPE_DEFAULTS
} = require("./constants");

const proto = {
    // No extension
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
            text.push(":");
        }
    }

    return text.join(" ");
};

class LuaGenerator {
    static proto = proto;

    static getRegisterFn(coclass) {
        return `register_${ coclass.getClassName() }`;
    }

    static writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const dynamic_set = [];
        const dynamic_get = [];

        for (const [name, property] of coclass.properties.entries()) {
            const {type, modifiers} = property;
            const cpptype = processor.getCppType(type, coclass, options);
            const is_static = modifiers.includes("/S") || coclass.isStatic();
            const obj = `${ is_static ? `${ fqn }::` : "self->" }`;
            const enum_fqn = processor.getEnumType(type, coclass, options);
            const is_class = !enum_fqn || processor.classes.has(enum_fqn) && processor.classes.get(enum_fqn).is_enum_class;
            const as_type = is_class ? enum_fqn || cpptype : "int";

            let propname = name;
            let getter, setter;
            let has_propget = is_static || modifiers.includes("/Enum") || modifiers.includes("/R") || modifiers.includes("/RW");
            let has_propput = modifiers.includes("/W") || modifiers.includes("/RW");

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
                }
            }

            if (!has_propput) {
                has_propget = true;
            }

            let rexpr = null;
            let wexpr = null;
            let r_tuple_types = false;

            if (has_propget) {
                const shared_ptr = removeNamespaces(options.shared_ptr, options);
                const is_ptr = type.endsWith("*");
                const is_shared_ptr = cpptype.startsWith(`${ shared_ptr }<`);
                const is_std = cpptype.startsWith("std::");
                const is_by_ref = !is_ptr && !is_shared_ptr && (is_std || processor.classes.has(cpptype) && !processor.enums.has(cpptype));
                let has_expr = false;

                if (cpptype.startsWith("std::tuple<")) {
                    r_tuple_types = CoClass.getTupleTypes(cpptype.slice("std::tuple<".length, -">".length));
                }

                rexpr = `${ obj }${ getter ? `${ getter }()` : propname }`;

                if (!getter && enum_fqn && !is_class) {
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
                        }
                    }

                    if (is_by_ref && !has_expr) {
                        const is_const = modifiers.includes("/C");
                        rexpr = `std::${ is_const ? "c" : "" }ref(${ rexpr })`;
                    }
                }
            }

            let in_val;

            if (has_propput) {
                if (setter) {
                    setter = `${ obj }${ setter }`;
                }

                in_val = `object_as<${ as_type }>(value)`;

                if (!is_class) {
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
                    if (object_is<${ as_type }>(value)) {
                        ${ in_val.split("\n").join(`\n${ " ".repeat(24) }`) };
                    } else {
                        sol::state_view lua(ts);
                        luaL_error(lua.lua_state(), "Unexpected value type");
                    }
                `.replace(/^ {20}/mg, "").trim();
            }

            if (coclass.isStatic()) {
                if (r_tuple_types) {
                    dynamic_get.push(`
                        { "${ name }", [] (sol::this_state& ts) {
                            sol::state_view lua(ts);
                            sol::table res = lua.create_table();
                            auto t = ${ rexpr };
                            ${ r_tuple_types.map((_, i) => `res[${ i + 1 }] = std::get<${ i }>(t);`).join(`\n${ " ".repeat(28) }`) }
                            return res;
                        }}
                    `.replace(/^ {24}/mg, "").trim());
                } else {
                    dynamic_get.push(`
                        { "${ name }", [] (sol::this_state& ts) {
                            return sol::object(ts, sol::in_place, ${ rexpr });
                        }}
                    `.replace(/^ {24}/mg, "").trim());
                }

                if (wexpr) {
                    dynamic_set.push(`
                        { "${ name }", [] (sol::stack_object& value, sol::this_state& ts) {
                            ${ wexpr.split("\n").join(`\n${ " ".repeat(28) }`) }
                        }}
                    `.replace(/^ {24}/mg, "").trim());
                }
            } else if (!r_tuple_types && rexpr === `self->${ propname }` && in_val === `self->${ propname } = object_as<${ type }>(value)`) {
                contentRegister.push(`exports["${ name }"] = &::${ fqn }::${ propname };`);
            } else {
                const args = [];

                if (!is_static) {
                    args.push(`::${ fqn }* self`);
                }

                if (r_tuple_types) {
                    args.push("sol::this_state ts");
                }

                const getter_decl = args.length !== 0 ? `[] (${ args.join(", ") })` : "[]";
                const getter_setter = [];

                if (r_tuple_types) {
                    getter_setter.push(`
                        ${ getter_decl } {
                            sol::state_view lua(ts);
                            sol::table res = lua.create_table();
                            auto t = ${ rexpr };
                            ${ r_tuple_types.map((_, i) => `res[${ i + 1 }] = std::get<${ i }>(t);`).join(`\n${ " ".repeat(28) }`) }
                            return res;
                        }
                    `.replace(/^ {24}/mg, "").trim());
                } else {
                    getter_setter.push(`
                        ${ getter_decl } {
                            return ${ rexpr };
                        }
                    `.replace(/^ {24}/mg, "").trim());
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
                            ${ wexpr.split("\n").join(`\n${ " ".repeat(28) }`) }
                        }
                    `.replace(/^ {24}/mg, "").trim());
                }

                contentRegister.push(`
                    exports["${ name }"] = sol::property(${ getter_setter.join(", ").split("\n").join(`\n${ " ".repeat(20) }`) });
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
                `${ " ".repeat(4) }[${ attributes.join(", ") }] o${ coclass.name }.${ name }`,
                "```",
                ""
            ].join("\n").replace(/\s*\( {2}\)/g, "()"));
        }

        if (coclass.isStatic()) {
            // https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_dynamic_get_set.cpp
            contentRegisterPrivate.push(`
                std::map<std::string, std::function<sol::object(sol::this_state&)>> getters({
                    ${ dynamic_get.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                sol::object dynamic_get(sol::table ns, sol::stack_object key, sol::this_state ts) {
                    // we use stack_object for the arguments because we
                    // know the values from Lua will remain on Lua's stack,
                    // so long we we don't mess with it
                    auto maybe_string_key = key.as<sol::optional<std::string>>();
                    if (maybe_string_key) {
                        const std::string& k = *maybe_string_key;
                        if (getters.count(k)) {
                            return getters[k](ts);
                        }
                    }

                    // sol::state_view lua(ts);
                    // std::stringstream err;
                    // err << "AttributeError: module '${ fqn.replaceAll("::", ".") }' has no attribute '" << (maybe_string_key ? *maybe_string_key : "") << "'";
                    // luaL_error(lua.lua_state(), err.str().c_str());

                    return ns[sol::metatable_key][key];
                }

                std::map<std::string, std::function<void(sol::stack_object&, sol::this_state&)>> setters({
                    ${ dynamic_set.join(",\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                });

                void dynamic_set(sol::table ns, sol::stack_object key, sol::stack_object value, sol::this_state ts) {
                    // we use stack_object for the arguments because we
                    // know the values from Lua will remain on Lua's stack,
                    // so long we we don't mess with it
                    auto maybe_string_key = key.as<sol::optional<std::string>>();
                    if (maybe_string_key) {
                        const std::string& k = *maybe_string_key;
                        if (setters.count(k)) {
                            setters[k](value, ts);
                            return;
                        }
                    }

                    // sol::state_view lua(ts);
                    // std::stringstream err;
                    // err << "AttributeError: module '${ fqn.replaceAll("::", ".") }' has no attribute '" << (maybe_string_key ? *maybe_string_key : "") << "'";
                    // luaL_error(lua.lua_state(), err.str().c_str());

                    ns[sol::metatable_key][key] = value;
                }
            `.replace(/^ {16}/mg, "").trim());
        }
    }

    static writeMethods(processor, coclass, contentRegisterPrivate, contentRegister, options) {
        const { fqn } = coclass;
        const { shared_ptr } = options;
        const indent = " ".repeat(4);
        const variant_types = new Set();

        for (const fname of Array.from(coclass.methods.keys()).sort((a, b) => {
            if (a === "create") {
                return -1;
            }

            if (b === "create") {
                return 1;
            }

            return a > b ? 1 : a < b ? -1 : 0;
        })) {
            const overloads = coclass.methods.get(fname);
            const contentFunction = [];

            processor.docs.push(`### ${ fqn }::${ fname }\n`.replaceAll("_", "\\_"));

            let is_constructor = false;
            let has_static = false;
            let has_instance = false;
            let overload_id = 0;

            for (const decl of overloads) {
                overload_id++;

                const [name, return_value_type, func_modifiers, list_of_arguments] = decl;
                is_constructor = func_modifiers.includes("/CO");
                const argc = list_of_arguments.length;

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
                    in_args[j] = is_in_array || is_in_out || arg_modifiers.includes("/I");
                    out_args[j] = is_out_array || is_in_out || arg_modifiers.includes("/O") || is_ptr && defval === "" && SIMPLE_ARGTYPE_DEFAULTS.has(argtype.slice(0, -1));
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

                if (!coclass.isStatic() && !func_modifiers.includes("/S")) {
                    has_instance = true;
                    overload.push(`
                        if (!self) {
                            goto overload${ overload_id };
                        }
                    `.replace(/^ {24}/mg, "").trim(), "");
                } else {
                    has_static = true;
                }

                overload.push("int usedkw = 0;");

                for (let i = 0; i < argc; i++) {
                    const j = indexes[i];
                    const [, argname, , arg_modifiers] = list_of_arguments[j];
                    let [argtype, , defval] = list_of_arguments[j];

                    const is_ptr = argtype.endsWith("*");
                    const is_out = out_args[j];
                    const is_optional = defval !== "" || is_out && !in_args[j];

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

                    if (is_out && is_ptr && !PTR.has(argtype)) {
                        callarg = `&${ callarg }`;
                        argtype = argtype.slice(0, -1);
                        defval = SIMPLE_ARGTYPE_DEFAULTS.has(argtype) ? SIMPLE_ARGTYPE_DEFAULTS.get(argtype) : "";
                    } else if (is_out && cpptype.startsWith(`${ shared_ptr }<`)) {
                        callarg = `reference_internal(${ callarg }, static_cast<${ cpptype }*>(nullptr))`;
                        argtype = cpptype.slice(`${ shared_ptr }<`.length, -">".length);
                        defval = SIMPLE_ARGTYPE_DEFAULTS.has(argtype) ? SIMPLE_ARGTYPE_DEFAULTS.get(argtype) : "";
                    } else if (defval === "" && SIMPLE_ARGTYPE_DEFAULTS.has(argtype)) {
                        defval = SIMPLE_ARGTYPE_DEFAULTS.get(argtype);
                    } else if (defval.endsWith("()") && processor.getCppType(defval.slice(0, -"()".length), coclass, options) === cpptype) {
                        defval = "";
                    }

                    cpptype = processor.getCppType(argtype, coclass, options);

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
                    const in_type_is_byref = processor.classes.has(in_type) && !processor.enums.has(in_type);
                    const var_type = is_array ? arr_cpptype : in_type_is_byref ? `std::shared_ptr<${ in_type }>` : in_type;
                    const object_is = `object_is${ arg_suffix }`;
                    const object_as = `object_as${ arg_suffix }`;

                    if (is_out) {
                        if (is_by_ref || is_array) {
                            retval.push([j, getTernary(
                                `${ argname }_positional`,
                                `vargs.get<sol::object>(${ i })`,
                                `${ argname }_kwarg`,
                                `kwargs.at("${ argname }")`,
                                is_optional,
                                `sol::object(ts, sol::in_place, ${ argname }_default)`
                            ), "sol::object"]);
                        } else {
                            retval.push([j, argname, cpptype]);
                        }
                    }

                    overload.push("", `
                        // =========================
                        // get argument ${ argname }
                        // =========================
                        bool ${ argname }_positional = false;
                        bool ${ argname }_kwarg = false;
                        if (!has_kwarg || argc > ${ i }) {
                            ${ argname }_positional = argc >= ${ i };

                            // positional parameter
                            if (${ argname }_positional && !${ object_is }(vargs.get<sol::object>(${ i }), static_cast<${ var_type }*>(nullptr))) {
                                goto overload${ overload_id };
                            }

                            // should not be a named parameter
                            if (has_kwarg && kwargs.count("${ argname }")) {
                                goto overload${ overload_id };
                            }
                        }
                        else if (kwargs.count("${ argname }")) {
                            // named parameter
                            if (!${ object_is }(kwargs.at("${ argname }"), static_cast<${ var_type }*>(nullptr))) {
                                goto overload${ overload_id };
                            }

                            ${ argname }_kwarg = true;
                            usedkw++;
                        }
                    `.replace(/^ {24}/mg, "").trim());

                    if (is_optional) {
                        // goto instruction needs all variables to be declared first
                        // put declaration of variable at top to be able to use goto
                        if (is_by_ref) {
                            overload.push(`auto ${ argname }_default = std::make_shared<${ cpptype }>();`);
                            if (defval !== "") {
                                overload.push(`*${ argname }_default = ${ defval };`);
                            }
                        } else {
                            overload.push(`${ cpptype } ${ argname }_default${ defval !== "" ? ` = ${ defval }` : "" };`);
                        }
                    } else {
                        overload.push(`
                            else {
                                // mandatory parameter
                                goto overload${ overload_id };
                            }
                        `.replace(/^ {28}/mg, "").trim());
                    }

                    const deref = in_type_is_byref ? "*" : "";

                    if (is_array) {
                        overload.push(`
                            auto ${ argname }_${ arrtype } = ${ getTernary(
                                    `${ argname }_positional`,
                                    `${ object_as }(vargs.get<sol::object>(${ i }), static_cast<${ var_type }*>(nullptr))`,
                                    `${ argname }_kwarg`,
                                    `${ object_as }(kwargs.at("${ argname }"), static_cast<${ var_type }*>(nullptr))`,
                                    is_optional,
                                    `${ object_as }(std::make_shared<${ arr_cpptype }>(${ is_by_ref ? "*" : "" }${ argname }_default))`
                                ) };
                            auto& ${ argname } = *${ argname }_${ arrtype };
                        `.replace(/^ {28}/mg, "").trim());
                    } else if (is_shared_ptr) {
                        overload.push(`
                            auto& ${ argname }_ref = ${ getTernary(
                                    `${ argname }_positional`,
                                    `${ deref }${ object_as }(vargs.get<sol::object>(${ i }), static_cast<${ var_type }*>(nullptr))`,
                                    `${ argname }_kwarg`,
                                    `${ deref }${ object_as }(kwargs.at("${ argname }"), static_cast<${ var_type }*>(nullptr))`,
                                    is_optional,
                                    `*${ argname }_default`
                                ) };
                            auto ${ argname } = reference_internal(${ argname }_ref, static_cast<${ cpptype }*>(nullptr));
                        `.replace(/^ {28}/mg, "").trim());
                    } else {
                        overload.push(`
                            auto${ is_by_ref ? "&" : "" } ${ argname } = ${ getTernary(
                                    `${ argname }_positional`,
                                    `${ deref }${ object_as }(vargs.get<sol::object>(${ i }), static_cast<${ var_type }*>(nullptr))`,
                                    `${ argname }_kwarg`,
                                    `${ deref }${ object_as }(kwargs.at("${ argname }"), static_cast<${ var_type }*>(nullptr))`,
                                    is_optional,
                                    `${ is_by_ref ? "*" : "" }${ argname }_default`
                                ) };
                        `.replace(/^ {28}/mg, "").trim());
                    }
                }

                overload.push("", `
                    // =========================
                    // call ${ name.replaceAll(".", "::") }
                    // =========================

                    // too many parameters
                    // unknown named parameters
                    if (argc >= ${ argc } || usedkw != kwargs.size()) {
                        goto overload${ overload_id };
                    }
                `.replace(/^ {20}/mg, "").trim());

                let callee;
                const path = name.split(is_constructor ? "::" : ".");
                const is_static = func_modifiers.includes("/S") || coclass.isStatic();

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
                    if (callargs.length === 1 && /^operator\s*(?:[/*+<>-]=?|\+\+|[!=]=)$/.test(path[path.length - 1])) {
                        const operator = path[path.length - 1].slice("operator".length).trim();
                        callee = `(*${ callee }) ${ operator } `;
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

                callee = `${ callee }(${ expr })`;

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
                                } else if (options.variantTypeReg && options.variantTypeReg.test(cpptype)) {
                                    const signature = `auto as_return_impl(${ cpptype }& obj, sol::state_view& lua)`;
                                    if (!options.implemented || !options.implemented.test(signature, options)) {
                                        if (!variant_types.has(cpptype)) {
                                            variant_types.add(cpptype);
                                            console.log(`variant type not implemented : "${ signature }"`);
                                        }
                                    } else {
                                        result = `as_return_impl(${ result }, lua)`;
                                    }
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
            }

            const is_by_ref = is_constructor ? "" : "&";
            const lua_args = [`sol::this_state${ is_by_ref } ts`, `sol::variadic_args${ is_by_ref } vargs`];
            if (!is_constructor && has_instance) {
                lua_args.unshift(`::${ fqn }* self`);
            }

            const lua_fname = `Lua_${ fname.replaceAll("::", "_") }`;

            contentRegisterPrivate.push(`
                auto ${ lua_fname }(${ lua_args.join(", ") }) {
                    sol::state_view lua(ts);
                    const int argc = vargs.size() - 1;
                    const bool has_kwarg = object_is<NamedParameters>(vargs.get<sol::object>(argc));

                    NamedParameters kwargs;
                    if (has_kwarg) {
                        kwargs = object_as<NamedParameters>(vargs.get<sol::object>(argc));
                    }

                    ${ contentFunction.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }

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
                contentRegister.push(`exports[sol::call_constructor] = sol::factories(::${ lua_fname });`);
            } else {
                const definitions = [];

                if (has_instance) {
                    definitions.push(`[] (::${ fqn }* self, sol::this_state ts, sol::variadic_args vargs) {
                        return ::${ lua_fname }(self, ts, vargs);
                    }`.replace(/^ {20}/mg, "").trim());
                }

                if (has_static) {
                    const call_lua_args = ["ts", "vargs"];
                    if (has_instance) {
                        call_lua_args.unshift(`static_cast<::${ fqn }*>(nullptr)`);
                    }

                    definitions.push(`[] (sol::this_state ts, sol::variadic_args vargs) {
                        return ::${ lua_fname }(${ call_lua_args.join(", ") });
                    }`.replace(/^ {20}/mg, "").trim());
                }

                const overload = definitions.length === 1 ? definitions[0] : `sol::overload(${ definitions.join(", ") })`;

                if (fname.startsWith("sol::")) {
                    contentRegister.push(`exports[${ fname }] = ${ overload };`);
                } else {
                    contentRegister.push(`exports.set_function("${ fname }", ${ overload });`);
                }
            }
        }
    }

    generate(processor, configuration, options, cb) {
        const { generated_include } = configuration;
        const { APP_NAME } = options;

        const files = new Map();
        const registrationsHdr = [];
        const registrations = [];

        for (const fqn of Array.from(processor.classes.keys()).sort()) {
            // TODO https://github.com/ThePhD/sol2/issues/1405

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
                } else if (parent.isStatic()) {
                    contentRegister.push(`sol::table ns = ${ ns }[sol::metatable_key];`);
                } else {
                    contentRegister.push(`sol::usertype<::${ parent.path.join("::") }> ns = ${ ns };`);
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
                    exports[sol::meta_function::new_index] = dynamic_set;
                    exports[sol::meta_function::index] = dynamic_get;

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

            LuaGenerator.writeProperties(processor, coclass, contentRegisterPrivate, contentRegister, options);
            LuaGenerator.writeMethods(processor, coclass, contentRegisterPrivate, contentRegister, options);

            if (contentRegisterPrivate.length !== 0) {
                contentCpp.push(`
                    namespace {
                        using namespace LUA_MODULE_NAME;
                        ${ namespaces.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }

                        ${ contentRegisterPrivate.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
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

            // TODO : add property signature
            // TODO : add method signature

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
