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

const PROJECT_DIR = WORKSPACE_ROOT;
const opencv_SOURCE_DIR = (() => {
    const platform = os.platform() === "win32" ? (/cygwin/.test(process.env.HOME) ? "Cygwin" : "x64") : "*-GCC";
    for (const buildDir of [
        process.env.CMAKE_CURRENT_BINARY_DIR,
        `out/build/${ platform }-*`,
        "build.luarocks",

    ]) {
        if (!buildDir) {
            continue;
        }

        const file = findFile(`${ buildDir }/opencv/opencv-src`, PROJECT_DIR);
        if (file) {
            return file;
        }
    }

    return null;
})();

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

    const ABIVER = spawnSync(luarcoks, ["config", "lua_version"]).stdout.toString().trim();
    const LUA_MODULES = sysPath.join(LUAROCKS_BINDIR, "lua_modules");

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
                `${ APPDATA }/luarocks/lib/lua/${ ABIVER }/?.dll`
            ].join(";").replace(/[\\/]/g, sysPath.sep.repeat(2)) };"..package.cpath`,
        ].join(";"),
    ];
} else if (fs.existsSync(config.Debug.exe)) {
    const {env, exe} = config.Debug;
    env.LUA_CPATH = `${ sysPath.resolve(exe, "../../lib/?.so") };${ spawnSync(exe, ["-e", "print(package.cpath)"]).stdout.toString().trim() }`;
}

const run = (file, options, env, next) => {
    const { BUILD_TYPE, OPENCV_BUILD_TYPE } = process.env;
    if (BUILD_TYPE && BUILD_TYPE !== env.BUILD_TYPE || OPENCV_BUILD_TYPE && OPENCV_BUILD_TYPE !== env.OPENCV_BUILD_TYPE) {
        next(0, null);
        return;
    }

    console.log("\nRunning", file, env);
    const extname = sysPath.extname(file);

    const args = [];

    if (extname === ".lua") {
        const argv = [...config[env.BUILD_TYPE].argv, file, ...options.argv];
        if (sysPath.basename(file) === "object_detection.lua" && options.argv.length === 0) {
            argv.push("ssd_tf", "--input", "vtest.avi");
        }
        args.push(config[env.BUILD_TYPE].exe, argv);
    } else if (extname === ".py") {
        const argv = [file];
        if (sysPath.basename(file) === "object_detection.py" && options.argv.length === 0) {
            argv.push("ssd_tf", "--input", "vtest.avi");
        }
        args.push(`python${ exeSuffix }`, argv);
    } else {
        throw new Error(`Unsupported extenstion ${ extname }`);
    }

    console.log(args.flat().map(arg => (arg.includes(" ") ? `"${ arg }"` : arg)).join(" "));

    args.push({
        stdio: options.stdio,
        env: Object.assign({}, config[env.BUILD_TYPE].env, process.env, env),
        cwd: options.cwd,
    });

    const child = spawn(...args);
    child.on("error", next);
    child.on("close", next);

    if (typeof options.run === "function") {
        options.run(child);
    }
};

const main = (options, next) => {
    options = Object.assign({
        cwd: WORKSPACE_ROOT,
        includes: [],
        argv: [],
        stdio: "inherit",
        INCLUDED_EXT: [".lua", ".py"],
        EXCLUDED_FILES: ["init.lua", "common.lua", "download_model.py"],
    }, options);

    const { cwd, includes, INCLUDED_EXT, EXCLUDED_FILES } = options;

    explore(sysPath.join(cwd, "samples"), (path, stats, next) => {
        const file = sysPath.relative(cwd, path);
        const basename = sysPath.basename(file);
        const extname = sysPath.extname(file);

        if (
            !INCLUDED_EXT.includes(extname) ||
            EXCLUDED_FILES.includes(basename) ||
            includes.length !== 0 && !includes.some(include => basename.startsWith(include))
        ) {
            next();
            return;
        }

        waterfall([
            next => {
                run(file, options, {
                    BUILD_TYPE: "Release",
                    OPENCV_BUILD_TYPE: "Release",
                }, next);
            },

            (signal, next) => {
                run(file, options, {
                    BUILD_TYPE: "Debug",
                    OPENCV_BUILD_TYPE: "Debug",
                }, next);
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
        argv: [],
        "--": 0,
    };

    for (const arg of process.argv.slice(2)) {
        if (arg === "--") {
            options[arg]++;
        } else if (options["--"] === 1 && arg[0] !== "-") {
            options.includes.push(arg);
        } else if (options["--"] > 1 || options["--"] !== 0 && arg[0] === "-") {
            options.argv.push(arg);
        } else if (arg === "--Debug" || arg === "--Release") {
            process.env.BUILD_TYPE = arg.slice(2);
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
            process.exitCode = code;
        }
    });
}
