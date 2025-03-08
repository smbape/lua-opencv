#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/video/optical_flow/optical_flow_dense.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local INDEX_BASE = 1 -- lua is 1-based indexed

local cap = cv.VideoCapture(cv.samples.findFile("vtest.avi"))
local ret, frame1 = cap:read()
if not ret or frame1:empty() then
    print('Could not open or find the image:', "vtest.avi")
    os.exit(0)
end


local prvs = cv.cvtColor(frame1, cv.COLOR_BGR2GRAY)
local hsv = cv.Mat.zeros(frame1:size(), frame1:type())
local hsv_parts = opencv_lua.VectorOfMat()
cv.split(hsv, hsv_parts)
hsv_parts[1 + INDEX_BASE]:setTo(255)

local flow_parts = opencv_lua.VectorOfMat()

while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    local ret, frame2 = cap:read()
    if not ret then
        print('No frames grabbed!')
        break
    end

    local next = cv.cvtColor(frame2, cv.COLOR_BGR2GRAY)
    local flow = cv.calcOpticalFlowFarneback(prvs, next, nil, 0.5, 3, 15, 3, 5, 1.2, 0)

    cv.split(flow, flow_parts)
    local mag, ang = cv.cartToPolar(flow_parts[1], flow_parts[2])

    ang = ang * 180 / math.pi / 2
    mag = cv.normalize(mag, nil, 0, 255, cv.NORM_MINMAX)
    ang:convertTo(hsv_parts[0 + INDEX_BASE]:type()):copyTo(hsv_parts[0 + INDEX_BASE])
    mag:convertTo(hsv_parts[2 + INDEX_BASE]:type()):copyTo(hsv_parts[2 + INDEX_BASE])

    hsv = cv.merge(hsv_parts)

    local bgr = cv.cvtColor(hsv, cv.COLOR_HSV2BGR)
    cv.imshow('opticalfb.png', frame2)
    cv.imshow('opticalhsv.png', bgr)
    local k = cv.waitKey(30)
    if k == 27 then
        break
    end
    prvs = next
end

cv.destroyAllWindows()
