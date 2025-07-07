#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/core/AddingImages/adding_images.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local alpha = 0.5

print([[ Simple Linear Blender
-----------------------
* Enter alpha [0.0-1.0]: ]])

local input_alpha = io.read("*n") -- read a number
if 0 <= input_alpha and input_alpha <= 1 then
    alpha = input_alpha
end
-- [load]
local src1 = cv.imread(cv.samples.findFile('LinuxLogo.jpg'))
local src2 = cv.imread(cv.samples.findFile('WindowsLogo.jpg'))
-- [load]
if src1:empty() then
    error("Error loading src1")
elseif src2:empty() then
    error("Error loading src2")
end
-- [blend_images]
local beta = (1.0 - alpha)
local dst = cv.addWeighted(src1, alpha, src2, beta, 0.0)
-- [blend_images]
-- [display]
cv.imshow('dst', dst)
cv.waitKey(0)
-- [display]
cv.destroyAllWindows()
