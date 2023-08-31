const { spawn, spawnSync } = require("node:child_process");
const sysPath = require("node:path");
const fs = require("node:fs");
const os = require("node:os");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const pkg = require("../package.json");

const regexEscape = str => {
    return str.replace(/[-/\\^$*+?.()|[\]{}]/g, "\\$&");
};

const version = process.env.npm_package_version || pkg.version;
const OpenCV_NAME_VERSION = "opencv-4.8.0";
const OpenCV_VERSION = OpenCV_NAME_VERSION.slice("opencv-".length);
const distVersion = "1"; // TODO : find a way to automatically update it
const workspaceRoot = sysPath.resolve(__dirname, "..");

const readme = sysPath.join(workspaceRoot, "README.md");

const batchSuffix = os.platform() === "win32" ? ".bat" : "";
const luarocksDir = sysPath.join(workspaceRoot, "luarocks");
const luarocks = sysPath.join(luarocksDir, `luarocks${ batchSuffix }`);
const lua = sysPath.join(luarocksDir, `lua${ batchSuffix }`);
const new_version = sysPath.join(__dirname, "new_version.lua");

const scmRockSpec = sysPath.join(luarocksDir, `${ pkg.name }-scm-1.rockspec`);

const spawnExec = (cmd, args, options, next) => {
    const child = spawn(cmd, args, options);
    child.on("error", next);
    child.on("close", () => {
        next();
    });
};

const updateContent = (file, replacer, cb) => {
    waterfall([
        next => {
            fs.readFile(file, (err, buffer) => {
                if (err && err.code === "ENOENT") {
                    err = null;
                    buffer = "";
                }
                next(err, buffer);
            });
        },

        (buffer, next) => {
            const oldContent = buffer.toString();
            const newContent = replacer(oldContent);

            if (newContent === oldContent) {
                next(null, false);
                return;
            }

            fs.writeFile(file, newContent, err => {
                next(err, true);
            });
        },
    ], cb);
};

waterfall([
    next => {
        spawnExec(luarocks, ["new_version", "--tag", `v${ version }`, scmRockSpec, `${ OpenCV_VERSION }-${ distVersion }`], {
            stdio: "inherit",
            cwd: workspaceRoot
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
        fs.readFile(scmRockSpec, next);
    },

    (buffer, next) => {
        const scmContent = buffer.toString();

        eachOfLimit([
            // ["luajit", "luajit-2.1.0-beta3", "5.1"],
            ["lua", "5.4.6", "5.4"],
        ], 1, ([target, lua_VERSION, abi], i, next) => {
            const [, ver] = lua_VERSION.match(/^(?:luajit-)?(\d+\.\d+)/);
            const binary = `${ OpenCV_VERSION }v${ version }${ target }${ ver }-${ distVersion }`;
            waterfall([
                next => {
                    spawnExec(lua, [new_version, scmRockSpec, binary, abi, "--prefix", "luarocks/lua_modules", "--platform", os.platform()], {
                        stdio: "inherit",
                        cwd: sysPath.join(luarocksDir, "lua_modules")
                    }, next);
                },

                next => {
                    spawnExec(luarocks, ["make", "--pack-binary-rock", sysPath.join(luarocksDir, "lua_modules", `${ pkg.name }-${ binary }.rockspec`)], {
                        stdio: "inherit",
                        cwd: workspaceRoot
                    }, next);
                },
            ], next);
        }, next);
    }
], err => {
    if (err) {
        throw err;
    }
});
