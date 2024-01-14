#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/HitMiss/hit_miss.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

local input_image = cv.Mat.createFromArray({
    { 0, 0,   0,   0,   0, 0,   0,   0 },
    { 0, 255, 255, 255, 0, 0,   0,   255 },
    { 0, 255, 255, 255, 0, 0,   0,   0 },
    { 0, 255, 255, 255, 0, 255, 0,   0 },
    { 0, 0,   255, 0,   0, 0,   0,   0 },
    { 0, 0,   255, 0,   0, 255, 255, 0 },
    { 0, 255, 0,   255, 0, 0,   255, 0 },
    { 0, 255, 255, 255, 0, 0,   0,   0 } }, cv.CV_8U)

local kernel = cv.Mat.createFromArray({
    { 0, 1,  0 },
    { 1, -1, 1 },
    { 0, 1,  0 } }, cv.CV_32S)

local output_image = cv.morphologyEx(input_image, cv.MORPH_HITMISS, kernel)

local rate = 50
local kernel = (kernel + 1) * 127
kernel = kernel:convertTo(cv.CV_8U)

local kernel = cv.resize(kernel, nil, kwargs({ fx = rate, fy = rate, interpolation = cv.INTER_NEAREST }))
cv.imshow("kernel", kernel)
cv.moveWindow("kernel", 0, 0)

input_image = cv.resize(input_image, nil, kwargs({ fx = rate, fy = rate, interpolation = cv.INTER_NEAREST }))
cv.imshow("Original", input_image)
cv.moveWindow("Original", 0, 200)

output_image = cv.resize(output_image, nil, kwargs({ fx = rate, fy = rate, interpolation = cv.INTER_NEAREST }))
cv.imshow("Hit or Miss", output_image)
cv.moveWindow("Hit or Miss", 500, 200)

cv.waitKey(0)
cv.destroyAllWindows()
