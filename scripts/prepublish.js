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
const { auditwheelOptions } = require("./pack");

const unixEscape = (arg, verbatim = true) => {
    if (verbatim && os.platform() === "win32" && arg[0] === "/" && arg[1] !== "/") {
        arg = `/${ arg }`;
    }

    if (/[^\w/=.-]/.test(arg)) {
        arg = `'${ arg.replaceAll("'", "'\\''") }'`;
    }

    return arg;
};

const unixCmd = argv => {
    return argv.map(arg => unixEscape(arg)).join(" ");
};

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

    console.log([cmd, unixCmd(args.flat())].join(" "));

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

        if (stdio === "pipe" || stdio === "tee") {
            next(code, Buffer.concat(stdout, stdout.nread), Buffer.concat(stderr, stderr.nread));
        } else {
            next(code);
        }
    });

    if (stdio === "pipe" || stdio === "tee") {
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

const prepublish = (target, version, options, next) => {
    const { name, cmake_build_args, branch } = options;
    const workspaceRoot = sysPath.join(prepublishRoot, "build");
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
                    ["git", ["fetch", "origin", branch]],
                    ["git", ["checkout", branch]],
                    ["git", ["pull", "origin", branch, "--force"]],
                ]);
            } else {
                tasks.push(...[
                    ["git", ["init", "-b", branch]],
                    ["git", ["remote", "add", "origin", sysPath.resolve(__dirname, "..")]],
                    ["git", ["pull", "origin", branch]],
                    ["git", ["branch", `--set-upstream-to=origin/${ branch }`, branch]],
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

            if (options.pack) {
                const args = [sysPath.join("scripts", "pack.js"), "--rockspec", scmRockSpec];

                if (options.server) {
                    args.push("--server", options.server);
                }

                if (os.platform() !== "win32" && options.repair) {
                    args.push("--repair");

                    for (const flag of auditwheelOptions.flags) {
                        if (options[flag.slice("--".length)]) {
                            args.push(flag);
                        }
                    }

                    for (const option of auditwheelOptions.options) {
                        if (Object.hasOwn(options, option.slice("--".length))) {
                            args.push(option, options[option.slice("--".length)]);
                        }
                    }
                }

                tasks.push(["node", args]);
            }

            if (os.platform() !== "win32") {
                tasks.splice(2, 0, ...[
                    [luarocks, ["config", "--scope", "project", "cmake_generator", "Ninja"]],
                    [luarocks, ["config", "--scope", "project", "cmake_build_args", "--", `-j${ os.cpus().length } -- -d explain`]],
                ]);
            }

            eachOfLimit(tasks, 1, ([cmd, args], icmd, next) => {
                spawnExec(cmd, args, {
                    stdio: "inherit",
                    cwd: projectRoot,
                    env: process.env
                }, next);
            }, next);
        },
    ], next);
};

const options = {
    name: "opencv_lua",
    cmake_build_args: [],
    branch: process.env.GIT_BRANCH || "main",
    server: process.env.LUAROCKS_SERVER ? sysPath.resolve(process.env.LUAROCKS_SERVER) : sysPath.join(prepublishRoot, "server"),
};

const aliases = new Map([
    ...auditwheelOptions.aliases,
]);

const optionValueKeys = new Set([
    "--pack",
    "--repair",
    ...auditwheelOptions.flags,
]);

const oneValueKeys = new Set([
    "--branch",
    "--server",
    "--name",
    "--lua-versions",
    ...auditwheelOptions.options,
]);

const argv = process.argv.slice(2);

for (let i = 0; i < argv.length; i++) {
    const arg = argv[i];
    const eq = arg.indexOf("=");

    let key, value;

    if (eq === -1) {
        key = arg;
        value = true;
    } else {
        key = arg.slice(0, eq);
        value = arg.slice(eq + 1);
    }

    if (aliases.has(key)) {
        key = aliases.get(key);
    }

    if (optionValueKeys.has(key)) {
        if (value !== true) {
            throw new Error(`${ key } is a flag not, therefore, cannot have a value`);
        }
        options[key.slice("--".length)] = value;
        continue;
    }

    if (oneValueKeys.has(key)) {
        if (eq === -1) {
            value = argv[++i];
        }
        options[key.slice("--".length)] = value;
        continue;
    }

    if (key === "-D") {
        if (++i === argv.length) {
            throw new Error(`Unknown option ${ arg }`);
        }

        if (!argv[i].includes("=")) {
            throw new Error(`Value missing for option ${ argv[i] }`);
        }

        argv[i] = key + argv[i];
        continue;
    }

    if (key.startsWith("-D")) {
        if (eq === -1) {
            value = argv[++i];
        }
        const colon = key.indexOf(":");
        const arg_name = key.slice("-D".length, colon === -1 ? key.length : colon);
        options.cmake_build_args.push(`${ arg_name } = "${ value.replaceAll("\"", "\\\"") }"`);
        continue;
    }

    throw new Error(`Unknown option ${ arg }`);
}

const versions = options["lua-versions"] ? options["lua-versions"].trim().split(/[\s,]+/) : [];
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
    if (versions.length !== 0 && !versions.includes(version)) {
        next();
        return;
    }

    eachOfLimit(flavors, 1, (contrib, icontrib, next) => {
        const opts = Object.assign({}, options);
        if (contrib) {
            opts.name += "-contrib";
            opts.cmake_build_args = ["BUILD_contrib = \"ON\""];
        }
        prepublish(target, version, opts, next);
    }, next);
});
