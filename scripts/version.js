const { spawn } = require("node:child_process");
const sysPath = require("node:path");
const fs = require("node:fs");
const os = require("node:os");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const doctoc = require("../generator/doctoc");

const version = process.env.npm_package_version || require("../package.json").version;
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

waterfall([
    next => {
        const files_to_change = [
            "README.md",
            "docs/hosting-you-own-binary-rocks-Windows.md",
            "docs/hosting-you-own-binary-rocks-Linux.md",
            "luarocks/opencv_lua-scm-1.rockspec",
        ];
        eachOfLimit(files_to_change, os.cpus().length, (filename, i, next) => {
            updateContent(sysPath.join(workspaceRoot, filename), oldContent => {
                return oldContent
                    .replace(new RegExp("(/lua-opencv/releases/download/v|/lua-opencv/tree/v)[0-9]+\\.[0-9]+\\.[0-9]+", "g"), `$1${ version }`)
                    .replace(new RegExp("(--branch v)[0-9]+\\.[0-9]+\\.[0-9]+( https://github.com/smbape/lua-opencv)", "g"), `$1${ version }$2`);
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

            const child = spawn("git", ["add", readme], {
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
