package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/video/background_subtraction/bg_sub.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

-- parser = argparse.ArgumentParser(description='This program shows how to use background subtraction methods provided by \
--                                               OpenCV. You can process both videos and images.')
-- parser.add_argument('--input', type=str, help='Path to a video or a sequence of image.', default='vtest.avi')
-- parser.add_argument('--algo', type=str, help='Background subtraction method (KNN, MOG2).', default='MOG2')
-- args = parser.parse_args()

local args = {
    input = 'vtest.avi',
    algo = 'MOG2',
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

-- [create]
local backSub
--create Background Subtractor objects
if args.algo == 'MOG2' then
    backSub = cv.createBackgroundSubtractorMOG2()
else
    backSub = cv.createBackgroundSubtractorKNN()
end
-- [create]

-- [capture]
local capture = cv.VideoCapture(cv.samples.findFileOrKeep(args.input))
if not capture:isOpened() then
    print('Unable to open: ' + args.input)
    os.exit(0)
end
-- [capture]

while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    local ret, frame = capture:read()
    if frame:empty() then
        break
    end

    -- [apply]
    --update the background model
    local fgMask = backSub:apply(frame)
    -- [apply]

    -- [display_frame_number]
    --get the frame number and write it on the current frame
    cv.rectangle(frame, { 10, 2 }, { 100, 20 }, { 255, 255, 255 }, -1)
    cv.putText(frame, tostring(capture:get(cv.CAP_PROP_POS_FRAMES)), { 15, 15 },
        cv.FONT_HERSHEY_SIMPLEX, 0.5, { 0, 0, 0 })
    -- [display_frame_number]

    -- [show]
    --show the current frame and the fg masks
    cv.imshow('Frame', frame)
    cv.imshow('FG Mask', fgMask)
    -- [show]

    local keyboard = cv.waitKey(30)
    if keyboard == string.byte('q') or keyboard == 27 then
        break
    end
end
