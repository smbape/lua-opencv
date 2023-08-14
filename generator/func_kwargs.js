const genFunc = (libname, fname, args) => {
    let mandatory = true;

    return `
function ${ libname }.${ fname } ( ... )
    local args={...}
    local has_kwarg = ${ libname }.kwargs.is_instance(args[#args])
    local kwargs = has_kwarg and args[#args] or ${ libname }.kwargs()
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
            if (not has_kwarg) or #args > ${ i + 1 } then
                -- positional parameter should not be a named parameter
                if has_kwarg and kwargs:has("${ kwname || argname }") then
                    error('${ kwname || argname } was both specified as a Positional and NamedParameter')
                end
                has_${ argname } = #args >= ${ i + 1 }
                if has_${ argname } then
                    ${ argname } = args[${ i + 1 }]
                end
            elseif kwargs:has("${ kwname || argname }") then
                -- named parameter
                has_${ argname } = true
                ${ argname } = kwargs:get("${ kwname || argname }")
                usedkw = usedkw + 1
            elseif ${ mandatory } then
                error('${ kwname || argname } is mandatory')
            end
        `.replace(/^ {8}/mg, "").trim();
    }).join(`\n\n${ " ".repeat(4) }`) }

    if not (usedkw == kwargs:size()) then
        error('there are ' .. (kwargs:size() - usedkw) .. ' unknown named parameters')
    end

    -- ... YOUR CODE HERE
end
`.trim();
};

const [,, libname, fname, ...args] = process.argv;
console.log(genFunc(libname, fname, args.map(arg => (/^[\['"]/.test(arg) ? eval(arg) : arg))));
