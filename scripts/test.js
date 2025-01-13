const process = require("node:process");
const fs = require("node:fs");
const os = require("node:os");
const { spawn, spawnSync } = require("node:child_process");
const sysPath = require("node:path");
const waterfall = require("async/waterfall");
const { explore } = require("fs-explorer");
const {findFile} = require("../generator/FileUtils");

const batchSuffix = os.platform() === "win32" ? ".bat" : "";
const exeSuffix = os.platform() === "win32" ? ".exe" : "";
const WORKSPACE_ROOT = process.env.WORKSPACE_ROOT ? sysPath.resolve(process.env.WORKSPACE_ROOT) : sysPath.resolve(__dirname, "..");
const LUAROCKS_BINDIR = process.env.LUAROCKS_BINDIR ? sysPath.resolve(process.env.LUAROCKS_BINDIR) : sysPath.join(WORKSPACE_ROOT, "luarocks");
const luarcoks = sysPath.join(LUAROCKS_BINDIR, `luarocks${ batchSuffix }`);

const lua_interpreter = spawnSync(luarcoks, ["config", "lua_interpreter"]).stdout.toString().trim();
const LUA_BINDIR = spawnSync(luarcoks, ["config", "variables.LUA_BINDIR"]).stdout.toString().trim();
const LUAROCKS_SYSCONFDIR = spawnSync(luarcoks, ["config", "sysconfdir"]).stdout.toString().trim();
const LUA_BINDIR_DEBUG = LUA_BINDIR.replace("Release", "Debug");
const ABIVER = spawnSync(luarcoks, ["config", "lua_version"]).stdout.toString().trim();
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
            const file = findFile(`${ hint }/modules/calib3d/CMakeLists.txt`, PROJECT_DIR);
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
const OPENCV_SAMPLES_DATA_PATH = sysPath.join(opencv_SOURCE_DIR, "samples", "data");

const config = {
    Debug: {
        exe: sysPath.join(LUA_BINDIR_DEBUG, lua_interpreter),
        env: {
            LUAROCKS_BINDIR,
            WORKSPACE_ROOT,
            PYTHONPATH,
            OPENCV_SAMPLES_DATA_PATH,
        },
        argv: [],
    },
    Release: {
        exe: sysPath.join(LUAROCKS_BINDIR, `lua${ batchSuffix }`),
        env: {
            LUAROCKS_BINDIR,
            WORKSPACE_ROOT,
            PYTHONPATH,
            OPENCV_SAMPLES_DATA_PATH,
        },
        argv: [],
    },
};

if (os.platform() === "win32") {
    const { APPDATA, PATH } = process.env;

    config.Release.env.PATH = `${ sysPath.join(LUAROCKS_BINDIR, "lua_modules", "bin") };${ sysPath.join(APPDATA, "luarocks", "bin") };${ PATH }`;
    config.Debug.env.LUAROCKS_SYSCONFDIR = LUAROCKS_SYSCONFDIR;

    config.Debug.env.PATH = `${ LUA_BINDIR_DEBUG };${ sysPath.join(LUAROCKS_BINDIR, "lua_modules", "bin") };${ sysPath.join(APPDATA, "luarocks", "bin") };${ PATH }`;
    config.Debug.env.LUAROCKS_SYSCONFDIR = LUAROCKS_SYSCONFDIR;

    config.Debug.argv = [
        "-e",
        [
            `package.path="${ [
                `${ LUA_MODULES }/share/lua/${ ABIVER }/?.lua`,
                `${ LUA_MODULES }/share/lua/${ ABIVER }/?/init.lua`,
                `${ APPDATA }/luarocks/share/lua/${ ABIVER }/?.lua`,
                `${ APPDATA }/luarocks/share/lua/${ ABIVER }/?/init.lua`,
            ].join(";").replace(/[\\/]/g, sysPath.sep.repeat(2)) };"..package.path`,

            `package.cpath="${ [
                `${ LUA_BINDIR_DEBUG }/?.dll`,
                `${ LUA_BINDIR_DEBUG }/loadall.dll`,
                `${ LUA_MODULES }/lib/lua/${ ABIVER }/?.dll`,
                `${ APPDATA }/luarocks/lib/lua/${ ABIVER }/?.dll`,
            ].join(";").replace(/[\\/]/g, sysPath.sep.repeat(2)) };"..package.cpath`,
        ].join(";"),
    ];
} else if (fs.existsSync(config.Debug.exe)) {
    config.Debug.argv = [
        "-e",
        [
            `package.path="${ [
                `${ LUA_MODULES }/share/lua/${ ABIVER }/?.lua`,
                `${ LUA_MODULES }/share/lua/${ ABIVER }/?/init.lua`,
            ].join(";").replace(/[\\/]/g, sysPath.sep.repeat(2)) };"..package.path`,

            `package.cpath="${ [
                `${ sysPath.resolve(LUA_BINDIR_DEBUG, "..", "lib") }/?.so`,
                `${ sysPath.resolve(LUA_BINDIR_DEBUG, "..", "lib") }/loadall.so`,
                `${ LUA_MODULES }/lib/lua/${ ABIVER }/?.so`,
            ].join(";").replace(/[\\/]/g, sysPath.sep.repeat(2)) };"..package.cpath`,
        ].join(";"),
    ];
}

const unixCmd = argv => {
    return argv.map(arg => {
        if (arg.includes(" ") || arg.includes("\\")) {
            return `'${ arg }'`;
        }

        if (os.platform() === "win32" && arg[0] === "/" && arg[1] !== "/") {
            return `/${ arg }`;
        }

        return arg;
    }).join(" ");
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
    } else {
        throw new Error(`Unsupported extenstion ${ extname }`);
    }

    const cmd = [keys.map(key => `${ key }=${ env[key] }`).join(" "), unixCmd(args.flat())].join(" ");

    if (options.bash) {
        console.log(cmd, "||", "exit $?");
        next(null, 0);
        return;
    }

    console.log(cmd);

    args.push({
        stdio: options.stdio,
        env,
        cwd: options.cwd,
    });

    const child = spawn(...args);
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

const unixPath = path => {
    return `/${ path.replace(":", "").replaceAll("\\", "/") }`;
};

const bash_init = "#!/usr/bin/env bash\n\nset -o pipefail\n";

const main = (options, next) => {
    options = Object.assign({
        cwd: WORKSPACE_ROOT,
        includes: [],
        includes_ext: [".lua", ".py"],
        excludes: [],
        argv: [],
        stdio: "inherit",
    }, options);

    const { cwd, includes, includes_ext, excludes } = options;

    excludes.push(...["init.lua", "common.lua", "download_model.py"]);

    if (options.bash) {
        console.log([
            bash_init,
            `cd ${ cwd.includes(" ") ? `'${ unixPath(cwd) }'` : unixPath(cwd) }`,
            "",
        ].join("\n"));
    }

    explore(sysPath.join(cwd, "samples"), (path, stats, next) => {
        const file = sysPath.relative(cwd, path);
        const basename = sysPath.basename(file);
        const extname = sysPath.extname(file);

        if (
            !includes_ext.includes(extname) ||
            excludes.some(exclude => basename.startsWith(exclude)) ||
            includes.length !== 0 && !includes.some(include => basename.startsWith(include))
        ) {
            next();
            return;
        }

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
            next(code);
        });
    }, (path, stats, files, state, next) => {
        const basename = sysPath.basename(path);
        const skip = state === "begin" && (basename[0] === "." || basename === "BackUp");
        next(null, skip);
    }, next);
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
