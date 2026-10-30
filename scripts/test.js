const process = require("node:process");
const fs = require("node:fs");
const os = require("node:os");
const { spawn, spawnSync } = require("node:child_process");
const sysPath = require("node:path");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const { explore } = require("fs-explorer");
const {findFile} = require("../generator/FileUtils");

const batchSuffix = os.platform() === "win32" ? ".bat" : "";
const exeSuffix = os.platform() === "win32" ? ".exe" : "";
const WORKSPACE_ROOT = process.env.WORKSPACE_ROOT ? sysPath.resolve(process.env.WORKSPACE_ROOT) : sysPath.resolve(__dirname, "..");
const LUAROCKS_BINDIR = process.env.LUAROCKS_BINDIR ? sysPath.resolve(process.env.LUAROCKS_BINDIR) : sysPath.join(WORKSPACE_ROOT, "luarocks");
const luarocks = sysPath.join(LUAROCKS_BINDIR, `luarocks${ batchSuffix }`);

const spawnShellSync = (cmd, args, options = {}) => {
    // https://nodejs.org/docs/latest-v18.x/api/child_process.html#spawning-bat-and-cmd-files-on-windows
    if (os.platform() === "win32" && (cmd.endsWith(".bat") || cmd.endsWith(".cmd"))) {
        if (cmd.includes(" ")) {
            cmd = `"${ cmd }"`;
        }
        options.shell = true;
    }

    return spawnSync(cmd, args, options);
};

const lua_interpreter = spawnShellSync(luarocks, ["config", "lua_interpreter"]).stdout.toString().trim();
const LUA_BINDIR = spawnShellSync(luarocks, ["config", "variables.LUA_BINDIR"]).stdout.toString().trim();
const LUA_BINDIR_DEBUG = LUA_BINDIR.replace("Release", "Debug");
const ABIVER = spawnShellSync(luarocks, ["config", "lua_version"]).stdout.toString().trim();
const LUA_MODULES = sysPath.join(LUAROCKS_BINDIR, "lua_modules");

const PROJECT_DIR = WORKSPACE_ROOT;
const opencv_SOURCE_DIR = (() => {
    const platform = os.platform() === "win32" ? (/cygwin/.test(process.env.HOME) ? "Cygwin" : "x64") : "*-GCC";
    for (const buildDir of [
        process.env.CMAKE_CURRENT_BINARY_DIR,
        `out/build/${ platform }-*`,
        "build.luarocks",
        ".",

    ]) {
        if (!buildDir) {
            continue;
        }

        for (const hint of [`${ buildDir }/opencv/opencv-src`, `${ buildDir }/opencv`]) {
            const file = findFile(`${ hint }/modules/core/CMakeLists.txt`, PROJECT_DIR);
            if (file) {
                return sysPath.resolve(file).split(sysPath.sep).slice(0, -3).join(sysPath.sep);
            }
        }
    }

    return null;
})();

const SAMPLES_PATH = sysPath.join(WORKSPACE_ROOT, "samples");
const PYTHON_VENV_PATH = process.env.PYTHON_VENV_PATH ? sysPath.resolve(process.env.PYTHON_VENV_PATH) : sysPath.join(SAMPLES_PATH, ".venv");
const PYTHON = sysPath.join(PYTHON_VENV_PATH, os.platform() === "win32" ? "Scripts" : "bin", `python${ exeSuffix }`);
const PYTHONPATH = sysPath.join(opencv_SOURCE_DIR, "samples", "dnn");

const LOVE = process.env.LOVE_BIN_PATH ? sysPath.resolve(process.env.LOVE_BIN_PATH) : "love";

const OPENCV_SAMPLES_DATA_PATH = sysPath.join(opencv_SOURCE_DIR, "samples", "data");

const config = {
    Debug: {
        exe: sysPath.join(LUA_BINDIR_DEBUG, lua_interpreter),
        argv: ["-l", "setup"],
        env: {
            PYTHONPATH,
            OPENCV_SAMPLES_DATA_PATH,
        },
    },
    Release: {
        exe: sysPath.join(LUAROCKS_BINDIR, `lua${ batchSuffix }`),
        argv: ["-l", "setup"],
        env: {
            PYTHONPATH,
            OPENCV_SAMPLES_DATA_PATH,
        },
    },
};

const unixPath = path => {
    return path.replaceAll("\\", "/").replace(/^(\w+):/, (match, drive) => {
        return `/${ drive.toLowerCase() }`;
    });
};

const unixEscape = (arg, verbatim = true) => {
    if (verbatim && os.platform() === "win32" && arg[0] === "/" && arg[1] !== "/") {
        arg = `/${ arg }`;
    }

    if (/[^\w/=.-]/.test(arg)) {
        arg = `'${ arg.replaceAll("'", "'\\''") }'`;
    }

    return arg;
};

const unixEnv = (key, value) => {
    if (os.platform() === "win32" && key === "PATH") {
        value = value.split(sysPath.delimiter).map(unixPath).join(":");
    }
    return `${ key }=${ unixEscape(value, false) }`;
};

const unixCmd = argv => {
    return argv.map(arg => unixEscape(arg)).join(" ");
};

const run = (file, env, options, next) => {
    const { BUILD_TYPE, OPENCV_BUILD_TYPE } = options.env;
    if (BUILD_TYPE && BUILD_TYPE !== env.BUILD_TYPE || OPENCV_BUILD_TYPE && OPENCV_BUILD_TYPE !== env.OPENCV_BUILD_TYPE) {
        next(null, 0);
        return;
    }

    const keys = Array.from(new Set([...Object.keys(config[env.BUILD_TYPE].env), ...Object.keys(env)]));
    env = Object.assign({}, process.env, config[env.BUILD_TYPE].env, options.env, env);

    const extname = sysPath.extname(file);

    const args = [];

    if (extname === ".lua") {
        const argv = [...config[env.BUILD_TYPE].argv, file, ...options.argv];
        if (sysPath.basename(file) === "object_detection.lua" && options.argv.length === 0) {
            argv.push("ssd_tf", "--input", "vtest.avi");
        }
        args.push(config[env.BUILD_TYPE].exe, argv);
    } else if (extname === ".py") {
        const argv = [file, ...options.argv];
        if (sysPath.basename(file) === "object_detection.py" && options.argv.length === 0) {
            argv.push("ssd_tf", "--input", "vtest.avi");
        }
        args.push(PYTHON, argv);
    } else if (extname === ".love") {
        const game = file.slice(0, -".love".length);

        const local_data = sysPath.resolve(options.cwd, game, "local_data");
        if (!fs.existsSync(local_data)) {
            fs.symlinkSync(sysPath.join(SAMPLES_PATH, "data"), local_data, "junction");
        }

        const opencv_data = sysPath.resolve(options.cwd, game, "opencv_data");
        if (!fs.existsSync(opencv_data)) {
            fs.symlinkSync(OPENCV_SAMPLES_DATA_PATH, opencv_data, "junction");
        }

        const argv = [game, ...options.argv];
        args.push(LOVE, argv);
    } else {
        throw new Error(`Unsupported extenstion ${ extname } for file ${ file }`);
    }

    const cmd = [keys.map(key => unixEnv(key, env[key])).join(" "), unixCmd(args.flat())].join(" ");

    if (options.bash) {
        console.log(cmd);
        next(null, 0);
        return;
    }

    console.log(cmd);

    const opts = {
        stdio: options.stdio,
        env,
        cwd: options.cwd,
    };

    // https://nodejs.org/docs/latest-v18.x/api/child_process.html#spawning-bat-and-cmd-files-on-windows
    if (os.platform() === "win32" && (args[0].endsWith(".bat") || args[0].endsWith(".cmd"))) {
        if (args[0].includes(" ")) {
            args[0] = `"${ args[0] }"`;
        }
        opts.shell = true;
    }

    const child = spawn(...args, opts);

    child.on("error", err => {
        if (next !== null) {
            next(err);
            next = null;
        }
    });

    child.on("close", (code, signal) => {
        if (next !== null) {
            next(code, signal);
            next = null;
        }
    });

    if (typeof options.run === "function") {
        options.run(child);
    }
};

// Source - https://unix.stackexchange.com/questions/799584/bash-exception-handling-with-a-stack-trace-and-cleanup-code-how-to-implement#answer-799585
// Posted by user3207874.
// Retrieved 2026-09-12, License - CC BY-SA 4.0

const bash_init = `
#!/usr/bin/env bash

set -Eeuo pipefail

trap 'catch' ERR

catch() {
    local exit_code=$?
    local cmd="$BASH_COMMAND"
    local i

    # Check if exit was due to error
    if [ $exit_code -ne 0 ]; then
        echo ""
        echo "[-] ERROR $exit_code during command: $cmd"
        echo "Call stack:"
        for ((i=1; i<\${#FUNCNAME[@]}; i++)); do
            local func="\${FUNCNAME[$i]}"
            local src="\${BASH_SOURCE[$i]}"
            local line="\${BASH_LINENO[$((i - 1))]}"

            # Added to track nested function calls from functions
            # Extract the actual line of code (skip if file inaccessible)
            if [[ -r "$src" ]]; then
                local code_line
                code_line=$(sed -n "\${line}s/^[[:space:]]*/    /p" "$src")
                echo "  at $src:$line in function $func:"
                echo "$code_line"
            else
                echo "  at $src:$line in function $func (source not readable)"
            fi
        done

        # echo "Tip: You can put any exception handling code here"
    fi
}

`.trim();

const runFile = (file, options, cb) => {
    waterfall([
        next => {
            run(file, {
                BUILD_TYPE: "Release",
                OPENCV_BUILD_TYPE: "Release",
            }, options, next);
        },

        (signal, next) => {
            run(file, {
                BUILD_TYPE: "Debug",
                OPENCV_BUILD_TYPE: "Debug",
            }, options, next);
        },
    ], (code, signal) => {
        cb(code);
    });
};

const main = (options, cb) => {
    options = Object.assign({
        cwd: WORKSPACE_ROOT,
        includes: [],
        includes_ext: [".lua"],
        excludes: [],
        argv: [],
        stdio: "inherit",
    }, options);

    const { cwd, includes, includes_ext, excludes } = options;

    excludes.push(...["init.lua", "common.lua", "download_model.py"]);

    if (options.bash) {
        console.log([
            bash_init,
            "",
            `cd ${ unixEscape(unixPath(cwd), false) }`,
            "",
        ].join("\n"));
    }

    const runs = new Set();

    waterfall([
        next => {
            eachOfLimit(["samples"], 1, (directory, i, next) => {
                explore(sysPath.join(cwd, directory), (path, stats, next) => {
                    const file = sysPath.relative(cwd, path);
                    const basename = sysPath.basename(file);
                    const extname = sysPath.extname(file);

                    if (
                        (includes.length === 0 || !includes.some(include => basename.startsWith(include))) && [ "_", "." ].includes(basename[0]) ||
                        !includes_ext.includes(extname) ||
                        excludes.some(exclude => basename.startsWith(exclude)) ||
                        includes.length !== 0 && !includes.some(include => basename.includes(include) || file.replaceAll("\\", "/") === include.replaceAll("\\", "/"))
                    ) {
                        next();
                        return;
                    }

                    runs.add(file.replaceAll("\\", "/"));
                    runFile(file, options, next);
                }, (path, stats, files, state, next) => {
                    const basename = sysPath.basename(path);

                    if (state === "begin" && basename === "love2d" && (includes.length === 0 || includes.some(include => basename.includes(include)))) {
                        const file = sysPath.relative(cwd, path);
                        runFile(`${ file }.love`, options, err => {
                            next(err, true);
                        });
                    } else {
                        const skip = state === "begin" && (basename[0] === "." || basename === "BackUp");
                        next(null, skip);
                    }
                }, err => {
                    if (err && err.code === "ENOENT") {
                        err = null;
                    }
                    next(err);
                });
            }, next);
        },
        next => {
            eachOfLimit(includes, 1, (file, i, next) => {
                if (fs.existsSync(file) && !fs.lstatSync(file).isDirectory() && !runs.has(file.replaceAll("\\", "/"))) {
                    runFile(file, options, next);
                } else {
                    next();
                }
            }, next);
        },
    ], cb);
};

exports.main = main;

if (typeof require !== "undefined" && require.main === module) {
    const options = {
        includes: [],
        excludes: [],
        argv: [],
        env: {},
        "--": 0,
    };

    for (const arg of process.argv.slice(2)) {
        if (arg === "--") {
            options[arg]++;
        } else if (arg[0] === "!") {
            options.excludes.push(arg.slice(1));
        } else if (options["--"] === 1 && arg[0] !== "-") {
            options.includes.push(arg);
        } else if (options["--"] > 1 || options["--"] === 1 && arg[0] === "-") {
            options.argv.push(arg);
        } else if (["--Debug", "--Release"].includes(arg)) {
            options.env.BUILD_TYPE = arg.slice(2);
        } else if (arg === "--bash") {
            options[arg.slice(2)] = true;
        } else {
            options.cwd = sysPath.resolve(arg);
            options["--"] = 1;
        }
    }

    main(options, err => {
        if (err) {
            if (!Array.isArray(err)) {
                throw err;
            }

            const code = err.flat(Infinity)[0];
            if (typeof code !== "number") {
                throw code;
            }

            process.exitCode = code;
        }
    });
}
