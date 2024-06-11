#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/objectDetection/cascade_classifier/objectDetection.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local round = opencv_lua.math.round
local dirname = opencv_lua.path.dirname
local findFile = opencv_lua.fs_utils.findFile
local kwargs = opencv_lua.kwargs
local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated
local convert_to_int = function(val) return opencv_lua.math.int(tonumber(val)) end

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
    for i, face in ipairs(faces) do
        local x, y, w, h = unpack(face)
        local center = { x + int(w / 2), y + int(h / 2) }
        frame = cv.ellipse(frame, center, { int(w / 2), int(h / 2) }, 0, 0, 360, { 255, 0, 255 }, 4)

        local faceROI = frame_gray:new(face)
        ---- In each face, detect eyes
        local eyes = eyes_cascade:detectMultiScale(faceROI)
        for i, eye in ipairs(eyes) do
            local x2, y2, w2, h2 = unpack(eye)
            local eye_center = { x + x2 + int(w2 / 2), y + y2 + int(h2 / 2) }
            local radius = int(round((w2 + h2) * 0.25))
            frame = cv.circle(frame, eye_center, radius, { 255, 0, 0 }, 4)
        end
    end

    cv.imshow('Capture - Face detection', frame)
end

local parser = argparse() {description='Code for Cascade Classifier tutorial.'}
parser:option('--face_cascade'):description('Path to face cascade.'):default('data/haarcascades/haarcascade_frontalface_alt.xml')
parser:option('--eyes_cascade'):description('Path to eyes cascade.'):default('data/haarcascades/haarcascade_eye_tree_eyeglasses.xml')
parser:option('--camera'):description('Camera divide number.'):convert(convert_to_int):default(0)
local args = parser:parse()

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
