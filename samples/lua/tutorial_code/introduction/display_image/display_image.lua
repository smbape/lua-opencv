#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/introduction/display_image/display_image.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

-- [imread]
local img = cv.imread(cv.samples.findFile("starry_night.jpg"))
-- [imread]
-- [empty]
if img:empty() then
    error("Could not read the image.")
end
-- [empty]
-- [imshow]
cv.imshow("Display window", img)
local k = cv.waitKey(0)
-- [imshow]
-- [imsave]
if k == string.byte("s") then
    cv.imwrite("starry_night.png", img)
end
-- [imsave]
