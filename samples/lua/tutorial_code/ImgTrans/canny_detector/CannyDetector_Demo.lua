#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/ImgTrans/canny_detector/CannyDetector_Demo.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

local max_lowThreshold = 100
local window_name = 'Edge Map'
local title_trackbar = 'Min Threshold:'
local ratio = 3
local kernel_size = 3
local src, src_gray, dst

local function CannyThreshold(val)
    local low_threshold = val
    local img_blur = cv.blur(src_gray, { 3, 3 })
    local detected_edges = cv.Canny(img_blur, low_threshold, low_threshold * ratio,
        kwargs({ apertureSize = kernel_size }))
    dst:setTo(0)
    src:copyTo(detected_edges, dst)
    cv.imshow(window_name, dst)
end

local parser = argparse() {description='Code for Canny Edge Detector tutorial.'}
parser:option('--input'):description('Path to input image.'):default('fruits.jpg')
local args = parser:parse()

src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    error('Could not open or find the image: ' .. args.input)
end

src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
dst = cv.Mat(src:size(), src:type())

cv.namedWindow(window_name)
cv.createTrackbar(title_trackbar, window_name, 0, max_lowThreshold, CannyThreshold)

CannyThreshold(0)

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly causes unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    local key = cv.waitKey(30)
    if key == 0x1b or key == string.byte("q") or key == string.byte("Q") then
        break
    end
end
