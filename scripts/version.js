const { spawn } = require("node:child_process");
const sysPath = require("node:path");
const fs = require("node:fs");
const os = require("node:os");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const doctoc = require("../generator/doctoc");

const version = process.env.npm_package_version || require("../package.json").version;

const versions = (() => {
    const semvers = [];

    for (let i = 0, last_index = 0, pre_realease = false; i <= version.length; i++) {
        if (i === version.length || version[i] === "." || !pre_realease && version[i] === "-") {
            if (pre_realease && semvers.length < 3) {
                for (let j = semvers.length; j < 3; j++) {
                    semvers[j] = "0";
                }
            }

            if (i !== version.length && version[i] === "-") {
                pre_realease = true;
            }

            const part = version.slice(last_index, i);
            if (!/^\d+$/.test(part)) {
                break;
            }

            semvers.push(part);
            last_index = i + 1;
        }
    }

    return semvers;
})();

if (versions.length > 4) {
    versions.length = 4;
}

for (let i = versions.length; i < 4; i++) {
    versions[i] = 0;
}

const workspaceRoot = sysPath.resolve(__dirname, "..");

const updateContent = (file, replacer, cb) => {
    waterfall([
        next => {
            fs.readFile(file, next);
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

        (hasChanged, next) => {
            if (!hasChanged) {
                next(null, hasChanged);
                return;
            }

            const child = spawn("git", ["add", file], {
                stdio: "inherit"
            });

            child.on("error", next);
            child.on("close", () => {
                next(null, hasChanged);
            });
        }
    ], cb);
};

const files_to_change = [
    "README.md",
    "docs/hosting-you-own-binary-rocks-Windows.md",
    "docs/hosting-you-own-binary-rocks-Linux.md",
    "luarocks/opencv_lua-scm-1.rockspec",
    "scripts/tasks.sh",
];

const VERSION_REG = "[0-9]+\\.[0-9]+\\.[0-9]+(?:-[0-9]+)?";

waterfall([
    next => {
        eachOfLimit(files_to_change, os.cpus().length, (filename, i, next) => {
            updateContent(sysPath.join(workspaceRoot, filename), oldContent => {
                return oldContent
                    .replace(new RegExp(`(/lua-opencv/releases/download/v|/lua-opencv/tree/v)${ VERSION_REG }`, "g"), `$1${ version }`)
                    .replace(new RegExp(`(--branch v)${ VERSION_REG }( https://github.com/smbape/lua-opencv)`, "g"), `$1${ version }$2`)
                    .replace(new RegExp(`\\\$\\\{DIST_VERSION:-\\d+\\\}`, "g"), `\${DIST_VERSION:-${ versions[3] }}`);
            }, (err, hasChanged) => {
                next(err);
            });
        }, next);
    },

    next => {
        const readme = sysPath.join(workspaceRoot, "README.md");

        doctoc.transformAndSave([readme], (err, transformed) => {
            if (!transformed) {
                next(null, transformed);
                return;
            }

            const child = spawn("git", ["add", ...files_to_change.map(filename => sysPath.join(workspaceRoot, filename))], {
                stdio: "inherit"
            });

            child.on("error", next);
            child.on("close", () => {
                next(null, transformed);
            });

            next(err, transformed);
        });
    }
], err => {
    if (err) {
        throw err;
    }
});
