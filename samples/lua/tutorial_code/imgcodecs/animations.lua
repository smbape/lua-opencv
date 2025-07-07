#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/imgcodecs/animations.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv

local INDEX_BASE = 1 -- lua is 1-based indexed
local OPENCV_DATA_PATH = opencv_lua.fs_utils.findFile("samples/data")

local function main()
    local parser = argparse() {}
    parser:argument("filename"):description("path to animation file"):default("animated_image.webp")
    local args = parser:parse()

    local filename = args.filename

    -- [write_animation]
    if filename == "animated_image.webp" then
        filename = OPENCV_DATA_PATH .. "/" .. filename

        -- Create an Animation instance to save
        local animation_to_save = cv.Animation()

        -- Generate a base image with a specific color
        local image = cv.Mat(128, 256, cv.CV_8UC4, { 150, 150, 150, 255 })
        local duration = 200
        local frames = {}
        local durations = {}

        -- Populate frames and durations in the Animation object
        for i = 0, 9 do
            local frame = image:copy()
            cv.putText(frame, "Frame " .. i, {30, 80}, cv.FONT_HERSHEY_SIMPLEX, 1.5, {255, 100, 0, 255}, 2)
            frames[#frames + 1] = frame
            durations[#durations + 1] = duration
        end

        animation_to_save.frames = frames
        animation_to_save.durations = durations

        -- Write the animation to file
        cv.imwriteanimation(filename, animation_to_save, {cv.IMWRITE_WEBP_QUALITY, 100})
        -- [write_animation]
    end

    -- [read_animation]
    local success, animation = cv.imreadanimation(filename)
    if not success then
        print("Failed to load animation frames")
        return
    end
    -- [read_animation]

    -- [show_animation]
    local i = 0
    local frames = animation.frames
    local durations = animation.durations
    while true do
        -- Without this, memory grows indefinitely
        collectgarbage()

        local frame = frames[i + INDEX_BASE]
        local duration = durations[i + INDEX_BASE]

        cv.imshow("Animation", frame)
        local key_code = cv.waitKey(duration)
        if key_code == 27 then  -- Exit if 'Esc' key is pressed
            return
        end

        i = i + 1
        if i == #frames then i = 0 end
    end
    -- [show_animation]
end

main()
