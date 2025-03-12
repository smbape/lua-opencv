#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/Histograms_Matching/back_projection/calcBackProject_Demo2.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global

local low = 20
local up = 20

local hsv, src

local function callback_low(val)
    low = val
end

local function callback_up(val)
    up = val
end

local function concat(a, b)
    local c = {}
    for k, v in pairs(a) do c[k] = v end
    for k, v in pairs(b) do c[k + #a] = v end
    return c
end

local function Hist_and_Backproj(mask)
    local h_bins = 30
    local s_bins = 32
    local histSize = { h_bins, s_bins }
    local h_range = { 0, 180 }
    local s_range = { 0, 256 }
    local ranges = concat(h_range, s_range) -- Concat list
    local channels = { 0, 1 }

    -- Get the Histogram and normalize it
    local hist = cv.calcHist({ hsv }, channels, mask, histSize, ranges, kwargs({ accumulate = false }))

    cv.normalize(hist, hist, kwargs({ alpha = 0, beta = 255, norm_type = cv.NORM_MINMAX }))

    -- Get Backprojection
    local backproj = cv.calcBackProject({ hsv }, channels, hist, ranges, kwargs({ scale = 1 }))

    -- Draw the backproj
    cv.imshow('BackProj', backproj)
end

local function pickPoint(event, x, y, flags, param)
    if event ~= cv.EVENT_LBUTTONDOWN then
        return
    end

    -- Fill and get the mask
    local seed = { x, y }
    local newMaskVal = 255
    local newVal = { 120, 120, 120 }
    local connectivity = 8
    local flags = connectivity + bit.lshift(newMaskVal, 8) + cv.FLOODFILL_FIXED_RANGE + cv.FLOODFILL_MASK_ONLY

    local mask2 = cv.Mat.zeros(src.rows + 2, src.cols + 2, cv.CV_8U)
    print('low:', low, 'up:', up)
    cv.floodFill(src, mask2, seed, newVal, { low, low, low }, { up, up, up }, flags)
    local mask = mask2:new({ 1, mask2.rows - 1 }, { 1, mask2.cols - 1 })

    cv.imshow('Mask', mask)
    Hist_and_Backproj(mask)
end

-- Read the image
local parser = argparse() {description='Code for Back Projection tutorial.'}
parser:option('--input'):description('Path to input image.'):default('home.jpg')
local args = parser:parse()

src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    error('Could not open or find the image: ' .. args.input)
end

-- Transform it to HSV
hsv = cv.cvtColor(src, cv.COLOR_BGR2HSV)

-- Show the image
local window_image = 'Source image'
cv.namedWindow(window_image)
cv.imshow(window_image, src)

-- Set Trackbars for floodfill thresholds
cv.createTrackbar('Low thresh', window_image, low, 255, callback_low)
cv.createTrackbar('High thresh', window_image, up, 255, callback_up)
-- Set a Mouse Callback
cv.setMouseCallback(window_image, pickPoint)

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
