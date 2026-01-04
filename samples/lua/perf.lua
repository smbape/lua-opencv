package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

local opencv_lua = require("init")
local cv = opencv_lua.cv
local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global
local inspect = require("inspect")
local common = require("common")

local rows = 512
local cols = 512
local channels = 3
local mat = cv.Mat({ rows, cols, channels }, cv.CV_8U, 255.0)
local times = 5
local total = mat:total() * mat:channels() * times
local t, elapsed, sum

local rng = cv.RNG(cv.getTickCount()) -- Make the starting point unpredictable
rng:fill(mat, cv.RNG.UNIFORM, 0, 256)

local function get_windows_symbols(dll)
    local cmd = string.format([[dumpbin /exports "%s"]], dll:gsub('/', package.config:sub(1, 1)))
    local handle = io.popen(string.format([["%s"]], cmd))
    local symbols = {}
    if handle ~= nil then
        local stdout = handle:read("*all")
        local success = handle:close()
        if success then
            local state = 0
            for line in string.gmatch(stdout, "[^\n]+") do
                if state == 0 then
                    if line:find("ordinal hint RVA") ~= nil then
                        state = 1
                    end
                else
                    local _, _, ordinal, hint, RVA, name, signature = line:find("%s+(%S+)%s+(%S+)%s+(%S+)%s(%S+) = %S+%s%((.+)%)")
                    if name ~= nil then
                        symbols[signature:gsub("cv::debug_build_guard::", "cv::")] = name
                    end
                end
            end
        end
    end
    return symbols
end

local function get_linux_symbols(so)
    local demangled = {}
    local handle = io.popen(string.format([[nm -gDC --defined-only "%s"]], so))
    if handle ~= nil then
        local stdout = handle:read("*all")
        local success = handle:close()
        if success then
            for line in string.gmatch(stdout, "[^\n]+") do
                local _, _, address, type_, signature = line:find("(%S+)%s+(%S+)%s+(%S+))")
                if address ~= nil then
                    demangled[address] = signature:gsub("cv::debug_build_guard::", "cv::")
                end
            end
        end
    end

    local symbols = {}
    local handle = io.popen(string.format([[nm -gD --defined-only "%s"]], so))
    if handle ~= nil then
        local stdout = handle:read("*all")
        local success = handle:close()
        if success then
            for line in string.gmatch(stdout, "[^\n]+") do
                local _, _, address, type_, name = line:find("(%S+)%s+(%S+)%s+(%S+))")
                if address ~= nil and demangled[address] ~= nil then
                    symbols[demangled[address]] = name
                end
            end
        end
    end

    return symbols
end

---@diagnostic disable-next-line: undefined-global
if type(jit) == "table" and package.config:sub(1, 1) ~= "/" then
    -- ffi limitations:
    -- ================================
    -- Function overloads
    -- Custom function overloads (vec -> vec.size(), vec.data())
    -- Default values handling
    -- Inheritance handling
    -- Multi return handling
    -- std types handling: std::string, std::shared_ptr, std::map, std::optional, std::pair, std::tuple, std::variant
    -- stl types handling: std::vector<T>, std::map<K, V>
    -- callbacks handling
    -- Hard to generate. Either demangle c++ symbols or generate c function wrappers

    local ffi = require("ffi")

    local opencv_lua_lib = package.searchpath("opencv_lua", package.cpath)

    local symbols = {}

    if package.config:sub(1, 1) == "/" then
        symbols = get_linux_symbols(opencv_lua_lib)
    else
        symbols = get_windows_symbols(opencv_lua_lib)
    end

    -- http://lua-users.org/wiki/StringTrim
    function trim(s)
       local _, i1 = s:find("^%s*")
       local i2 = s:find("%s*$")
       return s:sub(i1 + 1, i2 - 1)
    end

    -- http://lua-users.org/wiki/StringInterpolation
    local function interpolate(s, tab)
        return (s:gsub('($%b{})', function(w) return tab[trim(w:sub(3, -2))] or w end))
    end

    -- https://lua-l.lua.narkive.com/p3YDBscq/luajit-ffi-c-binding-best-approach
    -- http://lua-users.org/lists/lua-l/2011-01/msg00978.html
    -- http://lua-users.org/lists/lua-l/2011-02/msg01268.html
    ffi.cdef(interpolate([[
        void* malloc(size_t size);
        void free(void* ptr);

        typedef struct {
            union {
                char c_str[16];
                char* data;
            };
            unsigned size;
            unsigned capacity;
        } String;

        const String* getBuildInformation(void) asm("${class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> > const & __cdecl cv::getBuildInformation(void)}");

        typedef struct Mat_t Mat_t;
        typedef struct _InputArray _InputArray;
        void InputArray_createFromMat(_InputArray*, const Mat_t*) asm("${public: __cdecl cv::_InputArray::_InputArray(class cv::Mat const &)}");
        void InputArray_destructor(const _InputArray*) asm("${public: __cdecl cv::_InputArray::~_InputArray(void)}");

        typedef struct {
            double val[4];
        } Scalar;
        void sumElems(Scalar*, const _InputArray *) asm("${class cv::Scalar_<double> __cdecl cv::sum(class cv::_InputArray const &)}");
    ]], symbols))

    -- https://github.com/LuaJIT/LuaJIT/issues/28
    -- ffi.cdef[[
    --     void func_1(int) asm("func");
    --     void func_2(int, int) asm("func");
    -- ]]

    opencv_lua_lib = ffi.load(opencv_lua_lib)

    local function test_getBuildInformation()
        local t, elapsed, buildInformation

        -- local buildInformation = opencv_lua_lib.getBuildInformation()
        -- print("size", buildInformation.size)
        -- print("getBuildInformation", ffi.string(buildInformation.data, buildInformation.size))

        t = cv.getTickCount()
        cv.getBuildInformation()
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::getBuildInformation [C-API] took %.6f seconds", elapsed))

        t = cv.getTickCount()
        for i = 0, rows - 1 do
            for j = 0, cols - 1 do
                for k = 0, channels - 1 do
                    for l = 1, times do
                        buildInformation = cv.getBuildInformation()
                    end
                end
            end
        end
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::getBuildInformation [C-API] %i times took %.6f seconds.", total, elapsed))

        t = cv.getTickCount()
        for i = 0, rows - 1 do
            for j = 0, cols - 1 do
                for k = 0, channels - 1 do
                    for l = 1, times do
                        buildInformation = opencv_lua_lib.getBuildInformation()
                        buildInformation = ffi.string(buildInformation.data, buildInformation.size)
                    end
                end
            end
        end
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::getBuildInformation [FFI]   %i times took %.6f seconds.", total, elapsed))
    end

    test_getBuildInformation()

    local function test_sum()
        local t, elapsed, sum

        t = cv.getTickCount()
        sum = cv.sumElems(mat)[1] * times
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::sum [C-API] took %.6f seconds. Sum = %i", elapsed, sum))

        t = cv.getTickCount()
        for i = 0, rows - 1 do
            for j = 0, cols - 1 do
                for k = 0, channels - 1 do
                    for l = 1, times do
                        sum = cv.sumElems(mat)[1] * times
                    end
                end
            end
        end
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::sum [C-API] %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

        local function InputArray_createFromMat(mat)
            local ptr = ffi.gc(ffi.C.malloc(cv._InputArray.__sizeof), ffi.C.free)
            opencv_lua_lib.InputArray_createFromMat(ptr, mat.__self)
            ffi.gc(ptr, function(ptr)
                opencv_lua_lib.InputArray_destructor(ptr)
                ffi.C.free(ptr)
            end)
            return ptr
        end

        local Scalar = ffi.typeof("Scalar")

        local function sumInputArray(array)
            local scalar = Scalar()
            opencv_lua_lib.sumElems(scalar, array)
            return scalar
        end

        local function sumElems(mat)
            local array = InputArray_createFromMat(mat)
            local scalar = ffi.new("Scalar")
            opencv_lua_lib.sumElems(scalar, array)
            return scalar
        end

        local scalar = Scalar()
        local array = InputArray_createFromMat(mat)
        t = cv.getTickCount()
        for i = 0, rows - 1 do
            for j = 0, cols - 1 do
                for k = 0, channels - 1 do
                    for l = 1, times do
                        opencv_lua_lib.sumElems(scalar, array)
                        sum = scalar.val[0] * times
                    end
                end
            end
        end
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::sum [FFI]   %i times took %.6f seconds. Sum = %i. (Scalar*, const _InputArray *)", total, elapsed, sum))

        if true then return end

        local array = InputArray_createFromMat(mat)
        t = cv.getTickCount()
        for i = 0, rows - 1 do
            for j = 0, cols - 1 do
                for k = 0, channels - 1 do
                    for l = 1, times do
                        sum = sumInputArray(array).val[0] * times
                    end
                end
            end
        end
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::sum [FFI]   %i times took %.6f seconds. Sum = %i. (const _InputArray *)", total, elapsed, sum))

        t = cv.getTickCount()
        for i = 0, rows - 1 do
            for j = 0, cols - 1 do
                for k = 0, channels - 1 do
                    for l = 1, times do
                        sum = sumElems(mat).val[0] * times
                    end
                end
            end
        end
        elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
        collectgarbage()
        print(string.format("cv::sum [FFI]   %i times took %.6f seconds. Sum = %i. (const Mat *)", total, elapsed, sum))
    end

    test_sum()
end

if true then return end

-- if type(jit) == 'table' then
--     t = cv.getTickCount()
--     for i = 0, rows - 1 do
--         for j = 0, cols - 1 do
--             for k = 0, channels - 1 do
--                 for l = 1, times do
--                     bit.band(i, j, k)
--                 end
--                 bit.band(127, i, j, k)
--             end
--         end
--     end
--     elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
--     collectgarbage()
--     print(string.format("bit %i times took %.6f seconds", total, elapsed))
-- end

-- t = cv.getTickCount()
-- for i = 0, rows - 1 do
--     for j = 0, cols - 1 do
--         for k = 0, channels - 1 do
--             for l = 1, times do
--                 opencv_lua.bit.band(i, j, k)
--             end
--             opencv_lua.bit.band(127, i, j, k)
--         end
--     end
-- end
-- elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
-- collectgarbage()
-- print(string.format("opencv_lua.bit %i times took %.6f seconds", total, elapsed))

t = cv.getTickCount()
local bin = cv.Mat.zeros({ 20, 20 }, cv.CV_32S)
local mag = cv.Mat.zeros({ 20, 20 }, cv.CV_32S)
for i = 1, 50000 do
    local bin_cells = {
        bin:new({ 0, 0, 10, 10 }),
        bin:new({ 10, 0, 10, 10 }),
        bin:new({ 0, 10, 10, 10 }),
        bin:new({ 10, 10, 10, 10 }),
    }

    local mag_cells = {
        mag:new({ 0, 0, 10, 10 }),
        mag:new({ 10, 0, 10, 10 }),
        mag:new({ 0, 10, 10, 10 }),
        mag:new({ 10, 10, 10, 10 }),
    }
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("calling new %d times on a %ix%i matrix took %.6f seconds", 50000 * 4 * 2, bin.rows, bin.cols, elapsed))

if cv.Range.MAX then
    t = cv.getTickCount()
    local bin = cv.Mat.zeros({ 20, 20 }, cv.CV_32S)
    local mag = cv.Mat.zeros({ 20, 20 }, cv.CV_32S)
    for i = 1, 50000 do
        local bin_cells = {
            bin[{ { 0, 10 }, { 0, 10 } }],
            bin[{ { 10, cv.Range.MAX }, { 0, 10 } }],
            bin[{ { 0, 10 }, { 10, cv.Range.MAX } }],
            bin[{ { 10, cv.Range.MAX }, { 10, cv.Range.MAX } }]
        }

        local mag_cells = {
            mag[{ { 0, 10 }, { 0, 10 } }],
            mag[{ { 10, cv.Range.MAX }, { 0, 10 } }],
            mag[{ { 0, 10 }, { 10, cv.Range.MAX } }],
            mag[{ { 10, cv.Range.MAX }, { 10, cv.Range.MAX } }]
        }
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("slicing %d times on a %ix%i matrix took %.6f seconds", 50000 * 4 * 2, bin.rows, bin.cols, elapsed))
end

t = cv.getTickCount()
sum = {}
for i = 1, 512 do
    sum[i] = {}
    local tbl = sum[i]
    for j = 1, 512 do
        tbl[j] = {}
        local tbl = tbl[j]
        for k = 1, 3 do
            tbl[k] = {}
            local tbl = tbl[k]
            for l = 1, 2 do
                tbl[l] = 1
            end
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("lua nested matrix of %i elements took %.6f seconds", #sum * #sum[1] * #sum[1][1] * #sum[1][1][1], elapsed))

t = cv.getTickCount()
sum = {}
for i = 1, 512 do
    for j = 1, 512 do
        for k = 1, 3 do
            for l = 1, 2 do
                sum[#sum + 1] = 1
            end
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("lua flat matrix of %i elements took %.6f seconds", #sum, elapsed))

t = cv.getTickCount()
sum = cv.Mat({ rows, cols, channels, 2 }, cv.CV_32F, 1.0)
sum:table()
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("c++ -> lua nested matrix of %d elements took %.6f seconds", sum:total(), elapsed))

t = cv.getTickCount()
sum = cv.Mat({ rows, cols, channels, 2 }, cv.CV_32F, 1.0)
sum:table(false)
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("c++ -> lua flat matrix of %d elements took %.6f seconds", sum:total(), elapsed))

t = cv.getTickCount()
sum = 0
for i = 0, rows - 1 do
    for j = 0, cols - 1 do
        for k = 0, channels - 1 do
            for l = 1, times do
                sum = sum + mat(i, j, k)
            end
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("mat(i, j, k) %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

t = cv.getTickCount()
sum = 0
for i = 0, rows - 1 do
    for j = 0, cols - 1 do
        for k = 0, channels - 1 do
            for l = 1, times do
                sum = sum + mat[{ i, j, k }]
            end
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("mat[{i, j, k}] %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

t = cv.getTickCount()
sum = 0
for i = 0, rows - 1 do
    for j = 0, cols - 1 do
        for k = 0, channels - 1 do
            for l = 1, times do
                sum = sum + mat:get(i, j, k)
            end
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("mat:get(i, j, k) %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

local get = cv.Mat.get
t = cv.getTickCount()
sum = 0
for i = 0, rows - 1 do
    for j = 0, cols - 1 do
        for k = 0, channels - 1 do
            for l = 1, times do
                sum = sum + get(mat, i, j, k)
            end
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("get(mat, i, j, k) %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

t = cv.getTickCount()
local data = mat:table()
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("table %s took %.6f seconds", inspect(mat.shape), elapsed))

t = cv.getTickCount()
sum = 0
for i = 1, rows do
    for j = 1, cols do
        for k = 1, channels do
            for l = 1, times do
                sum = sum + data[i][j][k]
            end
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("data[i][j][k] %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

t = cv.getTickCount()
local result = cv.Mat.createFromArray(data, mat:type())
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("createFromArray %s took %.6f seconds. Sum = %d", inspect(mat.shape), elapsed, cv.sumElems(result)[1] * times))

;(function ()
    -- https://luajit.org/ext_ffi.html
    -- Do the check to see if JIT is enabled. If so use the optimized FFI structs.
    if type(jit) ~= 'table' then ---@diagnostic disable-line: undefined-global
        return
    end

    -- print(jit.version)  --LuaJIT 2.0.2

    local status, ffi = pcall(require, "ffi")

    local function slice(tbl, start)
        local result = {}
        for i = start, #tbl do
            result[#result + 1] = tbl[i]
        end
        return result
    end

    if not status then
        return
    end

    local ctypes = {
        [cv.CV_8U] = "unsigned char",
        [cv.CV_8S] = "char",
        [cv.CV_16U] = "unsigned short",
        [cv.CV_16S] = "short",
        [cv.CV_32S] = "int",
        [cv.CV_32F] = "float",
        [cv.CV_64F] = "double",
    }

    local Mat = common.class({
        __init__ = function(self, mat)
            self.m = mat
            self.sizes = mat.sizes
            self.channels = mat:channels()
            self.data = ffi.cast(ctypes[mat:depth()] .. "*", mat.data)
        end,

        __tostring = function(self)
            return tostring(self.m)
        end,
    })

    function Mat.__call(self, ...)
        return Mat.__index(self, { ... })
    end

    function Mat.__index(self, key)
        local v = Mat[key]
        if v ~= nil then
            return v
        end

        local idx = 0
        local size = 1
        for i = #self.sizes, 1, -1 do
            idx = idx + key[i] * size
            size = size * self.sizes[i]
        end

        return self.data[idx]
    end

    t = cv.getTickCount()
    local ffi_data = Mat(mat)
    sum = 0
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            for k = 0, channels - 1 do
                for l = 1, times do
                    sum = sum + ffi_data(i, j, k)
                end
            end
        end
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("ffi_data(i, j, k) %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

    t = cv.getTickCount()
    sum = 0
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            for k = 0, channels - 1 do
                for l = 1, times do
                    sum = sum + ffi_data[{ i, j, k }]
                end
            end
        end
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("ffi_data[{i, j, k}] %i times took %.6f seconds. Sum = %i", total, elapsed, sum))


    ffi.cdef("typedef struct { uint8_t blue, green, red; } brg_pixel_t;")
    ffi.metatype("brg_pixel_t", {
        __tostring = function(pixel)
            return string.format("[%i, %i, %i]", pixel.blue, pixel.green, pixel.red)
        end,
    })

    ffi.cdef("typedef struct { int* p; int dims; } MatSize;")
    ffi.metatype("MatSize", {
        __tostring = function(size)
            local str = "["
            for i = 0, size.dims - 1 do
                if i ~= 0 then str = str .. ", " end
                str = str .. size.p[i]
            end
            str = str .. "]"
            return str
        end,
    })

    local p

    local Mat = {
        sizes = function(self, sizes)
            p = ffi.new("int[?]", #sizes) -- TODO find a way to keep reference until gc
            self.size = ffi.new("MatSize")
            self.size.p = p
            for i = 0, #sizes - 1 do
                self.size.p[i] = sizes[i + 1]
            end
            self.size.dims = #sizes
        end,

        data = function(self, ctype, data)
            self._data = ffi.cast(ctype .. "*", data)
        end,
    }

    function Mat.__call(self, ...)
        return Mat.__index(self, { ... })
    end

    function Mat.__index(self, k)
        local v = Mat[k]
        if v ~= nil then
            return v
        end

        local idx = 0
        local size = 1
        for i = self.size.dims - 1, 0, -1 do
            idx = idx + k[i + 1] * size
            size = size * self.size.p[i]
        end
        return self._data[idx]
    end

    ffi.cdef("typedef struct { MatSize size; " .. ctypes[mat:depth()] .. "* _data; } Mat;")
    ffi.metatype("Mat", Mat)

    local ffi_mat = ffi.new("Mat")
    ffi_mat:sizes(mat.sizes)
    ffi_mat:data(ctypes[mat:depth()], mat.data)

    t = cv.getTickCount()
    sum = 0
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            for k = 0, channels - 1 do
                for l = 1, times do
                    sum = sum + ffi_mat(i, j, k)
                end
            end
        end
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("ffi_mat(i, j, k) %i times took %.6f seconds. Sum = %i", total, elapsed, sum))


    t = cv.getTickCount()
    sum = 0
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            for k = 0, channels - 1 do
                for l = 1, times do
                    sum = sum + ffi_mat[{ i, j, k }]
                end
            end
        end
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("ffi_mat[{i, j, k}] %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

    t = cv.getTickCount()
    sum = 0
    local bytes = ffi.cast(ctypes[mat:depth()] .. "*", mat.data)
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            for k = 0, channels - 1 do
                for l = 1, times do
                    sum = sum + bytes[i * cols * channels + j * channels + k]
                end
            end
        end
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("bytes[i * cols * channels + j * channels + k] %i times took %.6f seconds. Sum = %i", total, elapsed, sum))

    local matrix_t = ffi.typeof("$[$][$][$]", ffi.typeof(ctypes[mat:depth()]), rows, cols, channels)
    local matrix_ptr = ffi.cast(ffi.typeof("$*", matrix_t), mat.data)
    local bytes = matrix_ptr[0]
    t = cv.getTickCount()
    sum = 0
    local matrix = ffi.cast(ffi.typeof("$*", matrix_t), mat.data)
    local bytes = matrix[0]
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            for k = 0, channels - 1 do
                for l = 1, times do
                    sum = sum + bytes[i][j][k]
                end
            end
        end
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("bytes[i][j][k] %i times took %.6f seconds. Sum = %i", total, elapsed, sum))
end)();
