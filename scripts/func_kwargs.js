const genFunc = (libname, fname, args) => {
    let mandatory = true;
    let kwargs;
    const fdecl = ["function"];
    if (libname === "local") {
        kwargs = `${ libname }.kwargs`;
        fdecl.unshift(libname);
        fdecl.push(fname);
    } else {
        kwargs = "opencv_lua.kwargs";
        fdecl.push(`${ libname }.${ fname }`);
    }

    return `
${ fdecl.join(" ") } ( ... )
    local args={n=select("#", ...), ...}
    local has_kwarg = ${ kwargs }.isinstance(args[args.n])
    local kwargs = has_kwarg and args[args.n] or ${ kwargs }()
    local usedkw = 0

    ${ args.map((decl, i) => {
        let argname, kwname, defval, _mandatory;

        if (Array.isArray(decl)) {
            [argname, kwname, defval, _mandatory] = decl;
        } else {
            argname = decl;
        }

        if (mandatory) {
            if (_mandatory != null) {
                mandatory = _mandatory;
            } else if (defval != null) {
                mandatory = false;
            }
        }

        return `
            -- get argument ${ kwname || argname }
            local ${ argname }${ defval == null ? "" : ` = ${ defval }` }
            local has_${ argname } = false
            if (not has_kwarg) or args.n > ${ i + 1 } then
                -- positional parameter should not be a named parameter
                if has_kwarg and kwargs:has("${ kwname || argname }") then
                    error("${ kwname || argname } was both specified as a Positional and NamedParameter")
                end
                has_${ argname } = args.n >= ${ i + 1 }
                if has_${ argname } then
                    ${ argname } = args[${ i + 1 }]
                end
            elseif kwargs:has("${ kwname || argname }") then
                -- named parameter
                has_${ argname } = true
                ${ argname } = kwargs:get("${ kwname || argname }")
                usedkw = usedkw + 1
            elseif ${ mandatory } then
                error("${ kwname || argname } is mandatory")
            end
        `.replace(/^ {8}/mg, "").replace("elseif true then", "else").replace(/\n[^\S\n]+elseif false then\n[^\n]+/, "").trim();
    }).join(`\n\n${ " ".repeat(4) }`) }

    if usedkw ~= kwargs:size() then
        error("there are " .. (kwargs:size() - usedkw) .. " unknown named parameters")
    end

    --- ====================== ---
    --- CODE LOGIC STARTS HERE ---
    --- ====================== ---
end
`.trim();
};

const [,, libname, fname, ...args] = process.argv;
console.log(genFunc(libname, fname, args.map(arg => (/^[['"]/.test(arg) ? eval(arg) : arg)))); // eslint-disable-line no-eval
