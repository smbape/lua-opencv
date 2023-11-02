package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/video/meanshift/meanshift.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global

-- parser = argparse.ArgumentParser(description='This sample demonstrates the meanshift algorithm. \
--                                               The example file can be downloaded from: \
--                                               https://www.bogotobogo.com/python/OpenCV_Python/images/mean_shift_tracking/slow_traffic_small.mp4')
-- parser.add_argument('image', type=str, help='path to image file')
-- args = parser.parse_args()

local args = {
    image = 'slow_traffic_small.mp4',
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

local cap = cv.VideoCapture(cv.samples.findFile(args.image))

-- take first frame of the video
local ret, frame = cap:read()
if not ret or frame:empty() then
    print('Could not open or find the image:', args.image)
    os.exit(0)
end

-- setup initial location of window
local x, y, w, h = 300, 200, 100, 50 -- simply hardcoded the values
local track_window = { x, y, w, h }

-- set up the ROI for tracking
local roi = frame:new(track_window)
local hsv_roi = cv.cvtColor(roi, cv.COLOR_BGR2HSV)
local mask = cv.inRange(hsv_roi, cv.Mat.createFromArray({ 0., 60., 32. }), cv.Mat.createFromArray({ 180., 255., 255. }))
local roi_hist = cv.calcHist({ hsv_roi }, { 0 }, mask, { 180 }, { 0, 180 })
cv.normalize(roi_hist, roi_hist, 0, 255, cv.NORM_MINMAX)

-- Setup the termination criteria, either 10 iteration or move by at least 1 pt
local term_crit = cv.TermCriteria(bit.bor(cv.TermCriteria.EPS, cv.TermCriteria.COUNT), 10, 1)

while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    ret, frame = cap:read()
    if not ret then break end

    local hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    local dst = cv.calcBackProject({ hsv }, { 0 }, roi_hist, { 0, 180 }, 1)

    -- apply meanshift to get the new location
    ret, track_window = cv.meanShift(dst, track_window, term_crit)

    -- Draw it on image
    local img2 = cv.rectangle(frame, track_window, 255, 2)
    cv.imshow('img2', img2)

    local k = bit.band(cv.waitKey(30), 0xff)
    if k == 27 then
        break
    end
end
