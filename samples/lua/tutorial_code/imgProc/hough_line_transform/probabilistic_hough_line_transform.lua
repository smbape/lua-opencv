#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/imgProc/hough_line_transform/probabilistic_hough_line_transform.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated

local img = cv.imread(cv.samples.findFile('sudoku.png'))
local gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
local edges = cv.Canny(gray, 50, 150, kwargs({ apertureSize = 3 }))
local lines = cv.HoughLinesP(edges, 1, math.pi / 180, 100, kwargs({ minLineLength = 100, maxLineGap = 10 }))
for _, line in ipairs(lines:table()) do
    local x1, y1, x2, y2 = unpack(line)
    cv.line(img, { x1, y1 }, { x2, y2 }, { 0, 255, 0 }, 2)
end

cv.imshow('houghlines5.jpg', img)
cv.waitKey()
