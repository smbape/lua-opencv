const {
    spawn
} = require("node:child_process");
const sysPath = require("node:path");
const fs = require("fs-extra");
const os = require("node:os");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const prepublishRoot = sysPath.resolve(__dirname, "..", "out", "prepublish");
const wrapperSuffix = os.platform() === "win32" ? ".bat" : "";
const shellSuffix = os.platform() === "win32" ? ".bat" : ".sh";
const GIT_BRANCH = process.env.GIT_BRANCH || "main";

const spawnExec = (cmd, args, options, next) => {
    const {
        stdio
    } = options;

    if (stdio === "tee") {
        options.stdio = ["inherit", "pipe", "pipe"];
    }

    const stdout = Object.assign([], {
        nread: 0
    });

    const stderr = Object.assign([], {
        nread: 0
    });

    let err = false;

    const child = spawn(cmd, args, options);

    child.on("error", _err => {
        err = true;
        next(_err);
    });

    child.on("close", code => {
        if (err) {
            return;
        }

        if (stdio === "tee") {
            next(code, Buffer.concat(stdout, stdout.nread), Buffer.concat(stderr, stderr.nread));
        } else {
            next(code);
        }
    });

    if (stdio === "tee") {
        child.stdout.on("data", chunk => {
            stdout.push(chunk);
            stdout.nread += chunk.length;
            process.stdout.write(chunk);
        });

        child.stderr.on("data", chunk => {
            stderr.push(chunk);
            stderr.nread += chunk.length;
            process.stderr.write(chunk);
        });
    }
};

const prepublish = (target, version, cache, options, next) => {
    const { name, cmake_build_args, pack, repair } = options;
    const workspaceRoot = sysPath.join(prepublishRoot, version);
    const projectRoot = sysPath.join(workspaceRoot, name);
    const originalRockSpec = sysPath.join(projectRoot, "luarocks", "opencv_lua-scm-1.rockspec");
    const scmRockSpec = name !== "opencv_lua" ? sysPath.join(projectRoot, `${ name }-scm-1.rockspec`) : originalRockSpec;

    waterfall([
        next => {
            fs.mkdirs(projectRoot, next);
        },

        (performed, next) => {
            fs.access(sysPath.join(projectRoot, ".git"), fs.constants.X_OK, err => {
                next(null, !err);
            });
        },

        (exists, next) => {
            const tasks = [];

            if (exists) {
                tasks.push(...[
                    ["git", ["remote", "set-url", "origin", sysPath.resolve(__dirname, "..")]],
                    ["git", ["reset", "--hard", "HEAD"]],
                    ["git", ["clean", "-fd"]],
                    ["git", ["fetch", "origin", GIT_BRANCH]],
                    ["git", ["checkout", GIT_BRANCH]],
                    ["git", ["pull", "origin", GIT_BRANCH, "--force"]],
                ]);
            } else {
                tasks.push(...[
                    ["git", ["init", "-b", GIT_BRANCH]],
                    ["git", ["remote", "add", "origin", sysPath.resolve(__dirname, "..")]],
                    ["git", ["pull", "origin", GIT_BRANCH]],
                    ["git", ["branch", "--set-upstream-to=origin/main", GIT_BRANCH]],
                    ["git", ["config", "pull.rebase", "true"]],
                    ["git", ["config", "user.email", "you@example.com"]],
                    ["git", ["config", "user.name", "Your Name"]],
                ]);
            }

            if (name !== "opencv_lua" || cmake_build_args.length !== 0) {
                tasks.push(async next => {
                    await fs.copy(originalRockSpec, scmRockSpec);
                    const buffer = await fs.readFile(scmRockSpec);
                    const indent = " ".repeat(6);

                    let rockspec = buffer.toString();

                    if (name !== "opencv_lua") {
                        rockspec = rockspec.replaceAll("opencv_lua", name);
                    }

                    if (cmake_build_args.length !== 0) {
                        rockspec = rockspec.replace("LUA_INCDIR = \"$(LUA_INCDIR)\",", `LUA_INCDIR = "$(LUA_INCDIR)",\n${ indent }${ cmake_build_args.join(`,\n${ indent }`) },`);
                    }

                    await fs.writeFile(scmRockSpec, rockspec);
                    next();
                });
            }

            eachOfLimit(tasks, 1, (job, icmd, next) => {
                if (typeof job === "function") {
                    job(next);
                    return;
                }

                const [cmd, args] = job;

                spawnExec(cmd, args, {
                    stdio: "inherit",
                    cwd: projectRoot,
                }, next);
            }, next);
        },

        next => {
            const luarocks = sysPath.join("luarocks", `luarocks${ wrapperSuffix }`);

            const tasks = [
                [sysPath.join(projectRoot, `build${ shellSuffix }`), [`-DLua_VERSION=${ version }`, "--target", target, "--install"]],
                [sysPath.join(projectRoot, `build${ shellSuffix }`), [`-DLua_VERSION=${ version }`, "--target", "luarocks"]],
                [luarocks, ["make", scmRockSpec]],
            ];

            const env = {
                ROCKSPEC: scmRockSpec,
            };

            if (pack) {
                const args = [sysPath.join("scripts", "pack.js")];
                if (repair) {
                    args.push("--repair");
                }

                if (options.server) {
                    args.push("--server", options.server);
                } else {
                    env.LUAROCKS_SERVER = sysPath.join(prepublishRoot, "server");
                }

                tasks.push(["node", args]);
            }

            if (os.platform() !== "win32") {
                tasks.splice(2, 0, ...[
                    [luarocks, ["config", "--scope", "project", "cmake_generator", "Ninja"]],
                    [luarocks, ["config", "--scope", "project", "cmake_build_args", "--", `-j${ os.cpus().length }`]],
                ]);
            }

            const [cachedTarget, cachedVersion] = cache;
            const enableCache = cachedTarget !== target || cachedVersion !== version;
            if (enableCache) {
                const abi = cachedTarget === "luajit" ? "5.1" : cachedVersion;
                const OpenCV_DIR = sysPath.resolve(prepublishRoot, cachedVersion, name, "luarocks", "lua_modules", "lib", "luarocks", `rocks-${ abi }`, name, "scm-1");
                if (fs.existsSync(OpenCV_DIR)) {
                    env.OpenCV_DIR = OpenCV_DIR;
                }
            }

            eachOfLimit(tasks, 1, ([cmd, args], icmd, next) => {
                spawnExec(cmd, args, {
                    stdio: "inherit",
                    cwd: projectRoot,
                    env: Object.assign(env, process.env)
                }, next);
            }, next);
        },
    ], next);
};

const options = {
    name: "opencv_lua",
    cmake_build_args: [],
};

for (let i = 2; i < process.argv.length; i++) {
    const arg = process.argv[i];
    const eq = arg.indexOf("=");

    let key, value;

    if (eq === -1) {
        key = arg;
        value = true;
    } else {
        key = arg.slice(0, eq);
        value = arg.slice(eq + 1);
    }

    switch (key) {
        case "--pack":
        case "--repair":
            options[key.slice("--".length)] = value;
            break;
        case "--server":
        case "--name":
        case "--lua-versions":
            if (eq === -1) {
                value = process.argv[++i];
            }
            options[key.slice("--".length)] = value;
            break;
        default:
            if (key.startsWith("-D")) {
                if (eq === -1) {
                    value = process.argv[++i];
                }
                const colon = key.indexOf(":");
                const arg_name = key.slice("-D".length, colon === -1 ? key.length : colon);
                options.cmake_build_args.push(`${ arg_name } = "${ value.replaceAll("\"", "\\\"") }"`);
                continue;
            }
            throw new Error(`Unknown option ${ arg }`);
    }
}

const versions = options["lua-versions"] ? options["lua-versions"].split(",") : null;
const flavors = [false, true];

if (options.cmake_build_args.length !== 0) {
    flavors.length = 1;
    flavors[0] = false;
}

eachOfLimit([
    ["luajit", "luajit-2.1"],
    ["lua", "5.4"],
    ["lua", "5.3"],
    ["lua", "5.2"],
    ["lua", "5.1"],
], 1, ([target, version], i, next) => {
    if (versions && !versions.includes(version)) {
        next();
        return;
    }

    eachOfLimit(flavors, 1, (contrib, icontrib, next) => {
        const opts = Object.assign({}, options);
        if (contrib) {
            opts.name += "-contrib";
            opts.cmake_build_args = ["BUILD_contrib = \"ON\""];
        }
        prepublish(target, version, ["luajit", "luajit-2.1"], opts, next);
    }, next);
});
