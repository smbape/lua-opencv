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

module.exports = [
    ["enum class std.cv_status", "", [], [
        ["const std.cv_status.no_timeout", "", []],
        ["const std.cv_status.timeout", "", []],
    ], "", ""],

    ["class std.condition_variable", "", [], [], "", ""],

    ["std.condition_variable.condition_variable", "", [], [], "", ""],

    ["std.condition_variable.notify_one", "void", [], [], "", ""],
    ["std.condition_variable.notify_all", "void", [], [], "", ""],

    ["std.condition_variable.wait", "void", ["/Expr=*get_thread_gil(L)"], [], "", ""],
    ["std.condition_variable.wait", "void", ["/Expr=*get_thread_gil(L), $0"], [
        ["std::function<bool()>", "pred", "", []]
    ], "", ""],

    ...DURATION_TYPES.map((period, i) => [

        ["std.condition_variable.wait_for", "std::cv_status", ["/Expr=*get_thread_gil(L), $0"], [
            [`std::chrono::${ period }`, "rel_time", "", ["/C", "/Ref"]],
        ], "", ""],
        ["std.condition_variable.wait_for", "bool", ["/Expr=*get_thread_gil(L), $0"], [
            [`std::chrono::${ period }`, "rel_time", "", ["/C", "/Ref"]],
            ["std::function<bool()>", "pred", "", []]
        ], "", ""],

    ]).flat(),

    ["std.condition_variable.wait_until", "std::cv_status", ["/Expr=*get_thread_gil(L), $0"], [
        ["std::chrono::steady_clock::time_point", "abs_time", "", ["/C", "/Ref"]],
    ], "", ""],
    ["std.condition_variable.wait_until", "bool", ["/Expr=*get_thread_gil(L), $0"], [
        ["std::chrono::steady_clock::time_point", "abs_time", "", ["/C", "/Ref"]],
        ["std::function<bool()>", "pred", "", []]
    ], "", ""],
];
