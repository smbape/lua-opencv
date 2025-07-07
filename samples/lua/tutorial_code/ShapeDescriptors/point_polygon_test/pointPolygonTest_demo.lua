#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/ShapeDescriptors/point_polygon_test/pointPolygonTest_demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local INDEX_BASE = 1 -- lua is 1-based indexed

-- Create an image
local r = 100
local src = cv.Mat.zeros(4 * r, 4 * r, cv.CV_8U)

-- Create a sequence of points to make a contour
local vert = {
    { int(3 * r / 2), int(1.34 * r) },
    { 1 * r,          2 * r },
    { int(3 * r / 2), int(2.866 * r) },
    { int(5 * r / 2), int(2.866 * r) },
    { 3 * r,          2 * r },
    { int(5 * r / 2), int(1.34 * r) },
}

-- Draw it in src
for i = 1, #vert do
    cv.line(src, vert[i], vert[(i) % 6 + 1], { 255 }, 3)
end

-- Get the contours
local contours, _ = cv.findContours(src, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

-- Calculate the distances to the contour
local raw_dist = cv.Mat(src:size(), cv.CV_32F)
for i = 0, src.rows - 1 do
    for j = 0, src.cols - 1 do
        raw_dist[{ i, j }] = cv.pointPolygonTest(contours[0 + INDEX_BASE], { j, i }, true)
    end
end

local minVal, maxVal, _, maxDistPt = cv.minMaxLoc(raw_dist)
minVal = math.abs(minVal)
maxVal = math.abs(maxVal)

-- Depicting the  distances graphically
local drawing = cv.Mat.zeros(src:size(), cv.CV_8UC3)
for i = 0, src.rows - 1 do
    for j = 0, src.cols - 1 do
        if raw_dist[{ i, j }] < 0 then
            drawing[{ i, j, 0 }] = 255 - math.abs(raw_dist[{ i, j }]) * 255 / minVal
        elseif raw_dist[{ i, j }] > 0 then
            drawing[{ i, j, 2 }] = 255 - raw_dist[{ i, j }] * 255 / maxVal
        else
            drawing[{ i, j, 0 }] = 255
            drawing[{ i, j, 1 }] = 255
            drawing[{ i, j, 2 }] = 255
        end
    end
end

cv.circle(drawing, maxDistPt, int(maxVal), { 255, 255, 255 }, 1, cv.LINE_8, 0)
cv.imshow('Source', src)
cv.imshow('Distance and inscribed circle', drawing)
cv.waitKey()
