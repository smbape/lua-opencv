#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/ImgTrans/HoughCircle/hough_circle.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local INDEX_BASE = 1 -- lua is 1-based indexed

local function main(args)
    -- [load]
    local default_file = 'smarties.png'
    local filename = args.input

    -- Loads an image
    local src = cv.imread(cv.samples.findFile(filename), cv.IMREAD_COLOR)

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image!')
        print('Usage: hough_circle.lua [image_name -- default ' + default_file + '] \n')
        return -1
    end
    -- [load]

    -- [convert_to_gray]
    -- Convert it to gray
    local gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    -- [convert_to_gray]

    -- [reduce_noise]
    -- Reduce the noise to avoid false circle detection
    gray = cv.medianBlur(gray, 5)
    -- [reduce_noise]

    -- [houghcircles]
    local rows = gray.rows
    local circles = cv.HoughCircles(gray, cv.HOUGH_GRADIENT, 1, rows / 8,
        kwargs({
            param1 = 100,
            param2 = 30,
            minRadius = 1,
            maxRadius = 30
        }))
    -- [houghcircles]

    -- [draw]
    if not circles:empty() then
        circles = circles:convertTo(cv.CV_16U)
        for _, i in ipairs(circles[0]:table()) do
            local center = { i[0 + INDEX_BASE], i[1 + INDEX_BASE] }
            -- circle center
            cv.circle(src, center, 1, { 0, 100, 100 }, 3)
            -- circle outline
            local radius = i[2 + INDEX_BASE]
            cv.circle(src, center, radius, { 255, 0, 255 }, 3)
        end
    end
    -- [draw]

    -- [display]
    cv.imshow("detected circles", src)
    cv.waitKey(0)
    -- [display]

    return 0
end


local parser = argparse() {}
parser:argument('input'):description('Path to input image.'):default('smarties.png')
local args = parser:parse()

main(args)
