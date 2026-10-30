local exports = {}

local ffi = require("ffi")

local cast = ffi.cast
local uint8_t = ffi.typeof("uint8_t")
local uint8_ptr_t = ffi.typeof("uint8_t*")
local void_ptr_t = ffi.typeof("void*")
local void_ptr_ptr_t = ffi.typeof("void**")
local void_ptr_array_t = ffi.typeof("void*[1]")
local N = ffi.sizeof(void_ptr_t) / ffi.sizeof(uint8_t)

local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated

local unpack_bytes

if N == 4 then
    unpack_bytes = function(bytes)
        return bytes[0], bytes[1], bytes[2], bytes[3]
    end
elseif N == 8 then
    unpack_bytes = function(bytes)
        return bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5], bytes[6], bytes[7]
    end
else
    -- I have no idea if this is even possible
    -- wrote it juste in case
    unpack_bytes = function(cbytes)
        local lbytes = {}
        for i = 1, N do
            lbytes[i] = cbytes[i - 1]
        end
        return unpack(lbytes)
    end
end

local function tovoid_ptr(cdata)
    return cast(void_ptr_t, cdata)
end
exports.tovoid_ptr = tovoid_ptr

local function get_cdata_bytes(cdata)
    local void_ptr_array = void_ptr_array_t(tovoid_ptr(cdata))
    local bytes = cast(uint8_ptr_t, cast(void_ptr_t, cast(void_ptr_ptr_t, void_ptr_array)))
    return unpack_bytes(bytes)
end
exports.get_cdata_bytes = get_cdata_bytes

return exports
