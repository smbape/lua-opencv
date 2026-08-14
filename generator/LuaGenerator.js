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

const LUA_RESERVED_KEYWORDS = new Set([
    "and",
    "elseif",
    "end",
    "in",
    "local",
    "nil",
    "not",
    "or",
    "repeat",
    "until",
]);

const LUA_KEYWORDS = new Set([
    ...LUA_RESERVED_KEYWORDS,

    // https://www.lua.org/manual/5.1/index.html#index
    // Lua functions
    "_G",
    "_VERSION",
    // "assert", // assert also exists in c
    "collectgarbage",
    "dofile",
    "error",
    "getfenv",
    "getmetatable",
    "ipairs",
    "load",
    "loadfile",
    "loadstring",
    "module",
    "next",
    "pairs",
    "pcall",
    "print",
    "rawequal",
    "rawget",
    "rawset",
    "require",
    "select",
    "setfenv",
    "setmetatable",
    "tonumber",
    "tostring",
    "type",
    "unpack",
    "xpcall",

    "coroutine",
    "debug",
    "io",
    "math",
    "os",
    "package",
    "string",
    "table",
]);

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

        if ((cpptype.startsWith("std::map<") || cpptype.startsWith("std::multimap<") || cpptype.startsWith("std::unordered_map<") || cpptype.startsWith("std::unordered_multimap<")) && cpptype.endsWith(">")) {
            this.add_map(cpptype, coclass, options);
        } else if (cpptype.startsWith("std::vector<") && cpptype.endsWith(">")) {
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

        if (!(cpptype.startsWith("std::map<") || cpptype.startsWith("std::multimap<") || cpptype.startsWith("std::unordered_map<") || cpptype.startsWith("std::unordered_multimap<")) || !cpptype.endsWith(">")) {
            throw new Error(`invalid map type ${ cpptype }`);
        }

        const fqn = getTypeDef(cpptype, options);
        if (this.classes.has(fqn) && this.getCoClass(fqn, options).is_stdmap) {
            return;
        }

        const pos = cpptype.indexOf("<");
        const [key_type, value_type] = CoClass.getTupleTypes(cpptype.slice(pos + 1, -">".length));

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
            coclass.addMethod([`${ fqn }.data`, `std::shared_ptr<PointerArray<${ vtype }>>`, ["=ptr", "/Expr=", "/Output=$0 + i", `/WrapAs=std::make_shared<PointerArray<${ vtype }>>`], [
                ["std::ptrdiff_t", "i", "0", []],
            ], "", ""], options);
            coclass.addMethod([`${ fqn }.ptr`, `std::shared_ptr<PointerArray<${ vtype }>>`, ["/S", `/Call=std::make_shared<PointerArray<${ vtype }>>`, "/Expr=$1 + $2"], [
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

        coclass.addMethod([`${ fqn }.sol::meta_function::index`, vtype, ["/Ref", "/Call=_stl_container__index", `/Expr=L, ${ self }, $0`], [
            ["size_t", "index", "", []],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::new_index`, "void", ["/Call=_stl_container__newindex", `/Expr=L, ${ self }, $0`], [
            ["size_t", "index", "", []],
            [vtype, "value", "", ["/C", "/Ref"]],
        ], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::pairs`, "void", ["/Call=_stl_container__ipairs", `/Expr=L, ${ self }`], [], "", ""], options);

        coclass.addMethod([`${ fqn }.sol::meta_function::ipairs`, "void", ["/Call=_stl_container__ipairs", `/Expr=L, ${ self }`], [], "", ""], options);

        coclass.addMethod([`${ fqn }.table`, "void", ["/Call=lua_push", `/Expr=L, ${ self }`], [], "", ""], options);

        coclass.addMethod([`${ fqn }.size`, "size_t", ["=sol::meta_function::length"], [], "", ""], options);

        this.addDependencies(coclass, options);
    },
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

const LUA_RESERVED_DEFINITIONS = new Set([
    "nil",
    "any",
    "boolean",
    "string",
    "number",
    "integer",
    "function",
    "table",
    "thread",
    "userdata",
    "lightuserdata",
]);

const NATIVE_DEFINITION_TYPES = new Map([
    ["std::string", "string"],
    ["const char*", "string|lightuserdata"],
    ["const char *", "string"],

    ["_Bool", "boolean"],
    ["bool", "boolean"],
    ["int", "integer"],
    ["signed", "integer"],
    ["unsigned", "integer"],

    // https://en.cppreference.com/w/cpp/language/types
    ["signed char", "char"],
    ["unsigned char", "uchar"],
    ["short int", "short"],
    ["signed short", "short"],
    ["signed short int", "short"],
    ["unsigned short", "ushort"],
    ["unsigned short int", "ushort"],
    ["signed int", "integer"],
    ["unsigned int", "uint"],
    ["long int", "long"],
    ["signed long", "long"],
    ["signed long int", "long"],
    ["unsigned long", "ulong"],
    ["unsigned long int", "ulong"],
    ["long long", "long"],
    ["long long int", "long"],
    ["signed long long", "long"],
    ["signed long long int", "long"],
    ["unsigned long long", "ulong"],
    ["unsigned long long int", "ulong"],
    ["long double", "double"],
    ["unsigned __int32", "integer"],
    ["unsigned __int64", "integer"],
]);

const NATIVE_TYPES_ALIAS = new Map([
    ["char", "integer|string"],
    ["int", "integer"],
    ["float", "number"],
    ["double", "number"],

    ["uchar", "integer"],
    ["short", "integer"],
    ["ushort", "integer"],
    ["uint", "integer"],
    ["long", "integer"],
    ["ulong", "integer"],

    ["__int32", "integer"],
    ["__int64", "integer"],
]);

const is_pointer_or_const_pointer = type => {
    return /\*\s*(?:const)?$/.test(type);
};

class LuaGenerator {
    static proto = proto;

    static getRegisterFn(coclass) {
        return `register_${ coclass.getClassName() }`;
    }

    static getRegisterClassFn(coclass) {
        return `register_class_${ coclass.getClassName() }`;
    }

    static getRegisterThisFn(coclass) {
        return `register_this_${ coclass.getClassName() }`;
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

    static getProgId(id, { progids }) {
        return progids && progids.has(id) ? progids.get(id) : id;
    }

    static getColassProgId(coclass, options) {
        const path = this.getProgId(coclass.path.join("."), options).split(".");
        while (path.length !== 0 && path[0] === "") {
            path.shift();
        }
        return path;
    }

    // eslint-disable-next-line complexity
    static writeProperties(processor, coclass, { ascendants }, contentRegisterPrivate, contentRegister, contentRegisterThis, options) {
        const { fqn } = coclass;
        const getters = new Map();
        const setters = new Map();
        const registeri = contentRegister.length;
        let hasThisProperties = false;

        if (!coclass.isStatic()) {
            getters.set("__self", `lua_method__self<${ fqn }>`);
        }

        for (const [fname, overloads] of coclass.methods.entries()) {
            const ename = LuaGenerator.getLuaFn(fname);
            const luaFn = `Lua_${ ename.replaceAll("::", "_") }`;

            let has_propget = false;
            let has_propput = false;
            let name = null;

            for (const decl of overloads) {
                const [, , func_modifiers] = decl;

                if (func_modifiers.includes("/attr=propget")) {
                    name = getPropname(fname, func_modifiers);
                    has_propget = true;
                    break;
                } else if (func_modifiers.includes("/attr=propput")) {
                    name = getPropname(fname, func_modifiers);
                    has_propput = true;
                    break;
                }
            }

            if (has_propget || has_propput) {
                if (getters.size === 0 && setters.size === 0) {
                    contentRegisterPrivate.push("");
                }
                contentRegisterPrivate.push(`int ${ luaFn }(lua_State* L);`);
            }

            if (has_propget) {
                getters.set(name, luaFn);
            }

            if (has_propput) {
                setters.set(name, luaFn);
            }
        }

        for (const [name, property] of coclass.properties.entries()) {
            const {type, modifiers} = property;
            const cpptype = processor.getCppType(type, coclass, options);
            const isStatic = modifiers.includes("/S") || coclass.isStatic();
            const obj = `${ isStatic ? `${ fqn }::` : "self->" }`;
            const is_enum = modifiers.includes("/Enum");

            const propname = getPropname(name, modifiers);
            let getter;
            let has_propget = isStatic || is_enum || modifiers.includes("/R") || modifiers.includes("/RW");
            let has_propput = modifiers.includes("/W") || modifiers.includes("/RW");

            for (const modifier of modifiers) {
                if (modifier.startsWith("/R=")) {
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

                    hasThisProperties = true;

                    const typedef = getTypeDef(cpptype, options);
                    const path = [];

                    if (processor.classes.has(typedef)) {
                        path.push(...processor.classes.get(typedef).progid.split("."));
                    } else {
                        path.push(...cpptype.split("::"));
                    }

                    while (path.length !== 0 && path[0] === "") {
                        path.shift();
                    }

                    const id = this.getProgId(path.join("."), options);

                    let thisIndex, moduleIndex;

                    if (coclass.isStatic()) {
                        if (coclass.progid) {
                            moduleIndex = -2;
                            thisIndex = -1;
                        } else {
                            moduleIndex = -1;
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
                            contentRegisterThis.push(`lua_pushvalue(L, ${ moduleIndex }); // push the module`); thisIndex--; moduleIndex--;
                        }

                        contentRegisterThis.push(`lua_rawget_create_if_nil(L, -1, { ${ id.split(".").map(part => JSON.stringify(part)).join(", ") } }); // push ${ cpptype }`);
                        thisIndex--;
                        moduleIndex--;
                        cpptypeIndex = -1;
                    }

                    contentRegisterThis.push(`lua_pushliteral(L, "${ name }");`); thisIndex--; moduleIndex--; cpptypeIndex--;
                    contentRegisterThis.push(`lua_pushvalue(L, ${ cpptypeIndex }); // push ${ cpptype }`); thisIndex--; moduleIndex--; cpptypeIndex--;
                    contentRegisterThis.push(`lua_rawset(L, ${ thisIndex }); // set ${ fqn }::${ name }`); thisIndex += 2; moduleIndex += 2; cpptypeIndex += 2;

                    if (id) {
                        contentRegisterThis.push(`lua_pop(L, 1); // pop ${ cpptype }`); thisIndex++; moduleIndex++; cpptypeIndex++;

                        if (moduleIndex !== -1) {
                            contentRegisterThis.push("lua_pop(L, 1); // pop the module"); thisIndex++; moduleIndex++; cpptypeIndex++;
                        }
                    }

                    contentRegisterThis.push(""); // new line

                    writePropertyDoc(processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput);
                    continue;
                }

                rexpr = `lua_reference_push(L, ${ rexpr });`;
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
                    const lvalue = `${ obj }${ propname }`;

                    if (o_expr) {
                        in_val = makeExpansion(o_expr, lvalue).replace(/\$(?:value\b|\{[^\S\n]*value[^\S\n]*\})/g, in_val);
                    } else if (o_setter) {
                        in_val = `${ obj }${ o_setter }(${ in_val })`;
                    } else {
                        let rvalue = in_val;

                        if (o_type !== cpptype) {
                            rvalue = `static_cast<${ cpptype }>(${ rvalue })`;
                        }

                        in_val = `${ lvalue } = ${ rvalue }`;
                    }

                    wexpr.push(`
                    {
                        auto holder_value = lua_to(L, 3, static_cast<${ wtype }*>(nullptr), is_valid);
                        if (is_valid) {
                            decltype(auto) value = extract_holder(holder_value, static_cast<${ wtype }*>(nullptr));
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

            const namespaces = [];
            useNamespaces(namespaces, "push", processor, coclass);

            if (rexpr) {
                if (rexpr.includes("is_valid") && !obj_decl[0].includes("bool is_valid;")) {
                    obj_decl.unshift("bool is_valid;");
                }

                const luaFn = `Lua_getter_${ name }`;

                getters.set(name, luaFn);

                contentRegisterPrivate.push("", `
                    inline int ${ luaFn }(lua_State* L) {
                        ${ [...namespaces, ...obj_decl].join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                        ${ rexpr.split("\n").join(`\n${ " ".repeat(24) }`) }
                        return 1;
                    }
                `.replace(/^ {20}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));
            }

            if (wexpr) {
                if (wexpr.includes("is_valid") && !obj_decl[0].includes("bool is_valid;")) {
                    obj_decl.unshift("bool is_valid;");
                }

                const luaFn = `Lua_setter_${ name }`;

                setters.set(name, luaFn);

                contentRegisterPrivate.push("", `
                    inline int ${ luaFn }(lua_State* L) {
                        ${ [...namespaces, ...obj_decl].join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                        ${ wexpr.split("\n").join(`\n${ " ".repeat(24) }`) }
                    }
                `.replace(/^ {20}/mg, "").trim().replace(/^[^\S\n]*\n/mg, ""));
            }

            writePropertyDoc(processor, coclass, name, propname, modifiers, cpptype, has_propget, has_propput);
        }

        if (getters.size !== 0) {
            const mssing = [];
            const parents = [...coclass.parents].filter(parent => {
                if (!ascendants.has(parent)) {
                    return false;
                }

                return ascendants.get(parent).some(ascendant => {
                    if (!processor.classes.has(ascendant)) {
                        return false;
                    }

                    const keys = Array.from(processor.classes.get(ascendant).methods.keys());
                    return keys.length !== 0 && keys.some(fname => {
                        const ename = LuaGenerator.getLuaFn(fname);
                        return ename === "__index" || ename === "sol::meta_function::index";
                    });
                });
            });

            if (parents.length !== 0) {
                mssing.push(`
                    if (${ parents.map(parent => `usertype_info<${ parent }>::__index(L) != 0`).join(` ||\n${ " ".repeat(24) }`) }) {
                        return lua_gettop(L) - vargc;
                    }
                `.replace(/^ {20}/mg, "").trim());
            }

            mssing.push("return 0;");

            const tries = [];

            if (coclass.isStatic()) {
                tries.push("try__index");
            } else {
                tries.push("try_mt__index", `usertype_info<${ fqn }>::__index`);
            }

            contentRegisterPrivate.push("", `
                int try__index(lua_State* L) {
                    auto vargc = lua_gettop(L);
                    if (vargc != 2) {
                        return luaL_error(L, "%d arguments expected, got %d", 2, vargc);
                    }

                    if (lua_type(L, 2) == LUA_TSTRING) {
                        size_t len; auto key = lua_tolstring(L, 2, &len);

                        ${ Array.from(getters).map(([name, luaFn]) => `
                            if (len == ${ name.length } /* sizeof("${ name }") - 1 */ && strncmp(key, "${ name }", ${ name.length } /* sizeof("${ name }") - 1 */) == 0) {
                                return ${ luaFn }(L);
                            }
                        `.replace(/^ {4}/mg, "").trim()).join(`\n\n${ " ".repeat(24) }`) }
                    }

                    ${ mssing.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                }

                int __index(lua_State* L) {
                    auto vargc = lua_gettop(L);

                    if (${ tries.map(fn => `${ fn }(L) != 0`).join(` ||\n${ " ".repeat(24) }`) }) {
                        return lua_gettop(L) - vargc;
                    }

                    return lua_missing_declaration(L);
                }
            `.replace(/^ {16}/mg, "").trim().replace(/[^\S\n]+$/mg, ""));
        } else if (!coclass.isStatic()) {
            const tries = ["try_mt__index"];

            contentRegisterPrivate.push("", `
                int __index(lua_State* L) {
                    auto vargc = lua_gettop(L);

                    if (${ tries.map(fn => `${ fn }(L) != 0`).join(` ||\n${ " ".repeat(24) }`) }) {
                        return lua_gettop(L) - vargc;
                    }

                    return lua_missing_declaration(L);
                }
            `.replace(/^ {16}/mg, "").trim().replace(/[^\S\n]+$/mg, ""));
        }

        if (setters.size !== 0) {
            const mssing = [];
            const parents = [...coclass.parents].filter(parent => processor.classes.has(parent) && !processor.classes.get(parent).isStatic());

            if (parents.length !== 0) {
                mssing.push(`
                    if (${ parents.map(parent => `usertype_info<${ parent }>::__newindex(L) != -1`).join(` ||\n${ " ".repeat(24) }`) }) {
                        return 0;
                    }
                `.replace(/^ {20}/mg, "").trim().replace(/[^\S\n]+$/mg, ""));
            }

            mssing.push("return -1;");

            contentRegisterPrivate.push("", `
                int try__newindex(lua_State* L) {
                    auto vargc = lua_gettop(L);
                    if (vargc != 3) {
                        return luaL_error(L, "%d arguments expected, got %d", 3, vargc);
                    }

                    if (lua_type(L, 2) == LUA_TSTRING) {
                        size_t len; auto key = lua_tolstring(L, 2, &len);

                        ${ Array.from(setters).map(([name, luaFn]) => `
                            if (len == ${ name.length } /* sizeof("${ name }") - 1 */ && strncmp(key, "${ name }", ${ name.length } /* sizeof("${ name }") - 1 */) == 0) {
                                return ${ luaFn }(L);
                            }
                        `.replace(/^ {4}/mg, "").trim()).join(`\n\n${ " ".repeat(24) }`) }
                    }

                    ${ mssing.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }
                }

                int __newindex(lua_State* L) {
                    const auto ret = ${ coclass.isStatic() ? "try__newindex" : `usertype_info<${ fqn }>::__newindex` }(L);
                    if (ret != -1) {
                        return ret;
                    }
                    return lua_missing_declaration(L);
                }
            `.replace(/^ {16}/mg, "").trim());
        }

        return { getters, setters, hasThisProperties };
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
    static writeMethods(processor, coclass, { getters, setters }, contentRegisterPrivate, contentRegister, contentDecl, options) {
        const { fqn } = coclass;
        const { shared_ptr } = options;
        const indent = " ".repeat(4);
        const methods = new Map();
        const meta_methods = new Map();
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
            const hasConstructor = overloads.some(([, , func_modifiers]) => LuaGenerator.isConstructor(func_modifiers));
            let has_propget = false;
            let has_propput = false;

            for (const decl of overloads) {
                const [, , func_modifiers] = decl;

                if (func_modifiers.includes("/attr=propget")) {
                    has_propget = true;
                    break;
                } else if (func_modifiers.includes("/attr=propput")) {
                    has_propput = true;
                    break;
                }
            }

            // generate docs header
            processor.docs.push(`### ${ fqn.replaceAll("::", ".") }.${ hasConstructor ? cname : fname }\n`.replaceAll("_", "\\_"));

            let isConstructor = false;
            let overload_id = 0;
            let argcMax = 0;
            const argnames = new Set();

            let not_found = "LUAL_MODULE_ERROR_RETURN(L, \"Overload resolution failed\")";
            if (!coclass.isStatic()) {
                if (ename === "__index" || ename === "sol::meta_function::index") {
                    not_found = `return ${ getters.size === 0 ? "lua_missing_declaration" : "try__index" }(L)`;
                } else if (ename === "__newindex" || ename === "sol::meta_function::new_index") {
                    not_found = `return ${ setters.size === 0 ? "lua_missing_declaration" : "try__newindex" }(L)`;
                }
            }

            for (const decl of overloads) {
                overload_id++;

                const [name, return_value_type, func_modifiers] = decl;
                const list_of_arguments = decl[3].slice();
                const variadic = list_of_arguments.length !== 0 && list_of_arguments.at(-1)[0] === "...";
                const first_argument_is_lua_state = list_of_arguments.length !== 0 && /^lua_State\s*\*\s*$/.test(list_of_arguments[0][0]);

                if (first_argument_is_lua_state) {
                    list_of_arguments.shift();
                }

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
                const offset = has_propget ? 2 : has_propput ? 3 : isStatic ? 0 : 1;
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

                    if (argtype.startsWith("std::optional<") && defval === "") {
                        defval = "std::nullopt";
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
                    const deref = (is_in_arg || is_out_arg) && is_ptr && !PTR.has(argtype);

                    if (deref) {
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

                    const arr_cpptype = processor.getCppType(arrtype, coclass, options);
                    const is_shared_ptr = cpptype.startsWith(`${ shared_ptr }<`);
                    const is_by_ref = !is_ptr && !is_shared_ptr && processor.classes.has(cpptype) && !processor.enums.has(cpptype);
                    const var_type = is_array ? arr_cpptype : cpptype;
                    const argi = i + offset;
                    const argn = `${ argi + 1 } + __top__`;
                    const nd_mat = arg_modifiers.includes("/ND");
                    const defarg = `default_${ argname }_value`;
                    const defptr = `default_${ argname }_ptr`;

                    if (is_out_arg && is_array) {
                        is_optional = true;
                    }

                    const maybe_ptr = deref && is_optional && (!is_array || defval !== "");

                    if (is_out_arg) {
                        if (is_array) {
                            retval.push([j, `lua_push(L, ${ argname }_${ arrtype });`]);
                        } else {
                            const lua_push_args = ["L", `${ maybe_ptr ? "*" : "" }${ argname }`];

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

                            if (maybe_ptr) {
                                retval.push([j, `
                                    if (${ argname }) {
                                        lua_push(${ lua_push_args.join(", ") });
                                    } else {
                                        lua_pushnil(L);
                                    }
                                `.replace(/^ {36}/mg, "").trim()]);
                            } else {
                                retval.push([j, `lua_push(${ lua_push_args.join(", ") });`]);
                            }
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

                    if (maybe_ptr) {
                        extractors.push(`auto ${ defptr } = &${ defarg };`);
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
                        const args = [`static_cast<${ var_type }*>(nullptr)`, "is_valid"];

                        if (maybe_ptr) {
                            args.splice(1, 0, defptr);
                        }

                        const holder_type = `Holder_${ argname }`;
                        const holder_arg = `holder_${ argname }`;

                        extractors.push(`
                            using ${ holder_type } = decltype(lua_to(L, ${ argn }, ${ args.join(", ") }));
                            ${ holder_type } ${ holder_arg };

                            if (${ largc } > ${ argi }) {
                                // positional parameter
                                ${ holder_arg } = lua_to(L, ${ argn }, ${ args.join(", ") });
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
                                    ${ holder_arg } = lua_to(L, -1, ${ args.join(", ") });
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

                        if (maybe_ptr) {
                            extractors.push("", `decltype(auto) ${ argname } = ${ defptr };`);
                        } else {
                            extractors.push("", `
                                decltype(auto) ${ argname } = ${ is_optional ? `!is_valid ? ${ defarg } : ` : "" }extract_holder(${ holder_arg }, static_cast<${ var_type }*>(nullptr));
                            `.replace(/^ {32}/mg, "").trim());
                        }
                    }

                    if (is_optional && is_first_optional) {
                        firstoptarg = Math.min(firstoptarg, i);
                        is_first_optional = false;
                    }

                    if (maybe_ptr) {
                        callarg = argname;
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

                if (first_argument_is_lua_state) {
                    callargs.unshift("L");
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
                const uses_lua_state = first_argument_is_lua_state || /\bL\b/.test(expr);
                const has_gil_unlock = func_modifiers.includes("/GU");

                if (has_body || return_value_type === "void") {
                    if (!has_body && has_gil_unlock) {
                        callee = `
                            decltype(auto) __gil__ = get_thread_gil(L);
                            __gil__->unlock();
                            ${ callee.trim().split("\n").join(`\n${ " ".repeat(28) }`) };
                            __gil__->lock()
                        `.replace(/^ {28}/mg, "").trim();
                    }

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
                    const lua_push_args = [callee.trim()];

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

                    const expressions = [];

                    if (has_gil_unlock) {
                        expressions.push("decltype(auto) __gil__ = get_thread_gil(L);");
                        expressions.push("__gil__->unlock();");
                    }

                    if (isConstructor) {
                        if (default_constructor) {
                            callee = `auto self = ${ callee.trim() };`;
                            expressions.push(...callee.split("\n"), ...default_constructor.trim().split("\n"));
                            lua_push_args[0] = "self";
                        } else if (hasDC) {
                            callee = `auto self = ${ callee.trim() };`;
                            expressions.push(...callee.split("\n"));
                            lua_push_args[0] = "self";
                        }
                    }

                    if (isConstructor && hasDC) {
                        expressions.unshift("is_valid = true;");

                        if (has_gil_unlock) {
                            expressions.push("__gil__->lock();");
                        }

                        expressions.push(`return ${ lua_push_args.join(", ") };`);
                    } else {
                        const lua_push = func_modifiers.includes("/Ref") ? "lua_reference_push" : "lua_push";
                        let expr = lua_push_args.join(", ");

                        if (has_gil_unlock) {
                            expressions.push(`decltype(auto) __ret = ${ expr };`);
                            expressions.push("__gil__->lock();");
                            expr = "__ret";
                        }

                        expressions.push(`${ lua_push }(L, ${ expr });`);
                    }

                    retval.push([-1, `
                        try {
                            ${ expressions.join("\n").split("\n").join(`\n${ " ".repeat(28) }`) }
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

                LuaGenerator.writeMethodDoc(
                    processor,
                    coclass,
                    fname,
                    overload_id,
                    decl,
                    outlist,
                    indexes,
                    firstoptarg,
                    isConstructor,
                    options
                );

                LuaGenerator.writeMethodDefinition(
                    processor,
                    coclass,
                    fname,
                    overload_id,
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
                contentRegisterPrivate.push("    is_valid = false;", "");
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
                methods.set(meta_method, luaFn);
            } else if (!has_propget && !has_propput) {
                methods.set(ename, luaFn);
            }

            if (isConstructor || fname === cname) {
                meta_methods.set("__call", "__call_constructor");
            }
        }

        if (methods.has("__index")) {
            contentDecl.push("", `
                int usertype_info<${ fqn }>::__index(lua_State* L) {
                    return ${ methods.get("__index") }(L);
                }
            `.replace(/^ {16}/mg, "").trim());

            if (getters.size !== 0 || !coclass.isStatic()) {
                methods.set("__index", "__index");
            }
        } else if (getters.size !== 0) {
            methods.set("__index", "__index");

            if (!coclass.isStatic()) {
                contentDecl.push("", `
                    int usertype_info<${ fqn }>::__index(lua_State* L) {
                        return try__index(L);
                    }
                `.replace(/^ {20}/mg, "").trim());
            }
        } else if (!coclass.isStatic()) {
            contentDecl.push("", `
                int usertype_info<${ fqn }>::__index(lua_State* L) {
                    return 0;
                }
            `.replace(/^ {16}/mg, "").trim());
            methods.set("__index", "__index");
        } else {
            methods.set("__index", "lua_missing_declaration");
        }

        if (methods.has("__newindex")) {
            contentDecl.push("", `
                int usertype_info<${ fqn }>::__newindex(lua_State* L) {
                    return ${ methods.get("__newindex") }(L);
                }
            `.replace(/^ {16}/mg, "").trim());
        } else if (setters.size !== 0) {
            methods.set("__newindex", "__newindex");

            if (!coclass.isStatic()) {
                contentDecl.push("", `
                    int usertype_info<${ fqn }>::__newindex(lua_State* L) {
                        return try__newindex(L);
                    }
                `.replace(/^ {20}/mg, "").trim());
            }
        } else if (!coclass.isStatic()) {
            contentDecl.push("", `
                int usertype_info<${ fqn }>::__newindex(lua_State* L) {
                    return -1;
                }
            `.replace(/^ {16}/mg, "").trim());
            methods.set("__newindex", "lua_missing_declaration");
        } else {
            methods.set("__newindex", "lua_missing_declaration");
        }

        meta_methods.set("__index", methods.get("__index"));
        meta_methods.set("__newindex", methods.get("__newindex"));

        if (coclass.isStatic()) {
            methods.delete("__index");
            methods.delete("__newindex");
        }

        contentRegisterPrivate.push("", `
            const struct luaL_Reg methods[] = {
                ${ Array.from(methods.entries()).map(([ename, fname]) => `{"${ ename }", ${ fname }}`).concat(["{NULL, NULL} // Sentinel"]).join(`,\n${ " ".repeat(16) }`) }
            };

            const struct luaL_Reg meta_methods[] = {
                ${ Array.from(meta_methods.entries()).map(([ename, fname]) => `{"${ ename }", ${ fname }}`).concat(["{NULL, NULL} // Sentinel"]).join(`,\n${ " ".repeat(16) }`) }
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

    static writeMethodDoc(
        processor,
        coclass,
        fname,
        overload_id,
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
        const [name, return_value_type, func_modifiers] = decl;

        let [, , , list_of_arguments] = decl;
        if (list_of_arguments.length !== 0 && /^lua_State\s*\*\s*$/.test(list_of_arguments[0][0])) {
            list_of_arguments = list_of_arguments.slice(1);
        }

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
            const is_in_array = /^Input(?:Output)?Array(?:OfArrays)?$/.test(argtype);
            const is_out_array = /^(?:Input)?OutputArray(?:OfArrays)?$/.test(argtype);
            let str = is_in_array || is_out_array ? argtype : LuaGenerator.getDocCppType(processor, argtype, coclass, options);

            if (arg_modifiers.includes("/C")) {
                str = `const ${ str }`;
            }

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

    static getLuaIdentifier(identifier) {
        return LUA_KEYWORDS.has(identifier) ? `_${ identifier }` : identifier;
    }

    static getDefinitionAliases(processor, options) {
        const aliases = new Map(Array.from(NATIVE_TYPES_ALIAS.entries()));

        const daliases = [];

        for (const [name, _type] of processor.typedefs.entries()) {
            let type = _type;

            if (type.startsWith("struct ")) {
                type = type.slice("struct ".length);
            }

            if (name !== type) {
                daliases.push([name, type]);
            }
        }

        if (options.daliases) {
            daliases.push(...options.daliases.entries());
        }

        for (const [name, _type] of daliases) {
            if (LUA_RESERVED_DEFINITIONS.has(name) || !/^\S+$/.test(name)) {
                continue;
            }

            let type = _type;

            if (NATIVE_DEFINITION_TYPES.has(type)) {
                type = NATIVE_DEFINITION_TYPES.get(type);
            }

            if (name !== type) {
                aliases.set(name, type);
            }
        }

        let removeUndefinedDocName = true;
        while (removeUndefinedDocName) {
            removeUndefinedDocName = false;

            for (const [name, _type] of aliases.entries()) {
                let type = _type;

                const seen = new Set();
                while (aliases.has(type) && !seen.has(type)) {
                    seen.add(type);
                    type = aliases.get(type);
                }

                if (NATIVE_DEFINITION_TYPES.has(type)) {
                    type = NATIVE_DEFINITION_TYPES.get(type);
                }

                if (type.split("|").some(itype => !LUA_RESERVED_DEFINITIONS.has(itype) && !processor.classes.has(itype))) {
                    aliases.delete(name);
                    removeUndefinedDocName = true;
                }
            }
        }

        return aliases;
    }

    static getDefitionType(processor, type, coclass, options) {
        if (type.startsWith("std::vector<") && type.endsWith(">")) {
            const pos = type.indexOf("<");
            return `Array<${ this.getDefitionType(processor, type.slice(pos + 1, -">".length), coclass, options) }>`;
        }

        if (type.startsWith("std::optional<") && type.endsWith(">")) {
            const pos = type.indexOf("<");
            return this.getDefitionType(processor, type.slice(pos + 1, -">".length), coclass, options);
        }

        if (type.startsWith("std::tuple<") && type.endsWith(">")) {
            const pos = type.indexOf("<");
            const types = CoClass.getTupleTypes(type.slice(pos + 1, -">".length));
            return `[${ types.map(itype => this.getDefitionType(processor, itype, coclass, options)).join(", ") }]`;
        }

        if (type.startsWith("const ") && !NATIVE_DEFINITION_TYPES.has(type)) {
            type = type.slice("const ".length);
        } else if (/\bconst$/.test(type)) {
            type = type.slice(0, -"const".length).trim();
        }

        if (type.startsWith("struct ")) {
            type = type.slice("struct ".length);
        }

        let definitiontype;

        if (NATIVE_DEFINITION_TYPES.has(type)) {
            definitiontype = NATIVE_DEFINITION_TYPES.get(type);
        } else if (type.endsWith("*")) {
            const ptype = type.slice(0, -1).trim();
            if (type.endsWith("**") || !processor.classes.has(ptype)) {
                definitiontype = "userdata|lightuserdata";
                if (ptype === "void") {
                    definitiontype += "|string";
                }
            } else {
                definitiontype = this.getDefitionType(processor, ptype, coclass, options);
            }
        } else if (processor.enums.has(type) && !processor.classes.has(type)) {
            definitiontype = this.getProgId(type, options);
        } else if (processor.daliases.has(type)) {
            definitiontype = type;
        } else if (processor.typedefs.has(type) && processor.typedefs.get(type) != null && processor.typedefs.get(type).startsWith("struct ")) {
            definitiontype = type;
        } else {
            definitiontype = processor.getCppType(type, coclass, options);
        }

        if (processor.classes.has(definitiontype)) {
            definitiontype = processor.classes.get(definitiontype).progid;
        }

        return typeof options.getDefitionType === "function" ? options.getDefitionType(processor, definitiontype, coclass, options) : definitiontype;
    }

    static getModuleName(options) {
        return `${ options.APP_NAME.toLowerCase() }_lua`;
    }

    static writePropertiesDefinition(processor, ascendants, classes, options) {
        const module_name = this.getModuleName(options);
        const refs = [];

        const fields = classes.map(coclass => Array.from(coclass.properties.entries()).map(([name, property]) => {
            const { fqn, progid } = coclass;
            const {modifiers} = property;

            const propname = getPropname(name, modifiers);

            if (propname === "this") {
                const cpptype = processor.getCppType(property.type, coclass, options);
                if (`${ fqn }::${ name }` === cpptype) {
                    return null;
                }

                const typedef = getTypeDef(cpptype, options);
                const path = [];

                if (processor.classes.has(typedef)) {
                    path.push(...processor.classes.get(typedef).progid.split("."));
                } else {
                    path.push(...cpptype.split("::"));
                }

                while (path.length !== 0 && path[0] === "") {
                    path.shift();
                }

                const getter = [module_name];
                if (path.length !== 0) {
                    const id = this.getProgId(path.join("."), options);
                    getter.push(id);
                }

                const setter = [module_name];
                if (progid) {
                    setter.push(progid);
                }
                setter.push(name);

                refs.push(`${ setter.join(".") } = ${ getter.join(".") }`);

                return null;
            }

            let {type} = property;
            if (modifiers.includes("/C")) {
                type = `const ${ type }`;
            }

            const nullable = is_pointer_or_const_pointer(type) ? "?" : "";

            type = LuaGenerator.getDefitionType(processor, type, coclass, options);
            return `---@field ${ propname }${ nullable } ${ type }`;
        }).filter(definition => definition !== null)).flat();

        const [coclass] = classes;
        const {fqn, progid} = coclass;
        const path = progid.split(".");
        const parents = ascendants.has(fqn) ? ascendants.get(fqn).slice(1) : [];
        parents.push(coclass.isStatic() ? "table" : "userdata");

        if (processor.classes.has(parents[0])) {
            parents[0] = processor.classes.get(parents[0]).progid;
        }

        // TODO : handle constructors signatures
        if (!coclass.isStatic()) {
            fields.push(`---@overload fun(): ${ progid }`);
        }

        const definitions = [
            `---@class ${ progid } : ${ parents[0] }`,
            ...fields,
            `${ path.length === 1 ? "local " : "" }${ progid } = {}`,
            `${ module_name }.${ progid } = ${ progid }\n`,
        ];

        if (fqn && progid !== fqn.replaceAll("::", ".")) {
            definitions.unshift(`---@alias ${ fqn.replaceAll("::", ".") } ${ progid }`);
        }

        if (refs.length !== 0) {
            // new line
            refs.push("");
        }

        processor.definitions.push(...definitions);

        return refs;
    }

    static writeEnumsDefinition(processor, options) {
        for (const [fqn, decl] of processor.enums.entries()) {
            if (processor.classes.has(fqn)) {
                continue;
            }

            const path = fqn.split("::");
            const enum_class = path.slice(0, -1).join("::");
            const enum_coclass = processor.getCoClass(enum_class, options);

            const [, , , enums] = decl;

            const literals = [];

            for (const edecl of enums) {
                const [ename, , enum_modifiers] = edecl;
                if (!ename.startsWith("const ")) {
                    throw new Error(`enum ${ ename } is not supported`);
                }
                const epath = ename.slice("const ".length).split(".");

                // known invalid enum
                if (epath.join("::") === "cv::detail::ArgKind::OPAQUE") {
                    continue;
                }

                if (options.noEnumExport) {
                    continue;
                }

                let propname = epath[epath.length - 1];

                for (const modifier of enum_modifiers) {
                    if (modifier.startsWith("=")) {
                        propname = modifier.slice("=".length);
                    }
                }

                literals.push(`\`${ enum_coclass.progid }.${ propname }\``);
            }

            if (fqn.endsWith("<unnamed>")) {
                continue;
            }

            const progid = this.getProgId(fqn, options);
            processor.definitions.push(`---@alias ${ progid } ${ literals.join(" | ") }`);
        }

        if (processor.enums.size !== 0) {
            processor.definitions.push("");
        }
    }

    static writeMethodDefinition(
        processor,
        coclass,
        fname,
        overload_id,
        decl,
        outlist,
        indexes,
        firstoptarg,
        isConstructor,
        options
    ) {
        if (isConstructor) {
            // TODO : support constructor definition
            return;
        }

        // const ename = LuaGenerator.getLuaFn(fname);
        // const meta_method = LuaGenerator.getMetaMethod(ename);
        const {fqn} = coclass;
        const [name, return_value_type, func_modifiers] = decl;

        let [, , , list_of_arguments] = decl;
        if (list_of_arguments.length !== 0 && /^lua_State\s*\*\s*$/.test(list_of_arguments[0][0])) {
            list_of_arguments = list_of_arguments.slice(1);
        }

        const argc = list_of_arguments.length;
        const isStatic = func_modifiers.includes("/S") || coclass.isStatic();
        const cppfqn = processor.hasTypeDef(fqn) ? processor.typedefs.get(fqn) : fqn;
        // const cname = options.cname ? options.cname : "create";

        firstoptarg = Math.min(firstoptarg, argc);

        const argtypes = new Map([
            ["self", cppfqn],
            ["retval", return_value_type],
        ]);

        const params = Array.from(new Array(argc).keys()).map(i => {
            const j = indexes[i];
            const arg = list_of_arguments[j];
            const arg_modifiers = arg[3];
            let argtype = arg[0] === "..." ? "any" : (arg_modifiers.includes("/C") ? "const " : "") + arg[0];
            const argname = arg[0] === "..." ? "..." : arg[1];
            const is_optional = i >= firstoptarg ? "?" : "";

            if (is_pointer_or_const_pointer(argtype) && (arg_modifiers.includes("/O") || arg_modifiers.includes("/IO"))) {
                argtype = argtype.replace(/\bconst$/, "").trim().slice(0, -1).trim();
            }

            argtypes.set(argname, argtype);

            const types = [LuaGenerator.getDefitionType(processor, argtype, coclass, options)];

            if (types[0] === "string") {
                types.push("lightuserdata");
            }

            if (i < firstoptarg && is_pointer_or_const_pointer(argtype)) {
                types.push("nil");
            }

            return `---@param ${ LuaGenerator.getLuaIdentifier(argname) }${ is_optional } ${ types.join("|") }`;
        });

        const returns = outlist.map(argname => {
            const argtype = argtypes.get(argname);
            const nullable = is_pointer_or_const_pointer(argtype) ? "|nil" : "";
            return `---@return ${ LuaGenerator.getDefitionType(processor, argtype, coclass, options) }${ nullable } ${ LuaGenerator.getLuaIdentifier(argname) }`;
        });

        const caller = [
            this.getModuleName(options),
            ...coclass.progid.split(".")
        ].join(".");

        const parts = name.split(".");

        if (parts.at(-1).startsWith("operator") || fname.includes("sol::meta_function::")) {
            // TODO : support operator definition
            return;
        }

        // generate docs body
        const argnamelist = Array.from(new Array(argc).keys()).map(i => {
            const j = indexes[i];
            const arg = list_of_arguments[j];
            return arg[0] === "..." ? arg[0] : arg[1];
        });

        const fn = `function ${ caller }${ isStatic ? "." : ":" }${ fname }(${ argnamelist.map(LuaGenerator.getLuaIdentifier).join(", ") }) end`;

        processor.definitions.push([
           ...params,
           ...returns,
            fn,
            ""
        ].join("\n"));
    }

    static setFileHeader(fileHdr, registers, files, options) {
        files.set(sysPath.join(options.output, fileHdr), `
            #pragma once
            #include <lua_generated_include.hpp>
            #include <luadef.hpp>

            namespace LUA_MODULE_NAME {
                ${ registers.join("\n").split("\n").join(`\n${ " ".repeat(16) }`) }
            }
        `.replace(/^ {12}/mg, "").trim().replace(/[^\S\n]+$/mg, ""));
    }

    static getSortedFQNs(processor) {
        return Array.from(processor.classes.keys()).sort((a, b) => {
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
        });
    }

    generate(processor, configuration, options, cb) {
        const { generated_include } = configuration;

        const files = new Map();
        const registrationsHdr = [];
        const registerClasses = [];
        const registerInherits = [];
        const registerDefaults = [];
        const registerThises = [];
        const registrations = [];

        const assignableTypes = new Map();
        const ascendants = new Map();

        // denormalize parents and children
        for (const [fqn, coclass] of processor.classes.entries()) {
            if (coclass.isStatic()) {
                continue;
            }

            ascendants.set(fqn, [fqn]);

            const parents = [...coclass.parents];

            for (const parent of parents) {
                if (processor.classes.has(parent) && !processor.classes.get(parent).isStatic()) {
                    if (!assignableTypes.has(parent)) {
                        assignableTypes.set(parent, new Set());
                    }
                    assignableTypes.get(parent).add(fqn);
                    ascendants.get(fqn).push(parent);
                }

                if (processor.bases.has(parent)) {
                    for (const base of processor.bases.get(parent)) {
                        parents.push(base);
                    }
                }
            }
        }

        for (const [fqn, coclass] of processor.classes.entries()) {
            if (coclass.isStatic()) {
                continue;
            }

            if (!assignableTypes.has(fqn)) {
                assignableTypes.set(fqn, new Set());
            }

            assignableTypes.set(fqn, [fqn, ...assignableTypes.get(fqn)]);
        }

        if (!processor.definitions) {
            processor.definitions = [];
        }

        const sortedFqns = LuaGenerator.getSortedFQNs(processor);

        const progids = new Map();

        for (const fqn of sortedFqns) {
            const coclass = processor.classes.get(fqn);
            const path = LuaGenerator.getColassProgId(coclass, options);
            coclass.progid = path.join(".");

            if (coclass.progid) {
                if (!progids.has(coclass.progid)) {
                    progids.set(coclass.progid, []);
                }
                progids.get(coclass.progid).push(coclass);
            }
        }

        const aliases = LuaGenerator.getDefinitionAliases(processor, options);
        processor.daliases = aliases;
        const drefs = [];

        for (const [, classes] of progids.entries()) {
            const refs = LuaGenerator.writePropertiesDefinition(processor, ascendants, classes, options);
            drefs.push(...refs);
        }

        LuaGenerator.writeEnumsDefinition(processor, options);

        for (const fqn of sortedFqns) {
            const docid = processor.docs.length;

            const coclass = processor.classes.get(fqn);
            const fileCpp = coclass.getCPPFileName(options);
            const fileHdr = `${ fileCpp.slice(0, -".cpp".length) }.hpp`;
            const registerFn = LuaGenerator.getRegisterFn(coclass);
            const registerClassFn = LuaGenerator.getRegisterClassFn(coclass);
            const registerThisFn = LuaGenerator.getRegisterThisFn(coclass);
            const hasConstructor = Array.from(coclass.methods.values()).some(overloads => {
                return overloads.some(([, , func_modifiers]) => LuaGenerator.isConstructor(func_modifiers));
            });

            registrationsHdr.push(`#include <${ fileHdr }>`);
            registrations.push(`${ registerFn }(L);`);

            const registers = [`void ${ registerFn }(lua_State* L);`];
            const contentDecl = [];
            const path = LuaGenerator.getColassProgId(coclass, options);

            if (!coclass.isStatic()) {
                registerClasses.push(`${ registerClassFn }(L);`);
                registers.unshift(`void ${ registerClassFn }(lua_State* L);`);

                const descendants = assignableTypes.get(fqn);

                // https://scottmeyers.blogspot.com/2015/09/should-you-be-using-something-instead.html
                // linear search is faster or competetive to unordered_set on std::vector until around 20 - 35 elements
                // For multiple if statements, it seems to also be the seem
                // cf. perf.lua, model = cv.legacy.MultiTracker.create(), model:getDefaultName()
                const lines = [];

                if (descendants.length > 20) {
                    lines.push(...`
                        const auto luaopen_index = get_luaopen_index(L);
                        thread_local std::unordered_set<const void*> metatable_pointers = {
                            ${ descendants.map(descendant => `usertype_metatable_pointer<${ descendant }>(luaopen_index)`).join(`,\n${ " ".repeat(28) }`) }
                        };

                        is_valid = metatable_pointers.contains(mt_pointer);
                    `.replace(/^ {24}/mg, "").trim().split("\n"));
                } else {
                    lines.push(...`
                        const auto luaopen_index = get_luaopen_index(L);

                        is_valid = ${ descendants.map(descendant => `mt_pointer == usertype_metatable_pointer<${ descendant }>(luaopen_index)`).join(` ||\n${ " ".repeat(4) }`) };
                    `.replace(/^ {24}/mg, "").trim().split("\n"));
                }

                const decl = `
                    static std::mutex mutex;
                    static std::vector<const void*> metatable_pointers;
                    static std::vector<int> metatable_refs;
                    static const struct luaL_Reg* methods;
                    static const struct luaL_Reg* meta_methods;
                    static std::shared_ptr<${ fqn }> lua_userdata_to(lua_State* L, int index, bool& is_valid);
                    static int __index(lua_State* L);
                    static int __newindex(lua_State* L);
                `.replace(/^ {20}/mg, "").trim().split("\n");

                const impl = `
                    std::mutex usertype_info<${ fqn }>::mutex;
                    std::vector<const void*> usertype_info<${ fqn }>::metatable_pointers;
                    std::vector<int> usertype_info<${ fqn }>::metatable_refs;
                    const struct luaL_Reg* usertype_info<${ fqn }>::methods = ::methods;
                    const struct luaL_Reg* usertype_info<${ fqn }>::meta_methods = ::meta_methods;

                    std::shared_ptr<${ fqn }> usertype_info<${ fqn }>::lua_userdata_to(lua_State* L, int index, bool& is_valid) {
                        is_valid = lua_isuserdata(L, index) && lua_getmetatable(L, index);

                        if (is_valid) {
                            const auto mt_pointer = lua_topointer(L, -1);
                            lua_pop(L, 1);

                            ${ lines.join(`\n${ " ".repeat(28) }`) }

                            if (is_valid) {
                                return *static_cast<std::shared_ptr<${ fqn }>*>(lua_touserdata(L, index));
                            }
                        }

                        return std::shared_ptr<${ fqn }>();
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
                        static std::unordered_map<std::size_t, std::function<void(lua_State*, const std::shared_ptr<${ fqn }>&)>> derives_pushers;
                    `.replace(/^ {24}/mg, "").trim().split("\n"));

                    impl.push(...`
                        std::unordered_map<std::size_t, std::function<void(lua_State*, const std::shared_ptr<${ fqn }>&)>> usertype_info<${ fqn }>::derives_pushers;
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
                        return lua_rawget_create_if_nil(L, -1, { ${ path.map(part => JSON.stringify(part)).join(", ") } });
                    }
                `.replace(/^ {20}/mg, "").trim());
            }

            const contentCpp = ["#include <lua_generated_pch.hpp>", ""]; // GCC: Only one precompiled header can be used in a particular compilation.
            const contentRegisterPrivate = [];
            const contentRegister = [];

            if (coclass.is_enum_class) {
                if (path.length !== 0) {
                    contentRegister.push(`lua_rawget_create_if_nil(L, -1, { ${ path.map(part => JSON.stringify(part)).join(", ") } });`, "");
                }

                contentRegister.push(Array.from(coclass.properties.entries()).map(([name, {value}]) => `lua_pushliteral(L, "${ name }"); lua_push(L, ${ value }); lua_rawset(L, -3);`).join("\n"));

                for (const [name, {type, modifiers, value}] of coclass.properties.entries()) {
                    const cpptype = processor.getCppType(type, coclass, options);
                    writePropertyDoc(processor, coclass, name, value.slice(fqn.length + "::".length), modifiers, cpptype, true, false);
                }

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

                LuaGenerator.setFileHeader(fileHdr, registers, files, options);
                files.set(sysPath.join(options.output, fileCpp), contentCpp.join("\n").replace(/[^\S\r\n]+$/mg, ""));

                processor.docs.splice(docid, 0, `## ${ fqn.replaceAll("_", "\\_") }\n`);

                continue;
            }

            const namespaces = [];
            useNamespaces(namespaces, "push", processor, coclass);

            const contentRegisterClass = [];

            if (coclass.isStatic()) {
                if (path.length !== 0) {
                    contentRegister.push(`lua_rawget_create_if_nil(L, -1, { ${ path.map(part => JSON.stringify(part)).join(", ") } }); // push static class table`);
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
                    contentRegisterClass.push(`lua_rawget_create_if_nil(L, -1, { ${ path.slice(0, -1).map(part => JSON.stringify(part)).join(", ") } });  // push parent table`);
                }

                contentRegisterClass.push(`lua_register_class<::${ ascendants.get(fqn).join(", ::") }>(L, "${ name }");`);

                for (const parent of ascendants.get(fqn).slice(1)) {
                    contentRegisterClass.push(`
                        usertype_info<::${ parent }>::derives_pushers[typeid(${ fqn }).hash_code()] = std::move([] (lua_State* L, const std::shared_ptr<::${ parent }>& ptr) {
                            lua_push(L, std::reinterpret_pointer_cast<${ fqn }>(ptr));
                        });
                    `.replace(/^ {24}/mg, "").trim());
                }

                if (path.length > 1) {
                    contentRegisterClass.push("lua_pop(L, 1); // pop parent table");
                }

                contentRegister.push(`lua_rawget_create_if_nil(L, -1, { ${ path.map(part => JSON.stringify(part)).join(", ") } }); // push class metatable`);

                if (ascendants.get(fqn).length > 1) {
                    registerInherits.push(`lua_inherit<::${ ascendants.get(fqn).join(", ::") }>(L);`);
                }

                registerDefaults.push(`lua_register_defaults<${ fqn }>(L);`);
            }

            const contentRegisterThis = [""];

            // push this properties parent
            if (!coclass.isStatic() || path.length !== 0) {
                contentRegisterThis.unshift(contentRegister[0]);
            }

            const { getters, setters, hasThisProperties } = LuaGenerator.writeProperties(processor, coclass, { ascendants }, contentRegisterPrivate, contentRegister, contentRegisterThis, options);

            if (hasThisProperties) {
                const index = 1 + (coclass.isStatic() ? 0 : 1);
                registers.splice(index, 0, `void ${ registerThisFn }(lua_State* L);`);
                registerThises.push(`${ registerThisFn }(L);`);
            }

            if (!coclass.isStatic() || coclass.properties.size !== 0 && coclass.methods.size !== 0) {
                // new line
                contentRegisterPrivate.push("");
                contentRegister.push("");
            }

            LuaGenerator.writeMethods(processor, coclass, { getters, setters }, contentRegisterPrivate, contentRegister, contentDecl, options);

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

            if (!coclass.isStatic() || path.length !== 0) {
                contentRegisterThis.push(contentRegister.at(-1));
            }

            if (!coclass.isStatic()) {
                contentCpp.push(`
                    namespace LUA_MODULE_NAME {
                        void ${ registerClassFn }(lua_State* L) {
                            ${ contentRegisterClass.join("\n").split("\n").join(`\n${ " ".repeat(28) }`) }
                        }
                    }
                `.replace(/^ {20}/mg, "").trim(), "");
            }

            contentCpp.push(`
                namespace LUA_MODULE_NAME {
                    ${ contentDecl.join("\n").split("\n").join(`\n${ " ".repeat(20) }`) }

                    void ${ registerFn }(lua_State* L) {
                        ${ contentRegister.join("\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                    }
                }
            `.replace(/^ {16}/mg, "").trim());
            contentCpp.push("");

            if (hasThisProperties) {
                contentCpp.push(`
                    namespace LUA_MODULE_NAME {
                        void ${ registerThisFn }(lua_State* L) {
                            ${ contentRegisterThis.join("\n").split("\n").join(`\n${ " ".repeat(28) }`) }
                        }
                    }
                `.replace(/^ {20}/mg, "").trim(), "");
                contentCpp.push("");
            }

            LuaGenerator.setFileHeader(fileHdr, registers, files, options);
            files.set(sysPath.join(options.output, fileCpp), contentCpp.join("\n").replace(/^[^\S\r\n]+$/mg, ""));

            processor.docs.splice(docid, 0, `## ${ fqn.replaceAll("_", "\\_") }\n`);
        }

        processor.definitions.push(...drefs);

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
                    ${ [
                        `// ${ "=".repeat(64) }`,
                        "// classes",
                        `// ${ "=".repeat(64) }`,
                        ...registerClasses,

                        "", `// ${ "=".repeat(64) }`,
                        "// properties and methods",
                        `// ${ "=".repeat(64) }`,
                        ...registrations,

                        "", `// ${ "=".repeat(64) }`,
                        "// heritance",
                        `// ${ "=".repeat(64) }`,
                        ...registerInherits,

                        "", `// ${ "=".repeat(64) }`,
                        "// default properties and methods",
                        `// ${ "=".repeat(64) }`,
                        ...registerDefaults,

                        "", `// ${ "=".repeat(64) }`,
                        "// this properties",
                        `// ${ "=".repeat(64) }`,
                        ...registerThises,
                    ].join(`\n${ " ".repeat(20) }`) }
                }
            }
        `.replace(/^ {12}/mg, "").trim().replace(/[^\S\n]+$/mg, ""));

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

        const module_name = LuaGenerator.getModuleName(options);
        const definitions = sysPath.resolve(options.output, `${ module_name }.d.lua`);

        processor.definitions.unshift("");
        processor.definitions.unshift(`
            ---@meta
            ---@diagnostic disable: inject-field

            ${ Array.from(aliases.entries()).map(([name, type]) => `---@alias ${ name } ${ type }`).join(`\n${ " ".repeat(12) }`) }

            ---@class Array<T>: { [integer]: T }

            ---@class ${ module_name } : table
            local ${ module_name } = {}

            --- Returns the underlying lightuserdata pointer
            ---@param ptr any
            ---@return lightuserdata
            function ${ module_name }.__self(ptr) end
        `.replace(/^ {12}/mg, "").trim());

        if (options.definitions) {
            processor.definitions.push(...options.definitions);
        }

        processor.definitions.push(`return ${ module_name }`);

        files.set(definitions, processor.definitions.join("\n"));

        waterfall([
            next => {
                if (typeof options.beforeWriteFiles === "function") {
                    options.beforeWriteFiles(processor, files, options, next);
                } else {
                    next();
                }
            },

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
