const auditwheelOptions = {
    aliases: new Map([]),
    flags: new Set([
        "--strip",
        "--only-plat",
        "--disable-isa-ext-check",
    ]),
    options: new Set([
        "--plat",
        "--exclude",
    ]),
};

if (module !== require.main) {
    exports.auditwheelOptions = auditwheelOptions;
    return;
}

const { spawn, spawnSync } = require("node:child_process");
const sysPath = require("node:path");
const fs = require("fs-extra");
const os = require("node:os");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const pkg = require("../package.json");

const version = process.env.npm_package_version || pkg.version;
const OpenCV_NAME_VERSION = "opencv-4.12.0";
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
    options = Object.assign({}, options);

    // https://nodejs.org/docs/latest-v18.x/api/child_process.html#spawning-bat-and-cmd-files-on-windows
    if (os.platform() === "win32" && (cmd.endsWith(".bat") || cmd.endsWith(".cmd"))) {
        if (cmd.includes(" ")) {
            cmd = `"${ cmd }"`;
        }
        options.shell = true;
    }

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

    console.log(cmd, `'${ args.join("' '") }'`);

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

const options = {
    server: process.env.LUAROCKS_SERVER ? sysPath.resolve(process.env.LUAROCKS_SERVER) : sysPath.join(workspaceRoot, "out", "install", "luarocks"),
    rockspec: process.env.ROCKSPEC ? sysPath.resolve(process.env.ROCKSPEC) : sysPath.join(luarocksDir, `${ pkg.name }-scm-1.rockspec`),
};

const aliases = new Map([
    ...auditwheelOptions.aliases,
]);

const optionValueKeys = new Set([
    "--repair",
    ...auditwheelOptions.flags,
]);

const oneValueKeys = new Set([
    "--server",
    "--rockspec",
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

    throw new Error(`Unknown option ${ arg }`);
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
        spawnExec(luarocks, ["list", "--porcelain", "luarocks"], {
            stdio: "pipe",
            cwd: workspaceRoot
        }, next);
    },

    (_stdout, _stderr, next) => {
        if (_stdout.toString().trim().includes("luarocks")) {
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
        options.abi = abi;

        const binary = `${ OpenCV_VERSION }${ target }${ ver }-${ distVersion }`;
        const binaryRockSpec = `${ srcRockSpec.slice(0, -`${ OpenCV_VERSION }-${ distVersion }.rockspec`.length) }${ binary }.rockspec`;
        const lua_modules = sysPath.join(luarocksDir, "lua_modules");

        waterfall([
            next => {
                const args = [new_version, options.rockspec, binary, abi, "--platform", os.platform()];

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
        const luarocks_admin = sysPath.join(luarocksDir, "lua_modules", "bin", `luarocks-admin${ batchSuffix }`);
        fs.readFile(luarocks_admin, next);
    },

    (buffer, next) => {
        const content = buffer.toString();

        let lua_admin;

        if (os.platform() === "win32") {
            const end = content.indexOf("\" %*");
            const start = content.lastIndexOf("\"", end - 1) + 1;
            lua_admin = content.slice(start, end).replace(/rocks-\d\.\d/, `rocks-${ options.abi }`);
        } else {
            const end = content.indexOf("' \"$@\"");
            const start = content.lastIndexOf("'", end - 1) + 1;
            lua_admin = content.slice(start, end).replace(/rocks-\d\.\d/, `rocks-${ options.abi }`);
        }

        const init_argv = [
            "-e",
            [
                `package.path='${ [
                    `${ sysPath.join(luarocksDir, "src") }/?.lua`,
                ].join(";").replace(/[\\]/g, "/") };'..package.path`,
            ].join(";"),
        ];

        spawnExec(lua, [...init_argv, lua_admin, "make-manifest", options.server], {
            stdio: "inherit",
            cwd: luarocksDir,
        }, next);
    },
], err => {
    if (err) {
        throw err;
    }
});
