#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/imgProc/threshold_inRange/threshold_inRange.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = function(val) return opencv_lua.math.int(tonumber(val)) end

local max_value = 255
local max_value_H = math.floor(360 / 2)
local low_H = 0
local low_S = 0
local low_V = 0
local high_H = max_value_H
local high_S = max_value
local high_V = max_value
local window_capture_name = 'Video Capture'
local window_detection_name = 'Object Detection'
local low_H_name = 'Low H'
local low_S_name = 'Low S'
local low_V_name = 'Low V'
local high_H_name = 'High H'
local high_S_name = 'High S'
local high_V_name = 'High V'

-- [low]
local function on_low_H_thresh_trackbar(val)
    low_H = val
    low_H = math.min(high_H - 1, low_H)
    cv.setTrackbarPos(low_H_name, window_detection_name, low_H)
end
-- [low]

-- [high]
local function on_high_H_thresh_trackbar(val)
    high_H = val
    high_H = math.max(high_H, low_H + 1)
    cv.setTrackbarPos(high_H_name, window_detection_name, high_H)
end
-- [high]

local function on_low_S_thresh_trackbar(val)
    low_S = val
    low_S = math.min(high_S - 1, low_S)
    cv.setTrackbarPos(low_S_name, window_detection_name, low_S)
end

local function on_high_S_thresh_trackbar(val)
    high_S = val
    high_S = math.max(high_S, low_S + 1)
    cv.setTrackbarPos(high_S_name, window_detection_name, high_S)
end

local function on_low_V_thresh_trackbar(val)
    low_V = val
    low_V = math.min(high_V - 1, low_V)
    cv.setTrackbarPos(low_V_name, window_detection_name, low_V)
end

local function on_high_V_thresh_trackbar(val)
    high_V = val
    high_V = math.max(high_V, low_V + 1)
    cv.setTrackbarPos(high_V_name, window_detection_name, high_V)
end

local parser = argparse() {description='Code for Thresholding Operations using inRange tutorial.'}
parser:option('--camera'):description('Camera divide number.'):default(0):convert(int)
local args = parser:parse()

-- [cap]
local cap = cv.VideoCapture(args.camera)
-- [cap]

-- [window]
cv.namedWindow(window_capture_name)
cv.namedWindow(window_detection_name)
-- [window]

-- [trackbar]
cv.createTrackbar(low_H_name, window_detection_name, low_H, max_value_H, on_low_H_thresh_trackbar)
cv.createTrackbar(high_H_name, window_detection_name, high_H, max_value_H, on_high_H_thresh_trackbar)
cv.createTrackbar(low_S_name, window_detection_name, low_S, max_value, on_low_S_thresh_trackbar)
cv.createTrackbar(high_S_name, window_detection_name, high_S, max_value, on_high_S_thresh_trackbar)
cv.createTrackbar(low_V_name, window_detection_name, low_V, max_value, on_low_V_thresh_trackbar)
cv.createTrackbar(high_V_name, window_detection_name, high_V, max_value, on_high_V_thresh_trackbar)
-- [trackbar]

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly causes unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    -- [while]
    local ret, frame = cap:read()
    if frame:empty() then
        break
    end

    local frame_HSV = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    local frame_threshold = cv.inRange(frame_HSV, { low_H, low_S, low_V }, { high_H, high_S, high_V })
    -- [while]

    -- [show]
    cv.imshow(window_capture_name, frame)
    cv.imshow(window_detection_name, frame_threshold)
    -- [show]

    local key = cv.waitKey(30)
    if key == 0x1b or key == string.byte("q") or key == string.byte("Q") then
        break
    end
end
