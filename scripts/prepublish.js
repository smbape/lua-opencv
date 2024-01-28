const { spawn } = require("node:child_process");
const sysPath = require("node:path");
const fs = require("fs-extra");
const os = require("node:os");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const prepublish = sysPath.resolve(__dirname, "..", "out", "prepublish");
const wrapperSuffix = os.platform() === "win32" ? ".bat" : "";
const shellSuffix = os.platform() === "win32" ? ".bat" : ".sh";
const pack = process.argv.includes("--pack");
const GIT_BRANCH = process.env.GIT_BRANCH || "main";

const spawnExec = (cmd, args, options, next) => {
    const {stdio} = options;

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

eachOfLimit([
    ["luajit", "luajit-2.1"],
    ["lua", "5.4"],
    ["lua", "5.3"],
    ["lua", "5.2"],
    ["lua", "5.1"],
], 1, ([target, version], i, next) => {
    const workspaceRoot = sysPath.join(prepublish, version);
    const projectRoot = sysPath.join(workspaceRoot, "lua-opencv");

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
            const cmds = [];

            if (exists) {
                cmds.push(...[
                    ["git", ["remote", "set-url", "origin", sysPath.resolve(__dirname, "..")]],
                    ["git", ["reset", "--hard", "HEAD"]],
                    ["git", ["clean", "-fd"]],
                    ["git", ["fetch", "origin", GIT_BRANCH]],
                    ["git", ["checkout", GIT_BRANCH]],
                    ["git", ["pull", "origin", GIT_BRANCH]],
                ]);
            } else {
                cmds.push(...[
                    ["git", ["init", "-b", GIT_BRANCH]],
                    ["git", ["remote", "add", "origin", sysPath.resolve(__dirname, "..")]],
                    ["git", ["pull", "origin", GIT_BRANCH]],
                    ["git", ["branch", "--set-upstream-to=origin/main", GIT_BRANCH]],
                ]);
            }

            eachOfLimit(cmds, 1, ([cmd, args], i2, next) => {
                spawnExec(cmd, args, {
                    stdio: "inherit",
                    cwd: projectRoot,
                }, next);
            }, next);
        },

        next => {
            const luarocks = sysPath.join("luarocks", `luarocks${ wrapperSuffix }`);
            const cmds = [
                [sysPath.join(projectRoot, `build${ shellSuffix }`), ["--target", target, `-DLua_VERSION=${ version }`, "--install"]],
                [sysPath.join(projectRoot, `build${ shellSuffix }`), ["--target", "luarocks"]],
                [luarocks, ["make", sysPath.join("luarocks", "opencv_lua-scm-1.rockspec")]],
            ];

            if (pack) {
                cmds.push(["node", [sysPath.join("scripts", "pack.js")]]);
            }

            if (os.platform() !== "win32") {
                cmds.splice(2, 0, ...[
                    [luarocks, ["config", "--scope", "project", "cmake_generator", "Ninja"]],
                    [luarocks, ["config", "--scope", "project", "cmake_build_args", "--", `-j${ Math.max(1, os.cpus().length - 2) }`]],
                ]);
            }

            eachOfLimit(cmds, 1, ([cmd, args], i2, next) => {
                spawnExec(cmd, args, {
                    stdio: "inherit",
                    cwd: projectRoot,
                    env: Object.assign({
                        LUAROCKS_SERVER: sysPath.join(prepublish, "server")
                    }, process.env)
                }, next);
            }, next);
        },
    ], next);
});
