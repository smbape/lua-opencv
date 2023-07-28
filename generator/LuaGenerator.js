/* eslint-disable no-magic-numbers */
const version = process.env.npm_package_version || require("../package.json").version;
const fs = require("node:fs");
const sysPath = require("node:path");
const waterfall = require("async/waterfall");
const { convertExpression } = require("node-autoit-binding-utils/src/autoit-expression-converter");

const [VERSION_MAJOR, VERSION_MINOR] = version.split(".");

const knwon_ids = require("./ids");
const FileUtils = require("./FileUtils");

const proto = {
    // No extension
};

class LuaGenerator {
    static proto = proto;

    generate(processor, configuration, options, cb) {
        const { generated_include } = configuration;
        const { APP_NAME } = options;

        const files = new Map();

        for (const fqn of processor.classes.keys()) {
            // const coclass = processor.classes.get(fqn);

            const docid = processor.docs.length;

            // TODO /^cv::(?:Point|Rect|Scalar|Size|Vec)(?:\d[bdfisw])?$/

            // using namespace

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
            //     /External binding is external

            // TODO methods
            // constructor :
            //     /CO
            // function modifiers:
            //     /idlname= exported name
            //     /attr=propget
            //     /attr=propput
            //     instance call "/attr=propget", "/id=DISPID_VALUE"
            //     /Cast= cast return expr
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

        if (options.hdr !== false) {
            files.set(
                sysPath.join(options.output, "lua_generated_pch.hpp"),
                [
                    "#pragma once\n",
                    "#include <registration.hpp>\n",
                    ...generated_include
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
