#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/ShapeDescriptors/bounding_rotated_ellipses/generalContours_demo2.py
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
    local threshold = val + thresh_offset -- avoid too low values because they give thousands of contours

    -- [Canny]
    -- Detect edges using Canny
    local canny_output = cv.Canny(src_gray, threshold, threshold * 2)
    -- [Canny]

    -- [findContours]
    -- Find contours
    local contours, _ = cv.findContours(canny_output, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
    -- [findContours]

    -- Find the rotated rectangles and ellipses for each contour
    local minRect = {}
    local minEllipse = {}
    for i, c in ipairs(contours) do
        minRect[i] = cv.minAreaRect(c)
        if #c > 5 then
            minEllipse[i] = cv.fitEllipse(c)
        end
    end

    -- Draw contours + rotated rects + ellipses
    -- [zeroMat]
    local drawing = cv.Mat.zeros(canny_output:size(), cv.CV_8UC3)
    -- [zeroMat]

    -- [forContour]
    for i, c in ipairs(contours) do
        local color = { rng:uniform(0, 256), rng:uniform(0, 256), rng:uniform(0, 256) }
        -- contour
        cv.drawContours(drawing, contours, i - INDEX_BASE, color)
        -- ellipse
        if #c > 5 then
            cv.ellipse(drawing, minEllipse[i], color, 2)
        end
        -- rotated rectangle
        local box = cv.boxPoints(minRect[i])
        box = box:convertTo(cv.CV_32S):reshape(2) -- np.intp: Integer used for indexing (same as C ssize_t; normally either int32 or int64)
        cv.drawContours(drawing, { box }, 0, color)
    end
    -- [forContour]

    -- [showDrawings]
    -- Show in a window
    cv.imshow('Contours', drawing)
    -- [showDrawings]
end

-- [setup]
-- Load source image
local parser = argparse() {description='Code for Creating Bounding rotated boxes and ellipses for contours tutorial.'}
parser:option('--input'):description('Path to input image.'):default('stuff.jpg')
local args = parser:parse()

local src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    print('Could not open or find the image:', args.input)
    os.exit(0)
end

-- Convert image to gray and blur it
src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
src_gray = cv.blur(src_gray, { 3, 3 })
-- [setup]

-- [createWindow]
-- Create Window
local source_window = 'Source'
cv.namedWindow(source_window)
cv.imshow(source_window, src)
-- [createWindow]
-- [trackbar]
local max_thresh = 255 - thresh_offset
local thresh = 100 - thresh_offset -- initial threshold
cv.createTrackbar('Canny Thresh:', source_window, thresh, max_thresh, thresh_callback)
thresh_callback(thresh)
-- [trackbar]

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
