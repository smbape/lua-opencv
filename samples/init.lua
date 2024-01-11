local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

-- lua garbage collector system can keep data for a long time
-- for exemple, displaying camera images increases the memory usage
-- for around 100MB every 10s. This is huge
-- to not put the burden of frequent garbage collection call
-- on the developper, call collectgarbage before calling any opencv_lua function
-- opencv_lua.call_garbage_collect(true)

local sysPath = {
    basename = function (file)
        local index

        for i=#file,1,-1 do
            local c = file:sub(i,i)
            if c == "/" or c == "\\" then
                if i == 1 then return c end
                return file:sub(i + 1, #file)
            end
        end

        return file
    end,

    dirname = function (file)
        local index

        for i=#file,1,-1 do
            local c = file:sub(i,i)
            if c == "/" or c == "\\" then
                if i == 1 then return c end
                return file:sub(1, i - 1)
            end
        end

        if #file == 0 or (file:find(":") == nil and file:sub(1,1) ~= "/") then
            return "."
        end

        return file
    end
}

opencv_lua.path = sysPath

local OPENCV_SAMPLES_DATA_PATH = os.getenv("OPENCV_SAMPLES_DATA_PATH") or sysPath.dirname(opencv_lua.fs_utils.findFile("samples/data/lena.jpg", opencv_lua.kwargs({
    hints = {
        "out/build/x64-Debug/opencv/opencv-src",
        "out/build/x64-Release/opencv/opencv-src",
        "out/build/Linux-GCC-Debug/opencv/opencv-src",
        "out/build/Linux-GCC-Release/opencv/opencv-src",
        "out/prepublish/luajit-2.1/lua-opencv/build.luarocks/opencv/opencv-src",
        "out/prepublish/5.4/lua-opencv/build.luarocks/opencv/opencv-src",
        "out/prepublish/5.3/lua-opencv/build.luarocks/opencv/opencv-src",
        "out/prepublish/5.2/lua-opencv/build.luarocks/opencv/opencv-src",
        "out/prepublish/5.1/lua-opencv/build.luarocks/opencv/opencv-src",
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
