#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/ImgTrans/MakeBorder/copy_make_border.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int

math.randomseed(os.time())

local function main(args)
    -- [variables]
    -- First we declare the variables we are going to use
    local borderType = cv.BORDER_CONSTANT
    local window_name = "copyMakeBorder Demo"
    -- [variables]
    -- [load]
    local imageName = args.input

    -- Loads an image
    local src = cv.imread(cv.samples.findFile(imageName), cv.IMREAD_COLOR)

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image!')
        print('Usage: copy_make_border.py [image_name -- default lena.jpg] \n')
        return -1
    end
    -- [load]
    -- Brief how-to for this program
    print('\n' ..
        '\t   copyMakeBorder Demo: \n' ..
        '     -------------------- \n' ..
        ' ** Press \'c\' to set the border to a random constant value \n' ..
        ' ** Press \'r\' to set the border to be replicated \n' ..
        ' ** Press \'ESC\' to exit the program ')
    -- [create_window]
    cv.namedWindow(window_name, cv.WINDOW_AUTOSIZE)
    -- [create_window]
    -- [init_arguments]
    -- Initialize arguments for the filter
    local top = int(0.05 * src.rows)
    local bottom = top
    local left = int(0.05 * src.cols)
    local right = left
    -- [init_arguments]
    while true do
        -- [update_value]
        local value = { math.random(0, 255), math.random(0, 255), math.random(0, 255) }
        -- [update_value]
        -- [copymakeborder]
        local dst = cv.copyMakeBorder(src, top, bottom, left, right, borderType, nil, value)
        -- [copymakeborder]
        -- [display]
        cv.imshow(window_name, dst)
        -- [display]
        -- [check_keypress]
        local c = cv.waitKey(500)

        if c == 27 then
            break
        elseif c == 99 then  -- 99 = ord('c')
            borderType = cv.BORDER_CONSTANT
        elseif c == 114 then -- 114 = ord('r')
            borderType = cv.BORDER_REPLICATE
        end
        -- [check_keypress]
    end
    return 0
end


local args = {
    input = 'lena.jpg',
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
