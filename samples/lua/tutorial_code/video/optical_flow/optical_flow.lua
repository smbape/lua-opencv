#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/video/optical_flow/optical_flow.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global
local int = opencv_lua.math.int
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated
local INDEX_BASE = 1 -- lua is 1-based indexed

local parser = argparse() {description='This sample demonstrates Lucas-Kanade Optical Flow calculation. \
                                              The example file can be downloaded from: \
                                              https://www.bogotobogo.com/python/OpenCV_Python/images/mean_shift_tracking/slow_traffic_small.mp4'}
parser:argument('image'):description('path to image file'):default('slow_traffic_small.mp4')
local args = parser:parse()

local cap = cv.VideoCapture(cv.samples.findFile(args.image))

-- params for ShiTomasi corner detection
local feature_params = {
    maxCorners = 100,
    qualityLevel = 0.3,
    minDistance = 7,
    blockSize = 7,
    mask = nil
}

-- Parameters for lucas kanade optical flow
local lk_params = {
    winSize  = { 15, 15 },
    maxLevel = 2,
    criteria = cv.TermCriteria(bit.bor(cv.TermCriteria.EPS, cv.TermCriteria.COUNT), 10, 0.03)
}

-- Create some random colors
local colors = {}
local rng = cv.RNG()
for i = 1, 100 do
    colors[i] = { rng:uniform(0, 256), rng:uniform(0, 256), rng:uniform(0, 256) }
end

-- Take first frame and find corners in it
local ret, old_frame = cap:read()
if not ret or old_frame:empty() then
    print('Could not open or find the image:', args.image)
    os.exit(0)
end

local p0 = opencv_lua.VectorOfPoint2f()
local p1 = opencv_lua.VectorOfPoint2f()

local old_gray = cv.cvtColor(old_frame, cv.COLOR_BGR2GRAY)

feature_params.corners = p0
cv.goodFeaturesToTrack(old_gray, kwargs(feature_params))

-- Create a mask image for drawing purposes
local mask = cv.Mat.zeros(old_frame:size(), old_frame:type())

while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    local ret, frame = cap:read()
    if not ret then
        print('No frames grabbed!')
        break
    end

    local frame_gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)

    -- calculate optical flow
    local _, st, err = cv.calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, kwargs(lk_params))
    local good_new = opencv_lua.VectorOfPoint2f()

    for i = 1, #p0 do
        -- Select good points
        if st[i - INDEX_BASE] == 1 then
            good_new[#good_new + 1] = p1[i]

            local a, b = unpack(p1[i])
            local c, d = unpack(p0[i])

            -- draw the tracks
            cv.line(mask, { int(a), int(b) }, { int(c), int(d) }, colors[i], 2)
            cv.circle(frame, { int(a), int(b) }, 5, colors[i], -1)
        end
    end

    local img = cv.add(frame, mask)

    cv.imshow('frame', img)
    local k = bit.band(cv.waitKey(30), 0xff)
    if k == 27 then
        break
    end

    -- Now update the previous frame and previous points
    old_gray = frame_gray:copy()
    p0 = good_new
end

cv.destroyAllWindows()
