#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/ImgTrans/HoughLine/hough_lines.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated
local INDEX_BASE = 1 -- lua is 1-based indexed

local function main(args)
    -- [load]
    local default_file = 'sudoku.png'
    local filename = args.input

    -- Loads an image
    local src = cv.imread(cv.samples.findFile(filename), cv.IMREAD_GRAYSCALE)

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image!')
        print('Usage: hough_lines.lua [image_name -- default ' + default_file + '] \n')
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
        for i, line in ipairs(lines:table()) do
            local rho, theta = unpack(line)
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
        for i, l in ipairs(linesP:table()) do
            cv.line(cdstP, { l[0 + INDEX_BASE], l[1 + INDEX_BASE] }, { l[2 + INDEX_BASE], l[3 + INDEX_BASE] }, { 0, 0, 255 }, 3, cv.LINE_AA)
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

local parser = argparse() {description='This program demonstrates line finding with the Hough transform'}
parser:argument('input'):description('Path to input image.'):default('sudoku.png')
local args = parser:parse()

main(args)
