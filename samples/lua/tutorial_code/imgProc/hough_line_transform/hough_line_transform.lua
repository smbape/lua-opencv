#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/imgProc/hough_line_transform/hough_line_transform.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local int = opencv_lua.math.int
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated

local img = cv.imread(cv.samples.findFile('sudoku.png'))
local gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
local edges = cv.Canny(gray, 50, 150, kwargs({ apertureSize = 3 }))

local lines = cv.HoughLines(edges, 1, math.pi / 180, 200)
for _, line in ipairs(lines:table()) do
    local rho, theta = unpack(line)
    local a = math.cos(theta)
    local b = math.sin(theta)
    local x0 = a * rho
    local y0 = b * rho
    local x1 = int(x0 + 1000 * (-b))
    local y1 = int(y0 + 1000 * (a))
    local x2 = int(x0 - 1000 * (-b))
    local y2 = int(y0 - 1000 * (a))

    cv.line(img, { x1, y1 }, { x2, y2 }, { 0, 0, 255 }, 2)
end
cv.imshow('houghlines3.jpg', img)
cv.waitKey()
