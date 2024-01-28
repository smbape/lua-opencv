const sysPath = require("path");
const ExcelJS = require("exceljs");
const {
    format
} = require("ssf");
const eachOfLimit = require("async/eachOfLimit");
const series = require("async/series");
const {
    main
} = require("../scripts/test");

process.env.BUILD_TYPE = "Release";

// Convert Date object to Microsoft serial date aka ms date aka OA date
const dateToSerial = date => {
    const timezoneOffset = date.getTimezoneOffset() / (60 * 24);
    const msDate = date.getTime() / 86400000 + (25569 - timezoneOffset);
    return msDate;
};

const autoFitColumn = column => {
    const numFmt = column.numFmt;
    let maxLength = 6;
    column.eachCell({
        includeEmpty: true
    }, cell => {
        let columnLength;
        if (numFmt && cell.value !== undefined) {
            switch (cell.type) {
                case ExcelJS.ValueType.Date: {
                    const serialDate = dateToSerial(cell.value);
                    const formattedDate = format(numFmt, serialDate);
                    columnLength = formattedDate.length;
                    break;
                }
                case ExcelJS.ValueType.Number: {
                    const formattedNumber = format(numFmt, cell.value);
                    columnLength = formattedNumber.length;
                    break;
                }
                default: {
                    const formatted = format(numFmt, cell.value);
                    columnLength = formatted.length;
                }
            }
        } else {
            columnLength = cell.text.length;
        }
        maxLength = Math.max(maxLength, columnLength);
    });
    column.width = maxLength + 2;
};

const run = (data, child) => {
    child.stdout.on("data", chunk => {
        for (const line of chunk.toString().split("\n")) {
            const search = " took ";
            const pos = line.indexOf(search);
            if (pos !== -1) {
                const title = line.slice(0, pos);
                const end = line.indexOf(" ", pos + search.length);
                const time = parseFloat(line.slice(pos + search.length, end));
                if (!data.has(title)) {
                    data.set(title, []);
                }
                data.get(title).push(time);
            }
        }
        process.stdout.write(chunk);
    });
    child.stderr.on("data", chunk => {
        process.stderr.write(chunk);
    });
};

const writeFile = async (name, data) => {
    const entries = Array.from(data.entries());

    const workbook = new ExcelJS.Workbook();
    workbook.calcProperties.fullCalcOnLoad = true;

    const worksheet = workbook.addWorksheet("Sheet1");

    worksheet.getCell("A1").value = "Sample";
    for (let i = 0; i < entries.length; i++) {
        worksheet.getCell(`${ String.fromCharCode("B".charCodeAt(0) + i) }1`).value = entries[i][0];

        for (let j = 0; j < entries[i][1].length; j++) {
            worksheet.getCell(`A${ j + 2 }`).value = j + 1;
            worksheet.getCell(`${ String.fromCharCode("B".charCodeAt(0) + i) }${ j + 2 }`).value = entries[i][1][j];
        }
    }

    worksheet.columns.forEach(autoFitColumn);

    await workbook.xlsx.writeFile(sysPath.join(__dirname, `benchmark-${ name }.xlsx`));
}

const lua = new Map();
const python = new Map();

eachOfLimit(Array.from(new Array(100).keys()), 1, (v, k, next) => {
    console.log("test", v);
    series([
        next => {
            main({
                includes: ["perf.lua"],
                stdio: "pipe",
                run: run.bind(null, lua),
            }, next);
        },

        next => {
            main({
                includes: ["perf.py"],
                stdio: "pipe",
                run: run.bind(null, python),
            }, next);
        }
    ], next);
}, async err => {
    if (err) {
        throw err;
    }
    await writeFile("lua", lua);
    await writeFile("python", python);
});
