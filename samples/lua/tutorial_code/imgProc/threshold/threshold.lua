#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/threshold/threshold.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local max_value = 255
local max_type = 4
local max_binary_value = 255
local trackbar_type = 'Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted'
local trackbar_value = 'Value'
local window_name = 'Threshold Demo'
local src_gray

-- [Threshold_Demo]
local function Threshold_Demo(val)
    --0: Binary
    --1: Binary Inverted
    --2: Threshold Truncated
    --3: Threshold to Zero
    --4: Threshold to Zero Inverted
    local threshold_type = cv.getTrackbarPos(trackbar_type, window_name)
    local threshold_value = cv.getTrackbarPos(trackbar_value, window_name)
    local _, dst = cv.threshold(src_gray, threshold_value, max_binary_value, threshold_type)
    cv.imshow(window_name, dst)
end
-- [Threshold_Demo]

-- parser = argparse.ArgumentParser(description='Code for Basic Thresholding Operations tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='stuff.jpg')
-- args = parser.parse_args()

local args = {
    input = "stuff.jpg",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

-- [load]
-- Load an image
local src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    print('Could not open or find the image: ', args.input)
    os.exit(0)
end
-- Convert the image to Gray
src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
-- [load]

-- [window]
-- Create a window to display results
cv.namedWindow(window_name)
-- [window]

-- [trackbar]
-- Create Trackbar to choose type of Threshold
cv.createTrackbar(trackbar_type, window_name, 3, max_type, Threshold_Demo)
-- Create Trackbar to choose Threshold value
cv.createTrackbar(trackbar_value, window_name, 0, max_value, Threshold_Demo)
-- [trackbar]

-- Call the function to initialize
Threshold_Demo(0)

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly causes unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    -- Wait until user finishes program
    local key = cv.waitKey(1)
    if key ~= -1 then break end
end
