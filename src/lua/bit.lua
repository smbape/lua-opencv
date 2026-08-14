local function load_bit(bit)
    ---Returns the bitwise not of its argument.
    ---@param x integer
    ---@return integer
    function bit.bnot(x) return ~x end

    ---Returns the bitwise <b>or</b> of all of its arguments.
    ---@param x1 integer
    ---@param ... integer
    ---@return integer
    function bit.bor(x1, ...)
        local t, n = { ... }, select('#', ...)
        for i = 1, n do
            x1 = x1 | t[i]
        end
        return x1
    end

    ---Returns the bitwise <b>and</b> of all of its arguments.
    ---@param x1 integer
    ---@param ... integer
    ---@return integer
    function bit.band(x1, ...)
        local t, n = { ... }, select('#', ...)
        for i = 1, n do
            x1 = x1 & t[i]
        end
        return x1
    end

    ---Returns the bitwise <b>logical left-shift</b> of its first argument by the number of bits given by the second argument. 
    ---@param x integer
    ---@param n integer
    ---@return integer
    function bit.lshift(x, n) return x << n end

    ---Returns the bitwise <b>logical right-shift</b> of its first argument by the number of bits given by the second argument. 
    ---@param x integer
    ---@param n integer
    ---@return integer
    function bit.rshift(x, n) return x >> n end
end

return load_bit
