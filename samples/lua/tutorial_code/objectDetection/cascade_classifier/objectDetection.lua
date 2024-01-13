#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/objectDetection/cascade_classifier/objectDetection.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local round = opencv_lua.math.round
local dirname = opencv_lua.path.dirname
local findFile = opencv_lua.fs_utils.findFile
local kwargs = opencv_lua.kwargs

cv.samples.addSamplesDataSearchPath(dirname(dirname(dirname(findFile(
    "data/haarcascades/haarcascade_frontalface_alt.xml", kwargs({
        hints = {
            "out/build/x64-Debug/opencv/opencv-src",
            "out/build/x64-Release/opencv/opencv-src",
            "out/build/Linux-GCC-Debug/opencv/opencv-src",
            "out/build/Linux-GCC-Release/opencv/opencv-src",
            "out/prepublish/luajit-2.1/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.4/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.3/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.2/lua-opencv/build.luarocks/opencv/opencv-src",
            "out/prepublish/5.1/lua-opencv/build.luarocks/opencv/opencv-src",
            "opencv",
        }
    }))))))

local face_cascade, eyes_cascade

local function detectAndDisplay(frame)
    local frame_gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    frame_gray = cv.equalizeHist(frame_gray)

    ---- Detect faces
    local faces = face_cascade:detectMultiScale(frame_gray)
    for i, face in faces:pairs() do
        local x, y, w, h = face.x, face.y, face.width, face.height
        local center = { x + int(w / 2), y + int(h / 2) }
        frame = cv.ellipse(frame, center, { int(w / 2), int(h / 2) }, 0, 0, 360, { 255, 0, 255 }, 4)

        local faceROI = frame_gray:new(face)
        ---- In each face, detect eyes
        local eyes = eyes_cascade:detectMultiScale(faceROI)
        for i, eye in eyes:pairs() do
            local x2, y2, w2, h2 = eye.x, eye.y, eye.width, eye.height
            local eye_center = { x + x2 + int(w2 / 2), y + y2 + int(h2 / 2) }
            local radius = int(round((w2 + h2) * 0.25))
            frame = cv.circle(frame, eye_center, radius, { 255, 0, 0 }, 4)
        end
    end

    cv.imshow('Capture - Face detection', frame)
end

-- parser = argparse.ArgumentParser(description='Code for Cascade Classifier tutorial.')
-- parser.add_argument('--face_cascade', help='Path to face cascade.', default='data/haarcascades/haarcascade_frontalface_alt.xml')
-- parser.add_argument('--eyes_cascade', help='Path to eyes cascade.', default='data/haarcascades/haarcascade_eye_tree_eyeglasses.xml')
-- parser.add_argument('--camera', help='Camera divide number.', type=int, default=0)
-- args = parser.parse_args()

local args = {
    face_cascade = 'data/haarcascades/haarcascade_frontalface_alt.xml',
    eyes_cascade = 'data/haarcascades/haarcascade_eye_tree_eyeglasses.xml',
    camera = 0,
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    if type(args[name]) == 'number' then
        args[name] = tonumber(arg[i + 1])
    else
        args[name] = arg[i + 1]
    end
end

local face_cascade_name = args.face_cascade
local eyes_cascade_name = args.eyes_cascade

face_cascade = cv.CascadeClassifier()
eyes_cascade = cv.CascadeClassifier()

---- 1. Load the cascades
if not face_cascade:load(cv.samples.findFile(face_cascade_name)) then
    print('--(!)Error loading face cascade')
    os.exit(0)
end
if not eyes_cascade:load(cv.samples.findFile(eyes_cascade_name)) then
    print('--(!)Error loading eyes cascade')
    os.exit(0)
end

local camera_device = args.camera
---- 2. Read the video stream
local cap = cv.VideoCapture(camera_device)
if not cap.isOpened then
    print('--(!)Error opening video capture')
    os.exit(0)
end

while true do
    local ret, frame = cap:read()
    if frame:empty() then
        print('--(!) No captured frame -- Break!')
        break
    end

    detectAndDisplay(frame)

    if cv.waitKey(10) == 27 then
        break
    end
end
