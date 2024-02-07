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

print(string.format("Expected Sum = %i", cv.sumElems(mat)[1] * times))

if type(jit) == 'table' then
    t = cv.getTickCount()
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            for k = 0, channels - 1 do
                for l = 1, times do
                    bit.band(i, j, k)
                end
                bit.band(127, i, j, k)
            end
        end
    end
    elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
    collectgarbage()
    print(string.format("bit %i times took %.6f seconds", total, elapsed))
end

t = cv.getTickCount()
for i = 0, rows - 1 do
    for j = 0, cols - 1 do
        for k = 0, channels - 1 do
            for l = 1, times do
                opencv_lua.bit.band(i, j, k)
            end
            opencv_lua.bit.band(127, i, j, k)
        end
    end
end
elapsed = (cv.getTickCount() - t) / cv.getTickFrequency()
collectgarbage()
print(string.format("opencv_lua.bit %i times took %.6f seconds", total, elapsed))

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

-- https://luajit.org/ext_ffi.html
-- Do the check to see if JIT is enabled. If so use the optimized FFI structs.
if type(jit) == 'table' then
    -- print(jit.version)  --LuaJIT 2.0.2

    local status, ffi = pcall(require, "ffi")

    local function slice(tbl, start)
        local result = {}
        for i = start, #tbl do
            result[#result + 1] = tbl[i]
        end
        return result
    end

    if status then
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

            data = function(self, data)
                self._data = ffi.cast("unsigned char*", data)
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

        ffi.cdef("typedef struct { MatSize size; unsigned char* _data; } Mat;")
        ffi.metatype("Mat", Mat)

        local ffi_mat = ffi.new("Mat")
        ffi_mat:sizes(mat.sizes)
        ffi_mat:data(mat.data)

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
        local bytes = ffi.cast("unsigned char*", mat.data)
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
    end
end
