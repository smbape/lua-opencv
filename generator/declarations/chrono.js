const DURATION_TYPES = [
    "nanoseconds",
    "microseconds",
    "milliseconds",
    "seconds",
    "minutes",
    "hours",
    "days",
    "weeks",
    // "months", // common type with other DURATION_TYPES is outside DURATION_TYPES
    // "years", // common type with other DURATION_TYPES is outside DURATION_TYPES
];

const commonDurationType = (period1, period2) => {
    return DURATION_TYPES.indexOf(period1) <= DURATION_TYPES.indexOf(period2) ? period1 : period2;
};

module.exports = [
    ["class std.chrono.steady_clock.time_point", "", [], [], "", ""],

    /////////////////////////////////////////////////////////////////////////
    // https://en.cppreference.com/w/cpp/chrono/time_point/operator_arith2 //
    /////////////////////////////////////////////////////////////////////////

    ["std.chrono.steady_clock.time_point.operator-", "std::chrono::steady_clock::duration", [], [
        ["std::chrono::steady_clock::time_point", "pt_rhs", "", ["/C", "/Ref"]],
    ], "", ""],

    ["std.chrono.steady_clock.time_point.operator-", "std::chrono::steady_clock::duration", ["/S"], [
        ["std::chrono::steady_clock::time_point", "pt_lhs", "", ["/C", "/Ref"]],
        ["std::chrono::steady_clock::time_point", "pt_rhs", "", ["/C", "/Ref"]],
    ], "", ""],

    //////////////////////////////////////////////////////////////////////
    // https://en.cppreference.com/w/cpp/chrono/time_point/operator_cmp //
    //////////////////////////////////////////////////////////////////////

    ... [ "==", "<", "<=" ].map(op  => [
        [`std.chrono.steady_clock.time_point.operator${ op }`, "bool", [], [
            ["std::chrono::steady_clock::time_point", "rhs", "", ["/C", "/Ref"]],
        ], "", ""],

        [`std.chrono.steady_clock.time_point.operator${ op }`, "bool", ["/S"], [
            ["std::chrono::steady_clock::time_point", "lhs", "", ["/C", "/Ref"]],
            ["std::chrono::steady_clock::time_point", "rhs", "", ["/C", "/Ref"]],
        ], "", ""],
    ]).flat(),

    ["std.chrono.steady_clock.now", "std::chrono::steady_clock::time_point", ["/S"], [], "", ""],

    ["std.this_thread.sleep_until", "void", [], [
        ["std::chrono::steady_clock::time_point", "sleep_time", "", ["/C", "/Ref"]],
    ], "", ""],

    ... DURATION_TYPES.map((period, i) => [
        [`class std.chrono.${ period }`, "", [], [], "", ""],

        [`std.chrono.${ period }.count`, `std::chrono::${ period }::rep`, [], [], "", ""],

        ////////////////////////////////////////////////////////////////
        // https://en.cppreference.com/w/cpp/chrono/duration/duration //
        ////////////////////////////////////////////////////////////////

        [`std.chrono.${ period }.${ period }`, "", [], [
            [`std::chrono::${ period }::rep`, "d", "", ["/C"]],
        ], "", ""],

        ... DURATION_TYPES.slice(i).map(period2  => [
            [`std.chrono.${ period }.${ period }`, "", [], [
                [`std::chrono::${ period2 }`, "d", "", ["/C", "/Ref"]],
            ], "", ""],
        ]).flat(),

        /////////////////////////////////////////////////////////////////////////
        // https://en.cppreference.com/w/cpp/chrono/time_point/operator_arith2 //
        /////////////////////////////////////////////////////////////////////////

        ["std.chrono.steady_clock.time_point.operator+", "std::chrono::steady_clock::time_point", [], [
            [`std::chrono::${ period }`, "d", "", ["/C", "/Ref"]],
        ], "", ""],

        ["std.chrono.steady_clock.time_point.operator+", "std::chrono::steady_clock::time_point", ["/S"], [
            ["std::chrono::steady_clock::time_point", "pt", "", ["/C", "/Ref"]],
            [`std::chrono::${ period }`, "d", "", ["/C", "/Ref"]],
        ], "", ""],

        [`std.chrono.${ period }.operator+`, "std::chrono::steady_clock::time_point", [], [
            ["std::chrono::steady_clock::time_point", "pt", "", ["/C", "/Ref"]],
        ], "", ""],

        [`std.chrono.${ period }.operator+`, "std::chrono::steady_clock::time_point", ["/S"], [
            [`std::chrono::${ period }`, "d", "", ["/C", "/Ref"]],
            ["std::chrono::steady_clock::time_point", "pt", "", ["/C", "/Ref"]],
        ], "", ""],

        ["std.this_thread.sleep_for", "void", [], [
            [`std::chrono::${ period }`, "sleep_duration", "", ["/C", "/Ref"]],
        ], "", ""],

        ///////////////////////////////////////////////////////////////////////
        // https://en.cppreference.com/w/cpp/chrono/duration/operator_arith4 //
        ///////////////////////////////////////////////////////////////////////

        ... DURATION_TYPES.map(period2  => [
            ... [ "+", "-" ].map(op  => [
                [`std.chrono.${ period }.operator${ op }`, `std::chrono::${ commonDurationType(period, period2) }`, [], [
                    [`std::chrono::${ period2 }`, "rhs", "", ["/C", "/Ref"]],
                ], "", ""],

                [`std.chrono.${ period }.operator${ op }`, `std::chrono::${ commonDurationType(period, period2) }`, ["/S"], [
                    [`std::chrono::${ period }`, "lhs", "", ["/C", "/Ref"]],
                    [`std::chrono::${ period2 }`, "rhs", "", ["/C", "/Ref"]],
                ], "", ""],
            ]).flat(),

            ... [ "==", "<", "<=" ].map(op  => [
                [`std.chrono.${ period }.operator${ op }`, "bool", [], [
                    [`std::chrono::${ period2 }`, "rhs", "", ["/C", "/Ref"]],
                ], "", ""],

                [`std.chrono.${ period }.operator${ op }`, "bool", ["/S"], [
                    [`std::chrono::${ period }`, "lhs", "", ["/C", "/Ref"]],
                    [`std::chrono::${ period2 }`, "rhs", "", ["/C", "/Ref"]],
                ], "", ""],
            ]).flat(),
        ]).flat(),

        ... [ "*", "/" ].map(op  => [
            [`std.chrono.${ period }.operator${ op }`, `std::chrono::${ period }`, [], [
                [`std::chrono::${ period }::rep`, "s", "", []],
            ], "", ""],

            [`std.chrono.${ period }.operator${ op }`, `std::chrono::${ period }`, ["/S"], [
                [`std::chrono::${ period }`, "lhs", "", ["/C", "/Ref"]],
                [`std::chrono::${ period }::rep`, "s", "", []],
            ], "", ""],
        ]).flat(),
    ]).flat(),
];
