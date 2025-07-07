#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/TrackingMotion/harris_detector/cornerHarris_Demo.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local int = opencv_lua.math.int

local source_window = 'Source image'
local corners_window = 'Corners detected'
local max_thresh = 255

local src_gray

local function cornerHarris_demo(val)
    local thresh = val

    -- Detector parameters
    local blockSize = 2
    local apertureSize = 3
    local k = 0.04

    -- Detecting corners
    local dst = cv.cornerHarris(src_gray, blockSize, apertureSize, k)

    -- Normalizing
    local dst_norm = cv.Mat(dst:size(), cv.CV_32F)
    cv.normalize(dst, dst_norm, kwargs({ alpha = 0, beta = 255, norm_type = cv.NORM_MINMAX }))
    local dst_norm_scaled = cv.convertScaleAbs(dst_norm)

    -- transform into an lua table for faster processing in lua
    local rows, cols = dst_norm.rows, dst_norm.cols
    dst_norm = dst_norm:table()

    -- Drawing a circle around corners
    for i = 1, rows do
        for j = 1, cols do
            if int(dst_norm[i][j]) > thresh then
                cv.circle(dst_norm_scaled, { j - 1, i - 1 }, 5, { 0 }, 2)
            end
        end
    end

    -- Showing the result
    cv.namedWindow(corners_window)
    cv.imshow(corners_window, dst_norm_scaled)
end

-- Load source image and convert it to gray
local parser = argparse() {description='Code for Harris corner detector tutorial.'}
parser:option('--input'):description('Path to input image.'):default('building.jpg')
local args = parser:parse()

local src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    print('Could not open or find the image:', args.input)
    os.exit(0)
end

src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)

-- Create a window and a trackbar
cv.namedWindow(source_window)
local thresh = 200 -- initial threshold
cv.createTrackbar('Threshold: ', source_window, thresh, max_thresh, cornerHarris_demo)
cv.imshow(source_window, src)
cornerHarris_demo(thresh)

while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly cause unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    -- Wait until user press some key
    local key = cv.waitKey(1)
    if key ~= -1 then break end
end
