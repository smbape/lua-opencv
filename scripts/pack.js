const { spawn, spawnSync } = require("node:child_process");
const sysPath = require("node:path");
const fs = require("fs-extra");
const os = require("node:os");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const pkg = require("../package.json");

const version = process.env.npm_package_version || pkg.version;
const OpenCV_NAME_VERSION = "opencv-4.10.0";
const OpenCV_VERSION = OpenCV_NAME_VERSION.slice("opencv-".length);
const distVersion = process.env.DIST_VERSION || "1"; // TODO : find a way to automatically update it
const workspaceRoot = sysPath.resolve(__dirname, "..");

const batchSuffix = os.platform() === "win32" ? ".bat" : "";
const luarocksDir = sysPath.join(workspaceRoot, "luarocks");
const luarocks = sysPath.join(luarocksDir, `luarocks${ batchSuffix }`);
const lua = sysPath.join(luarocksDir, `lua${ batchSuffix }`);
const new_version = sysPath.join(__dirname, "new_version.lua");

let srcRockSpec;

const spawnExec = (cmd, args, options, next) => {
    console.log(cmd, `'${ args.join("' '") }'`);
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

    const child = spawn(cmd, args, options);
    child.on("error", next);
    child.on("close", code => {
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

const options = {
    server: process.env.LUAROCKS_SERVER ? sysPath.resolve(process.env.LUAROCKS_SERVER) : sysPath.join(workspaceRoot, "out", "install", "luarocks"),
    rockspec: process.env.ROCKSPEC ? sysPath.resolve(process.env.ROCKSPEC) : sysPath.join(luarocksDir, `${ pkg.name }-scm-1.rockspec`),
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
        case "--repair":
            options[key.slice("--".length)] = value;
            break;
        case "--server":
        case "--rockspec":
            if (eq === -1) {
                value = process.argv[++i];
            }
            options[key.slice("--".length)] = value;
            break;
        default:
            throw new Error(`Unknown option ${ arg }`);
    }
}

waterfall([
    next => {
        fs.symlink("../../docs", sysPath.join(luarocksDir, "lua_modules", "docs"), "junction", err => {
            next(err && err.code === "EEXIST" ? null : err);
        });
    },

    next => {
        fs.symlink("../../samples", sysPath.join(luarocksDir, "lua_modules", "samples"), "junction", err => {
            next(err && err.code === "EEXIST" ? null : err);
        });
    },

    next => {
        fs.mkdirs(options.server, next);
    },

    (performed, next) => {
        spawnExec(luarocks, ["new_version", "--tag", `v${ version }`, options.rockspec, `${ OpenCV_VERSION }-${ distVersion }`], {
            stdio: "tee",
            cwd: workspaceRoot
        }, next);
    },

    (_stdout, _stderr, next) => {
        srcRockSpec = _stdout.toString().trim().slice("Wrote ".length);

        spawnExec(luarocks, ["pack", srcRockSpec], {
            stdio: "inherit",
            cwd: workspaceRoot
        }, next);
    },

    next => {
        const srcRock = `${ srcRockSpec.slice(0, -".rockspec".length) }.src.rock`;
        eachOfLimit([srcRockSpec, srcRock], 1, (filename, i, next) => {
            const src = sysPath.join(workspaceRoot, filename);
            const dst = sysPath.join(options.server, filename);
            fs.move(src, dst, {
                overwrite: true,
            }, next);
        }, next);
    },

    next => {
        const has_luarocks = os.platform() !== "win32" || spawnSync(luarocks, ["list", "luarocks"]).stdout.toString().trim().includes("luarocks");
        if (has_luarocks) {
            next();
            return;
        }

        spawnExec(luarocks, ["install", "luarocks"], {
            stdio: "inherit",
            cwd: workspaceRoot
        }, next);
    },

    next => {
        spawnExec(luarocks, ["config", "variables.LUA"], {
            stdio: "tee",
            cwd: workspaceRoot
        }, next);
    },

    (_stdout, _stderr, next) => {
        const LUA = _stdout.toString().trim();

        spawnExec(LUA, ["-v"], {
            stdio: "tee",
            cwd: workspaceRoot
        }, next);
    },

    (_stdout, _stderr, next) => {
        const [, target, ver] = (_stdout.length === 0 ? _stderr : _stdout).toString().trim().toLowerCase().match(/(\w+) (\d+\.\d+)/);
        const abi = target === "luajit" ? "5.1" : ver;

        const binary = `${ OpenCV_VERSION }${ target }${ ver }-${ distVersion }`;
        const binaryRockSpec = `${ srcRockSpec.slice(0, -`${ OpenCV_VERSION }-${ distVersion }.rockspec`.length) }${ binary }.rockspec`;
        const lua_modules = sysPath.join(luarocksDir, "lua_modules");

        waterfall([
            next => {
                const args = [new_version, options.rockspec, binary, abi, "--platform", os.platform()];
                if (options.repair) {
                    args.push("--repair");
                }

                spawnExec(lua, args, {
                    stdio: "inherit",
                    cwd: lua_modules
                }, next);
            },

            next => {
                spawnExec(luarocks, ["make", "--pack-binary-rock", binaryRockSpec], {
                    stdio: "tee",
                    cwd: lua_modules
                }, next);
            },

            (stdout, stderr, next) => {
                stdout = stdout.toString();
                const start = stdout.indexOf("Packed: ") + "Packed: ".length;
                const binaryRock = stdout.slice(start).trim().slice(lua_modules.length + 1);

                eachOfLimit([binaryRockSpec, binaryRock], 1, (filename, i2, next) => {
                    const src = sysPath.join(lua_modules, filename);
                    const dst = sysPath.join(options.server, filename);
                    fs.move(src, dst, {
                        overwrite: true,
                    }, next);
                }, next);
            },

        ], next);
    },

    next => {
        fs.readFile(luarocks, next);
    },

    (buffer, next) => {
        const content = buffer.toString();
        const pos = content.indexOf(" --project-tree ");
        const quote = os.platform() === "win32" ? "\"" : "'";
        const end = content.lastIndexOf(quote, pos - 1);
        const start = content.lastIndexOf(quote, end - 1) + quote.length;
        const exe = content.slice(start, end);

        spawnExec(exe.replace(/(?<=^|[/\\])luarocks([^/\\]*)$/, "luarocks-admin$1"), ["make-manifest", options.server], {
            stdio: "inherit",
            cwd: luarocksDir
        }, next);
    },
], err => {
    if (err) {
        throw err;
    }
});
