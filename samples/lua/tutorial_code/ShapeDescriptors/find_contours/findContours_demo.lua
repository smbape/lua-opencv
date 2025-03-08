#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/ShapeDescriptors/find_contours/findContours_demo.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local INDEX_BASE = 1 -- lua is 1-based indexed

-- Make the starting point predictable
local rng = cv.RNG(12345)

local src_gray
local thresh_offset = 10

local function thresh_callback(val)
    local threshold = val + thresh_offset

    -- Detect edges using Canny
    local canny_output = cv.Canny(src_gray, threshold, threshold * 2)

    -- Find contours
    local contours, hierarchy = cv.findContours(canny_output, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

    -- Draw contours
    local drawing = cv.Mat.zeros(canny_output:size(), cv.CV_8UC3)
    for i = 1, #contours do
        local color = { rng:uniform(0, 256), rng:uniform(0, 256), rng:uniform(0, 256) }
        cv.drawContours(drawing, contours, i - INDEX_BASE, color, 2, cv.LINE_8, hierarchy, 0)
    end

    -- Show in a window
    cv.imshow('Contours', drawing)
end

-- Load source image
local parser = argparse() {description='Code for Finding contours in your image tutorial.'}
parser:option('--input'):description('Path to input image.'):default('HappyFish.jpg')
local args = parser:parse()

local src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    print('Could not open or find the image:', args.input)
    os.exit(0)
end

-- Convert image to gray and blur it
src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
src_gray = cv.blur(src_gray, { 3, 3 })

-- Create Window
local source_window = 'Source'
cv.namedWindow(source_window)
cv.imshow(source_window, src)
local max_thresh = 255 - thresh_offset
local thresh = 100 - thresh_offset -- initial threshold
cv.createTrackbar('Canny Thresh:', source_window, thresh, max_thresh, thresh_callback)
thresh_callback(thresh)

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly cause unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    -- Wait until user press some key
    local key = cv.waitKey(1)
    if key ~= -1 then break end
end
