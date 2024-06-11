#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/highgui/trackbar/AddingImagesTrackbar.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv

local alpha_slider_max = 100
local title_window = 'Linear Blend'
local src1, src2

-- [on_trackbar]
local function on_trackbar(val)
    local alpha = val / alpha_slider_max
    local beta = (1.0 - alpha)
    local dst = cv.addWeighted(src1, alpha, src2, beta, 0.0)
    cv.imshow(title_window, dst)
end
-- [on_trackbar]

local parser = argparse() {description='Code for Adding a Trackbar to our applications tutorial.'}
parser:option('--input1'):description('Path to the first input image.'):default('LinuxLogo.jpg')
parser:option('--input2'):description('Path to the second input image.'):default('WindowsLogo.jpg')
local args = parser:parse()

-- [load]
-- Read images ( both have to be of the same size and type )
src1 = cv.imread(cv.samples.findFile(args.input1))
src2 = cv.imread(cv.samples.findFile(args.input2))
-- [load]
if src1:empty() then
    error('Could not open or find the image: ' .. args.input1)
end

if src2:empty() then
    error('Could not open or find the image: ' .. args.input2)
end

-- [window]
cv.namedWindow(title_window)
-- [window]

-- [create_trackbar]
local trackbar_name = string.format('Alpha x %d', alpha_slider_max)
cv.createTrackbar(trackbar_name, title_window, 0, alpha_slider_max, on_trackbar, 0)
-- [create_trackbar]

-- Show some stuff
on_trackbar(0)

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly causes unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    -- Wait until user press some key
    local key = cv.waitKey(1)
    if key ~= -1 then break end
end
