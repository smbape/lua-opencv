/* eslint-disable no-magic-numbers */
const version = process.env.npm_package_version || require("../package.json").version;
const fs = require("node:fs");
const sysPath = require("node:path");
const waterfall = require("async/waterfall");
const { convertExpression } = require("node-autoit-binding-utils/src/autoit-expression-converter");

const [VERSION_MAJOR, VERSION_MINOR] = version.split(".");

const knwon_ids = require("./ids");
const FileUtils = require("./FileUtils");
const {makeExpansion, useNamespaces, removeNamespaces} = require("./alias");

const proto = {
    // No extension
};

class LuaGenerator {
    static proto = proto;

    static getRegisterFn(coclass) {
        return `register_${ coclass.getClassName() }`;
    }

    generate(processor, configuration, options, cb) {
        const { generated_include } = configuration;
        const { APP_NAME } = options;

        const files = new Map();
        const registrationsHdr = [];
        const registrations = [];

        for (const fqn of Array.from(processor.classes.keys()).sort()) {
            if (![
                "cv",
                "cv::Mat",
                "cv::UMat",
                "cv::MediaFormat",
            ].includes(fqn)) {
                continue;
            }

            const docid = processor.docs.length;

            const coclass = processor.classes.get(fqn);
            const fileCpp = coclass.getCPPFileName(options);
            const fileHdr = `${ fileCpp.slice(0, -".cpp".length) }.hpp`;
            const registerFn = LuaGenerator.getRegisterFn(coclass);

            registrationsHdr.push(`#include <${ fileHdr }>`);
            registrations.push(`${ registerFn }(lua, module);`);

            files.set(sysPath.join(options.output, fileHdr), `
                #pragma once
                #include <lua_generated_pch.hpp>

                namespace LUA_MODULE_NAME {
                    void ${ registerFn }(sol::state_view& lua, sol::table& module);
                }
            `.replace(/^ {16}/mg, "").trim());

            const contentCpp = [`#include <${ fileHdr }>\n`];
            const contentRegisterPrivate = [];
            const contentRegister = [];

            let ns = "module";
            for (let i = 0; i < coclass.path.length - 1; i++) {
                const parent = processor.classes.get(coclass.path.slice(0, i + 1).join("::"));
                ns = `${ ns }["${ parent.name }"]`;
            }

            if (coclass.path.length > 1) {
                const parent = processor.classes.get(coclass.path.slice(0, -1).join("::"));

                if (parent.isStatic()) {
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

            const dynamic_set = [];
            const dynamic_get = [];

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

            for (const [name, property] of coclass.properties.entries()) {
                const {type, value, modifiers} = property;
                const cpptype = processor.getCppType(type, coclass, options);
                const is_static = modifiers.includes("/S") || coclass.isStatic();
                const obj = `${ is_static ? `${ fqn }::` : "self." }`;
                const enum_fqn = processor.getEnumType(type, coclass, options);
                const is_class = !enum_fqn || processor.classes.get(enum_fqn).is_enum_class;
                const as_type = is_class ? enum_fqn || type : "int";

                let propname = name;
                let getter, setter;
                let has_propget = is_static || modifiers.includes("/Enum") || modifiers.includes("/R") || modifiers.includes("/RW");
                let has_propput = modifiers.includes("/W") || modifiers.includes("/RW");

                for (const modifier of modifiers) {
                    if (modifier[0] === "=") {
                        propname = modifier.slice(1);
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

                if (has_propget) {
                    const shared_ptr = removeNamespaces(options.shared_ptr, options);
                    const is_ptr = type.endsWith("*");
                    const is_shared_ptr = cpptype.startsWith(`${ shared_ptr }<`);
                    const is_std = cpptype.startsWith("std::");
                    const is_by_ref = !is_ptr && !is_shared_ptr && (is_std || processor.classes.has(cpptype));
                    let has_expr = false;

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

                    if (is_class) {
                        // input must be enum_qn or type
                        in_val = `object_as<${ as_type }>(value)`;
                    } else {
                        // input must be int and cast as enum
                        in_val = `static_cast<${ enum_fqn }>(object_as<${ as_type }>(value))`;
                    }

                    in_val = setter ? `${ setter }(${ in_val })` : `${ obj }${ propname } = ${ in_val }`;

                    wexpr = `
                        if (object_is<${ as_type }>(value)) {
                            ${ in_val };
                        } else {
                            sol::state_view lua(ts);
                            luaL_error(lua.lua_state(), "Unexpected value type");
                        }
                    `.replace(/^ {24}/mg, "").trim();
                }

                if (coclass.isStatic()) {
                    dynamic_get.push(`
                        { "${ name }", [] (sol::this_state& ts) {
                            return sol::object(ts, sol::in_place, ${ rexpr });
                        }}
                    `.replace(/^ {24}/mg, "").trim());

                    if (wexpr) {
                        dynamic_set.push(`
                            { "${ name }", [] (sol::stack_object& value, sol::this_state& ts) {
                                ${ wexpr.split("\n").join(`\n${ " ".repeat(32) }`) }
                            }}
                        `.replace(/^ {24}/mg, "").trim());
                    }

                    // dynamic set
                    // dynamic get
                    // module["CV_8UC1"] = CV_8UC1;
                } else {
                    if (rexpr === `self.${ propname }` && in_val === `self.${ propname } = object_as<${ type }>(value)`) {
                        contentRegister.push(`exports["${ name }"] = &::${ fqn }::${ propname };`);
                    } else {
                        const getter_decl = !is_static ? `[] (::${ fqn }& self)` : "[]";
                        const getter_setter = [`
                            ${ getter_decl } {
                                return ${ rexpr };
                            }
                        `.replace(/^ {28}/mg, "").trim()];

                        if (wexpr) {
                            const setter_args = [];

                            if (!is_static) {
                                setter_args.push(`::${ fqn }& self`);
                            }

                            setter_args.push(...[
                                "sol::stack_object value",
                                "sol::this_state ts",
                            ]);

                            getter_setter.push(`
                                [] (${ setter_args.join(", ") }) {
                                    ${ wexpr.split("\n").join(`\n${ " ".repeat(36) }`) }
                                }
                            `.replace(/^ {32}/mg, "").trim());
                        }

                        contentRegister.push(`
                            exports["${ name }"] = sol::property(${ getter_setter.join(", ").split("\n").join(`\n${ " ".repeat(28) }`) });
                        `.replace(/^ {28}/mg, "").trim());
                        contentRegister.push("");
                    }
                }
            }

            if (coclass.isStatic()) {
                // https://github.com/ThePhD/sol2/blob/develop/examples/source/usertype_dynamic_get_set.cpp
                contentRegisterPrivate.push(`
                    std::map<std::string, std::function<sol::object(sol::this_state&)>> getters({
                        ${ dynamic_get.join(",\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                    });

                    sol::object dynamic_get(sol::stack_object ns, sol::stack_object key, sol::this_state ts) {
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

                        sol::state_view lua(ts);
                        std::stringstream err; err << "AttributeError: module '${ fqn.replaceAll("::", ".") }' has no attribute '" << (maybe_string_key ? *maybe_string_key : "") << "'";
                        luaL_error(lua.lua_state(), err.str().c_str());

                        return sol::object(ts, sol::in_place, sol::lua_nil);
                    }

                    std::map<std::string, std::function<void(sol::stack_object&, sol::this_state&)>> setters({
                        ${ dynamic_set.join(",\n").split("\n").join(`\n${ " ".repeat(24) }`) }
                    });

                    void dynamic_set(sol::stack_object ns, sol::stack_object key, sol::stack_object value, sol::this_state ts) {
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

                        sol::state_view lua(ts);
                        std::stringstream err; err << "AttributeError: module '${ fqn.replaceAll("::", ".") }' has no attribute '" << (maybe_string_key ? *maybe_string_key : "") << "'";
                        luaL_error(lua.lua_state(), err.str().c_str());
                    }
                `.replace(/^ {20}/mg, "").trim());
            }

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

            // TODO /^cv::(?:Point|Rect|Scalar|Size|Vec)(?:\d[bdfisw])?$/

            // using namespace
            // in shared_ptr => reference pointer
            // multi return as std::make_tuple

            // TODO properties
            // property/method, cast in/out arg as enum/int if cpptype is enum
            // property modifiers:
            //     = exported name
            //     /idlname= exported name
            //     /RW=property
            //     /R= getter
            //     /RExpr= return expr, $0 with the current return expr
            //     /W= setter
            //     /WExpr= set expression, $0 with the current fqn setter or set expr
            //     /Cast= cast out val
            //     if ::this return module/usertype
            //     pointer to shared_ptr
            //     /S class static property
            //     /Enum is enum value
            //     /ExternalNoDecl binding is external and do not declare it in header glue
            //
            //      binding is external

            // TODO methods
            // constructor :
            //     /CO
            // function modifiers:
            //     /idlname= exported name
            //     /attr=propget
            //     /attr=propput
            //     instance call "/attr=propget", "/id=DISPID_VALUE"
            //     /Expr=
            //     /Cast= cast self before calling function on it
            //     /Prop= use prop of self before calling function on it
            //     /Call=
            //     /WrapAs= callee = `${ modifier.slice("/WrapAs=".length) }(${ callee })`;
            //     /Output= expression to return ; callee = makeExpansion(modifier.slice("/Output=".length), callee);
            //     /Body= expression that has a return; callee = makeExpansion(modifier.slice("/Body=".length), callee);
            // argument modifiers:
            //     /IO, /I, /O in/out how?
            //     /Cast= cast arg
            //     /Expr= arg expr, replace
            //     /RRef std::move(${ callarg })

            // if (argtype === "InputArray") {
            //     is_array = true;
            //     arrtype = "InputArray";
            //     argtype = "Mat";
            // } else if (argtype === "InputOutputArray") {
            //     is_array = true;
            //     arrtype = "InputOutputArray";
            //     argtype = "Mat";
            // } else if (argtype === "OutputArray") {
            //     is_array = true;
            //     arrtype = "OutputArray";
            //     argtype = "Mat";
            // } else if (argtype === "InputArrayOfArrays") {
            //     is_array = true;
            //     arrtype = "InputArray";
            //     argtype = "vector_Mat";
            // } else if (argtype === "InputOutputArrayOfArrays") {
            //     is_array = true;
            //     arrtype = "InputOutputArray";
            //     argtype = "vector_Mat";
            // } else if (argtype === "OutputArrayOfArrays") {
            //     is_array = true;
            //     arrtype = "OutputArray";
            //     argtype = "vector_Mat";
            // }

            // if (is_array) {
            //     defval = defval
            //         .replace("InputArrayOfArrays", "std::vector<cv::Mat>")
            //         .replace("InputOutputArrayOfArrays", "std::vector<cv::Mat>")
            //         .replace("OutputArrayOfArrays", "std::vector<cv::Mat>")
            //         .replace("InputArray", "Mat")
            //         .replace("InputOutputArray", "Mat")
            //         .replace("OutputArray", "Mat")
            //         .replace("noArray", argtype);
            // }

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
                sysPath.join(options.output, "lua_generated_pch.hpp"),
                [
                    "#pragma once\n",
                    "#include <registration.hpp>\n",
                    "#include <register_all.hpp>\n",
                    ...generated_include
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

    genManifest(comClasses, comInterfaceExternalProxyStubs, debugPostFix, options) {
        const { LIB_UID, OUTPUT_NAME, updateAssembly } = options;

        const assemblies = [`
            <assemblyIdentity
                type="win32"
                name="${ OUTPUT_NAME }${ debugPostFix }.sxs"
                version="${ version }.0" />
        `.replace(/^ {12}/mg, "").trim()];

        if (typeof updateAssembly === "function") {
            updateAssembly(assemblies, debugPostFix, options);
        }

        return `
            <?xml version="1.0" encoding="UTF-8" standalone="yes"?>
            <assembly xmlns="urn:schemas-microsoft-com:asm.v1" manifestVersion="1.0">
                ${ assemblies.join("\n").split("\n").join(`\n${ " ".repeat(16) }`) }

                <file xmlns="urn:schemas-microsoft-com:asm.v1" name="${ OUTPUT_NAME }${ debugPostFix }.dll">
                    ${ comClasses.map(comClass => comClass.split("\n").join(`\n${ " ".repeat(20) }`)).join(`\n\n${ " ".repeat(20) }`) }

                    <typelib tlbid="{${ LIB_UID }}"
                        version="${ VERSION_MAJOR }.${ VERSION_MINOR }"
                        helpdir="." />
                </file>

                ${ comInterfaceExternalProxyStubs.map(comClass => comClass.split("\n").join(`\n${ " ".repeat(16) }`)).join(`\n\n${ " ".repeat(16) }`) }
            </assembly>
        `.replace(/^ {12}/mg, "").trim();
    }
}

module.exports = LuaGenerator;
