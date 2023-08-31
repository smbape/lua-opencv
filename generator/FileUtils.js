const fs = require("node:fs");
const sysPath = require("node:path");
const cpus = require("node:os").cpus().length;
const eol = require("eol");
const eachOfLimit = require("async/eachOfLimit");
const waterfall = require("async/waterfall");
const series = require("async/series");
const { mkdirp } = require("mkdirp");
const doctoc = require("./doctoc");

/**
 * @param  {String} str
 * @param  {String} pattern
 * @return {boolean}
 */
const isMatch = (str, pattern) => {
    const slen = str.length;
    const plen = pattern.length;

    let s = 0;
    let p = 0;
    let lastindex = -1;
    let lastmatch = 0;

    while (s < slen) {
        if (p < plen && pattern[p] === "*") {
            // it is enough to match patterns that are not wilward in the order of encounter
            // therefore, keeping track of the last wildcard position is enough
            while (p < plen && pattern[p] === "*") {
                p++;
            }

            lastindex = p;
            lastmatch = s;
        } else if (p < plen && (pattern[p] === "?" || pattern[p] === str[s])) {
            // pattern and text matches, advance both cursors
            p++;
            s++;
        } else if (lastindex !== -1) {
            // there is a wildcard before the remaining pattern
            // the remaining pattern was not found from the lastmatch
            // match one character as wilwcard
            // and retry from the last non wildcard pattern match
            s = ++lastmatch;
            p = lastindex;
        } else {
            return false;
        }
    }

    while (p < plen && pattern[p] === "*") {
        p++;
    }

    return p === plen;
};

const findFile = (path, rootPath = ".") => {
    path = path.split(/[\\/]/).join(sysPath.sep);
    const pos = path.indexOf("*");
    let parent = sysPath.resolve(rootPath);
    const isAbsolute = sysPath.isAbsolute(path);

    do {
        rootPath = parent;
        parent = sysPath.dirname(rootPath);

        if (pos === -1) {
            const rpath = isAbsolute ? sysPath.resolve(path) : sysPath.resolve(rootPath, path);
            try {
                fs.accessSync(rpath, fs.constants.R_OK);
                return rpath;
            } catch (err) {
                continue;
            }
        }

        const start = path.lastIndexOf(sysPath.sep, pos - 1);
        const end = path.indexOf(sysPath.sep, start + 1);
        const prefix = start === -1 ? rootPath : sysPath.resolve(rootPath, path.slice(0, start));
        const pattern = path.slice(start + 1, end === -1 ? path.length : end);
        const suffix = end === -1 ? "" : path.slice(end + 1);

        const candidates = !fs.existsSync(prefix) ? [] : fs.readdirSync(prefix).filter(item => {
            return isMatch(item, pattern);
        });

        for (const candidate of candidates) {
            const next = sysPath.join(prefix, candidate);

            if (end === -1) {
                return next;
            }

            const file = findFile(suffix, next);
            if (file !== null) {
                return file;
            }
        }

        continue;
    } while (!isAbsolute && parent !== rootPath);

    return null;
};

exports.findFile = findFile;

const writeFiles = (files, options, cb) => {
    const doctoc_to_generate = new Set();

    series([
        next => {
            // write files
            eachOfLimit(files.keys(), cpus, (filename, i, next) => {
                waterfall([
                    next => {
                        if (options.save === false) {
                            next();
                            return;
                        }

                        mkdirp(sysPath.dirname(filename)).then(performed => {
                            next();
                        }, next);
                    },

                    next => {
                        fs.readFile(filename, (err, buffer) => {
                            if (err && err.code === "ENOENT") {
                                err = null;
                                buffer = Buffer.from([]);
                            }
                            next(err, buffer);
                        });
                    },

                    (buffer, next) => {
                        const content = eol.lf(files.get(filename));
                        const str = buffer.toString();

                        if (content === str) {
                            next(null, false);
                            return;
                        }

                        console.log("write file", options.output, sysPath.relative(options.output, filename));
                        if (options.save === false) {
                            next(null, false);
                            return;
                        }

                        fs.writeFile(filename, content, err => {
                            if (options.toc !== false && filename.endsWith(".md")) {
                                doctoc_to_generate.add(filename);
                            }
                            next(err, true);
                        });
                    },
                ], next);
            }, next);
        },

        next => {
            if (options.save === false) {
                next();
                return;
            }

            // generate doctoc
            doctoc.transformAndSave(doctoc_to_generate, next);
        },

    ], err => {
        cb(err);
    });
};

exports.writeFiles = writeFiles;

const deleteFiles = (directory, files, options, cb) => {
    files = new Set([...files.keys()]);

    waterfall([
        next => {
            fs.readdir(directory, next);
        },

        (names, next) => {
            eachOfLimit(names, cpus, (filename, i, next) => {
                filename = sysPath.join(directory, filename);

                if (files.has(filename) || ![".h", ".hpp", ".c", ".cc", ".cpp", ".cxx"].some(ext => filename.endsWith(ext))) {
                    next();
                    return;
                }

                console.log("delete file", filename);
                if (options.save === false) {
                    next();
                    return;
                }

                fs.unlink(filename, next);
            }, next);
        }
    ], err => {
        cb(err);
    });
};

exports.deleteFiles = deleteFiles;
