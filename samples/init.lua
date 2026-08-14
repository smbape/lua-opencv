local lfs = require("lfs")
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

local sysPath = {}

sysPath.abspath = opencv_lua.fs_utils.absolute

sysPath.basename = function(filepath)
    for i = #filepath, 1, -1 do
        local c = filepath:sub(i, i)
        if c == "/" or c == "\\" then
            if i == 1 then return c end
            return filepath:sub(i + 1, #filepath)
        end
    end

    return filepath
end

sysPath.dirname = function(filepath)
    for i = #filepath, 1, -1 do
        local c = filepath:sub(i, i)
        if c == "/" or c == "\\" then
            if i == 1 then return c end
            return filepath:sub(1, i - 1)
        end
    end

    if #filepath == 0 or (filepath:find(":") == nil and filepath:sub(1, 1) ~= "/") then
        return "."
    end

    return filepath
end

sysPath.exists = opencv_lua.fs_utils.exists

sysPath.isfile = function(filepath)
    return lfs.attributes(filepath, "mode") ~= "directory"
end

-- https://github.com/moteus/lua-path/blob/v0.3.1/lua/path.lua

local DIR_SEP = package.config:sub(1, 1)
local IS_WINDOWS = DIR_SEP == '\\'

---@param s string
---@param start string
---@return boolean
local startswith = function(s, start)
    return s:sub(1, #start) == start
end

function sysPath.isfullpath(filepath)
    if type(filepath) ~= "string" then
        return false
    end

    if not IS_WINDOWS then
        return startswith(filepath, "/")
    end

    filepath = filepath:gsub("[\\/]", DIR_SEP)

    -- https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats
    -- Example: Ways to refer to the same file
    -- @"c:\temp\test-file.txt",
    -- @"\\127.0.0.1\c$\temp\test-file.txt",
    -- @"\\LOCALHOST\c$\temp\test-file.txt",
    -- @"\\.\c:\temp\test-file.txt",
    -- @"\\?\c:\temp\test-file.txt",
    -- @"\\.\UNC\LOCALHOST\c$\temp\test-file.txt"

    if startswith(filepath, "\\\\.\\UNC\\") then
        filepath = "\\" .. filepath:sub(8 --[[ len("\\.\UNC") + 1 --]], #filepath)
    elseif startswith(filepath, "\\\\.\\") or startswith(filepath, "\\\\?\\") then
        filepath = filepath:sub(5 --[[ len("\\.\") + 1 --]], #filepath)
    end

    return startswith(filepath, "\\\\") or filepath:match("^[^\\]+:\\") ~= nil
end

function sysPath.remove_dir_end(filepath)
    return (string.gsub(filepath, '[\\/]+$', ''))
end

function sysPath.ensure_dir_end(filepath)
    return sysPath.remove_dir_end(filepath) .. DIR_SEP
end

function sysPath.join_(filepath1, filepath2)
    local ch = filepath2:sub(1, 1)
    if ch == "\\" or ch == "/" then
        return sysPath.remove_dir_end(filepath1) .. filepath2
    end
    return sysPath.ensure_dir_end(filepath1) .. filepath2
end

sysPath.join = function(...)
    local t, n = { ... }, select('#', ...)
    local filepath = t[1]
    for i = 2, n do
        if sysPath.isfullpath(t[i]) then
            filepath = t[i]
        else
            filepath = sysPath.join_(filepath, t[i])
        end
    end
    return filepath
end

opencv_lua.path = sysPath

local OPENCV_SAMPLES_DATA_PATH = os.getenv("OPENCV_SAMPLES_DATA_PATH") or
    sysPath.dirname(opencv_lua.fs_utils.findFile("samples/data/lena.jpg", opencv_lua.kwargs({
        hints = {
            "out/build/x64-Debug/opencv/opencv-src",
            "out/build/x64-Release/opencv/opencv-src",
            "out/build/Linux-GCC-Debug/opencv/opencv-src",
            "out/build/Linux-GCC-Release/opencv/opencv-src",
            "out/prepublish/build/opencv_lua/build.luarocks/opencv/opencv-src",
            "opencv",
        }
    })))
cv.samples.addSamplesDataSearchPath(OPENCV_SAMPLES_DATA_PATH)

local env = {
    OPENCV_SAMPLES_DATA_PATH = OPENCV_SAMPLES_DATA_PATH,
}

opencv_lua.env = env

local LOCAL_SAMPLES_DATA_PATH = opencv_lua.fs_utils.findFile("samples/data")
cv.samples.addSamplesDataSearchPath(LOCAL_SAMPLES_DATA_PATH)

return opencv_lua
