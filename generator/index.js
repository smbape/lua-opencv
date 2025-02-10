/* eslint-disable no-magic-numbers */

const fs = require("node:fs");
const fsPromises = require("node:fs/promises");
const sysPath = require("node:path");
const { spawn } = require("node:child_process");
const os = require("node:os");

const { mkdirp } = require("mkdirp");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const { explore } = require("fs-explorer");

const OpenCV_VERSION = "opencv-4.10.0";
// const OpenCV_DLLVERSION = OpenCV_VERSION.slice("opencv-".length).replaceAll(".", "");
global.OpenCV_VERSION = OpenCV_VERSION;

const Python3_EXECUTABLE = process.env.Python3_EXECUTABLE ? process.env.Python3_EXECUTABLE : "python";

const getOptions = output => {
    const options = {
        APP_NAME: "OpenCV",
        language: "lua",
        namespace: "cv",
        implicitNamespaceType: /^(?:Point|Rect|Scalar|Size|Vec)(?:\d[bdfisw])?$/,
        variantTypeReg: /(?:<cv::Ptr)/,
        shared_ptr: "cv::Ptr",
        make_shared: "cv::makePtr",
        exception: "cv::Exception",
        Any: "Object",
        AnyObject: "Object",
        cname: "new",

        isCaseSensitive: true,
        hasIsInstanceSupport: true, // do not generate reflection methods and properties
        hasInheritanceSupport: true, // do not duplicate parent methods

        // used to lookup classes
        namespaces: new Set([]),

        other_namespaces: new Set(),

        // used to reduce class name length
        remove_namespaces: new Set([
            "cv",
            "std",
        ]),

        self: "*self",
        self_get: (name = null) => {
            return name ? `self->${ name }` : "self";
        },

        build: new Set(),
        notest: new Set(),
        skip: new Set(),

        output: sysPath.join(output, "generated"),

        onClass: (processor, coclass, opts) => {
            // Nothing to do
        },

        onCoClass: (processor, coclass, opts) => {
            // Nothing to do
        },

        vectors: new Set([
            "std::vector<std::variant<int, cv::Range>>",
        ]),
    };

    const argv = process.argv.slice(2);
    const flags_true = ["hdr", "impl", "save"];
    const flags_false = ["test"];

    for (const opt of flags_true) {
        options[opt] = !argv.includes(`--no-${ opt }`);
    }

    for (const opt of flags_false) {
        options[opt] = argv.includes(`--${ opt }`);
    }

    for (let i = 0; i < argv.length; i++) {
        const opt = argv[i];

        if (opt.startsWith("--no-") && flags_true.includes(opt.slice("--no-".length))) {
            continue;
        }

        if (opt.startsWith("--") && flags_false.includes(opt.slice("--".length))) {
            continue;
        }

        if (opt.startsWith("--no-test=")) {
            for (const fqn of opt.slice("--no-test=".length).split(/[ ,]/)) {
                options.notest.add(fqn);
            }
            continue;
        }

        if (opt.startsWith("--build=")) {
            for (const fqn of opt.slice("--build=".length).split(/[ ,]/)) {
                options.build.add(fqn);
            }
            continue;
        }

        if (opt.startsWith("--skip=")) {
            for (const fqn of opt.slice("--skip=".length).split(/[ ,]/)) {
                options.skip.add(fqn);
            }
            continue;
        }

        if (opt.startsWith("-D")) {
            const [key, value] = opt.slice("-D".length).split("=");
            options[key] = typeof value === "undefined" ? true : value;
            continue;
        }

        throw new Error(`Unknown option ${ opt }`);
    }

    return options;
};

const {
    CUSTOM_CLASSES,
} = require("./constants");
const {findFile} = require("./FileUtils");
const custom_declarations = require("./custom_declarations");
const DeclProcessor = require("./DeclProcessor");
const LuaGenerator = require("./LuaGenerator");

const PROJECT_DIR = sysPath.dirname(__dirname);
const SRC_DIR = sysPath.join(PROJECT_DIR, "src");

const findSourceDir = name => {
    const platform = os.platform() === "win32" ? (/cygwin/.test(process.env.HOME) ? "Cygwin" : "x64") : "*-GCC";

    const hints = [
        `out/build/${ platform }-*`,
        "build.luarocks",
    ];

    if (process.env.CMAKE_BINARY_DIR) {
        hints.unshift(process.env.CMAKE_BINARY_DIR);
    }

    if (process.env.OpenCV_DIR) {
        const { OpenCV_DIR } = process.env;
        const parts = OpenCV_DIR.split(/[\\/]/);
        const pos = parts.indexOf("lua_modules");
        const build = sysPath.resolve(sysPath.dirname(parts.slice(0, pos).join(sysPath.sep)), "build.luarocks");
        const hint = sysPath.relative(PROJECT_DIR, build);
        hints.unshift(hint);
    }

    for (const hint of hints) {
        const file = findFile(`${ hint }/${ name }`, PROJECT_DIR);
        if (file) {
            return file;
        }
    }

    return null;
};

const opencv_SOURCE_DIR = findSourceDir("opencv/opencv-src");

const python_bindings_generator = sysPath.join(sysPath.dirname(opencv_SOURCE_DIR), "opencv-build", "modules", "python_bindings_generator");
const src2 = sysPath.resolve(opencv_SOURCE_DIR, "modules/python/src2");
const headers = sysPath.join(python_bindings_generator, "headers.txt");
const pyopencv_generated_include = sysPath.join(python_bindings_generator, "pyopencv_generated_include.h");

const hdr_parser = fs.readFileSync(sysPath.join(src2, "hdr_parser.py")).toString();
const hdr_parser_start = hdr_parser.indexOf("class CppHeaderParser");
const hdr_parser_end = hdr_parser.indexOf("if __name__ == '__main__':");

const options = getOptions(PROJECT_DIR);
options.proto = LuaGenerator.proto;

waterfall([
    next => {
        const files = [sysPath.join(sysPath.dirname(sysPath.dirname(opencv_SOURCE_DIR)), "CMakeCache.txt"), pyopencv_generated_include];
        const results = [];
        eachOfLimit(files, files.length, (file, i, next) => {
            fs.stat(file, (err, stats) => {
                if (err && err.code === "ENOENT") {
                    err = null;
                    results[i] = null;
                } else {
                    results[i] = stats;
                }
                next(err);
            });
        }, err => {
            next(err, results);
        });
    },

    (results, next) => {
        const [statsCMakeCache, statsPyOpenCVGeneratedInclude] = results;
        if (statsCMakeCache && statsPyOpenCVGeneratedInclude) {
            next();
            return;
        }

        console.log("generating", pyopencv_generated_include);

        const child = spawn(Python3_EXECUTABLE, [sysPath.join(src2, "gen2.py"), python_bindings_generator, headers], {
            stdio: [0, "pipe", "pipe"]
        });

        child.stdout.on("data", chunk => {
            process.stdout.write(chunk);
        });

        child.stderr.on("data", chunk => {
            process.stderr.write(chunk);
        });

        child.on("close", code => {
            if (code !== 0) {
                console.log(`python process exited with code ${ code }`);
                process.exit(code);
            }
            next();
        });
    },

    next => {
        mkdirp(options.output).then(performed => {
            next();
        }, next);
    },

    next => {
        const srcfiles = [];

        explore(SRC_DIR, async (path, stats, next) => {
            if (path.endsWith(".h") || path.endsWith(".hpp")) {
                const content = await fsPromises.readFile(path);
                if (content.includes("CV_EXPORTS")) {
                    srcfiles.push(path);
                }
            }
            next();
        }, {followSymlink: true}, async err => {
            const generated_include = (await fsPromises.readFile(pyopencv_generated_include))
                .toString()
                .split("\n")
                .filter(content => !content.includes("python_bridge"))
                .map(line => line.trim())
                .concat(srcfiles.map(path => `#include "${ path.slice(SRC_DIR.length + 1).replace("\\", "/") }"`));

            next(err, srcfiles, generated_include);
        });
    },

    (srcfiles, generated_include, next) => {
        // exported class that are not included by python generated include file
        srcfiles.push(sysPath.resolve(opencv_SOURCE_DIR, "modules/flann/include/opencv2/flann/defines.h"));

        const buffers = [];
        let nlen = 0;
        const child = spawn(Python3_EXECUTABLE, []);

        child.stderr.on("data", chunk => {
            process.stderr.write(chunk);
        });

        child.on("close", code => {
            if (code !== 0) {
                console.log(`python process exited with code ${ code }`);
                process.exit(code);
            }

            const buffer = Buffer.concat(buffers, nlen);

            const configuration = JSON.parse(buffer.toString());
            configuration.decls.push(...custom_declarations.load(options));
            configuration.generated_include = generated_include;

            for (const [name, modifiers] of CUSTOM_CLASSES) {
                configuration.decls.push([`class ${ name }`, "", modifiers, [], "", ""]);
            }

            configuration.namespaces.push(...options.namespaces);
            configuration.namespaces.push(...options.other_namespaces);

            const processor = new DeclProcessor(options);
            processor.process(configuration, options);

            next(null, processor, configuration);
        });

        child.stderr.on("data", chunk => {
            process.stderr.write(chunk);
        });

        child.stdout.on("data", chunk => {
            buffers.push(chunk);
            nlen += chunk.length;
        });

        const code = `
            import io, json, os, re, string, sys

            ${ hdr_parser
                .slice(hdr_parser_start, hdr_parser_end)
                .split("\n")
                .join(`\n${ " ".repeat(12) }`) }

            with open(${ JSON.stringify(headers) }, 'r') as f:
                srcfiles = [l.strip() for l in f.readlines()]

            ${ srcfiles.map(file => `srcfiles.append(${ JSON.stringify(file) })`).join(`\n${ " ".repeat(12) }`) }

            parser = CppHeaderParser(generate_umat_decls=True, generate_gpumat_decls=True)
            all_decls = []
            for hdr in srcfiles:
                decls = parser.parse(hdr)
                if len(decls) == 0 or hdr.find('/python/') != -1:
                    continue

                all_decls += decls

            # parser.print_decls(all_decls)
            print(json.dumps({"decls": all_decls, "namespaces": sorted(parser.namespaces)}, indent=4))
        `.trim().replace(/^ {12}/mg, "");

        // fs.writeFileSync(sysPath.join(PROJECT_DIR, "gen.py"), code);

        child.stdin.write(code);
        child.stdin.end();
    },

    (processor, configuration, next) => {
        // func_modifiers = ["=readMat"]
        const generator = new LuaGenerator();
        generator.generate(processor, configuration, options, next);
    },
], err => {
    if (err) {
        throw err;
    }
    console.log(`Build files have been written to: ${ options.output }`);
});
