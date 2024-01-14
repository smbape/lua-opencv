#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/ImgTrans/HoughLine/hough_lines.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int

local function main(args)
    -- [load]
    local default_file = 'sudoku.png'
    local filename = args.input

    -- Loads an image
    local src = cv.imread(cv.samples.findFile(filename), cv.IMREAD_GRAYSCALE)

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image!')
        print('Usage: hough_lines.py [image_name -- default ' + default_file + '] \n')
        return -1
    end
    -- [load]

    -- [edge_detection]
    -- Edge detection
    local dst = cv.Canny(src, 50, 200, nil, 3)
    -- [edge_detection]

    -- Copy edges to the images that will display the results in BGR
    local cdst = cv.cvtColor(dst, cv.COLOR_GRAY2BGR)
    local cdstP = cdst:copy()

    -- [hough_lines]
    --  Standard Hough Line Transform
    local lines = cv.HoughLines(dst, 1, math.pi / 180, 150, nil, 0, 0)
    -- [hough_lines]
    -- [draw_lines]
    -- Draw the lines
    if not lines:empty() then
        for i, line in lines:__pairs() do
            local rho, theta = line:__unpack()
            local a = math.cos(theta)
            local b = math.sin(theta)
            local x0 = a * rho
            local y0 = b * rho
            local pt1 = { int(x0 + 1000 * (-b)), int(y0 + 1000 * (a)) }
            local pt2 = { int(x0 - 1000 * (-b)), int(y0 - 1000 * (a)) }

            cv.line(cdst, pt1, pt2, { 0, 0, 255 }, 3, cv.LINE_AA)
        end
    end
    -- [draw_lines]

    -- [hough_lines_p]
    -- Probabilistic Line Transform
    local linesP = cv.HoughLinesP(dst, 1, math.pi / 180, 50, nil, 50, 10)
    -- [hough_lines_p]
    -- [draw_lines_p]
    -- Draw the lines
    if not linesP:empty() then
        for i, l in linesP:__pairs() do
            cv.line(cdstP, { l[0], l[1] }, { l[2], l[3] }, { 0, 0, 255 }, 3, cv.LINE_AA)
        end
    end
    -- [draw_lines_p]
    -- [imshow]
    -- Show results
    cv.imshow("Source", src)
    cv.imshow("Detected Lines (in red) - Standard Hough Line Transform", cdst)
    cv.imshow("Detected Lines (in red) - Probabilistic Line Transform", cdstP)
    -- [imshow]
    -- [exit]
    -- Wait and Exit
    cv.waitKey()
    return 0
    -- [exit]
end

local args = {
    input = 'sudoku.png',
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
