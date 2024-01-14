#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/Pyramids/pyramids.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

local function main(args)
    print([[
    Zoom In-Out demo
    ------------------
    * [i] -> Zoom [i]n
    * [o] -> Zoom [o]ut
    * [ESC] -> Close program
    ]])
    -- [load]
    local filename = args.input

    -- Load the image
    local src = cv.imread(cv.samples.findFile(filename))

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image!')
        print('Usage: pyramids.py [image_name -- default ../data/chicky_512.png] \n')
        return -1
    end
    -- [load]
    -- [loop]
    while true do
        local rows, cols, _channels = src.rows, src.cols, src:channels()
        -- [show_image]
        cv.imshow('Pyramids Demo', src)
        -- [show_image]
        local k = cv.waitKey(0)

        if k == 27 then
            break
            -- [pyrup]
        elseif k == string.byte('i') then
            src = cv.pyrUp(src, kwargs({ dstsize = { 2 * cols, 2 * rows } }))
            print('** Zoom In: Image x 2')
            -- [pyrup]
            -- [pyrdown]
        elseif k == string.byte('o') then
            src = cv.pyrDown(src, kwargs({ dstsize = { math.floor(cols / 2), math.floor(rows / 2) } }))
            print('** Zoom Out: Image / 2')
            -- [pyrdown]
        end
    end
    -- [loop]

    cv.destroyAllWindows()
    return 0
end

local args = {
    input = "chicky_512.png",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

main(args)
