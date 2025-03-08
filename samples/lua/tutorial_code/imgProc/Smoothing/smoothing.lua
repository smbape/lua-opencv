#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/imgProc/Smoothing/smoothing.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int

--  Global Variables

local DELAY_CAPTION = 1500
local DELAY_BLUR = 100
local MAX_KERNEL_LENGTH = 31

local src = nil
local dst = nil
local window_name = 'Smoothing Demo'


local function display_dst(delay)
    cv.imshow(window_name, dst)
    local c = cv.waitKey(delay)
    if c >= 0 then return -1 end
    return 0
end


local function display_caption(caption)
    dst = cv.Mat.zeros(src:size(), src:type())
    local rows, cols = src.rows, src.cols
    cv.putText(dst, caption,
        { int(cols / 4), int(rows / 2) },
        cv.FONT_HERSHEY_COMPLEX, 1, { 255, 255, 255 })

    return display_dst(DELAY_CAPTION)
end


local function main(args)
    cv.namedWindow(window_name, cv.WINDOW_AUTOSIZE)

    -- Load the source image
    local imageName = args.input

    src = cv.imread(cv.samples.findFile(imageName))
    if src:empty() then
        print('Error opening image')
        print('Usage: smoothing.lua [image_name -- default ../data/lena.jpg] \n')
        return -1
    end

    if display_caption('Original Image') ~= 0 then
        return 0
    end

    dst = src:copy()
    if display_dst(DELAY_CAPTION) ~= 0 then
        return 0
    end

    -- Applying Homogeneous blur
    if display_caption('Homogeneous Blur') ~= 0 then
        return 0
    end

    -- [blur]
    for i = 1, MAX_KERNEL_LENGTH - 1, 2 do
        dst = cv.blur(src, { i, i })
        if display_dst(DELAY_BLUR) ~= 0 then
            return 0
        end
    end
    -- [blur]

    -- Applying Gaussian blur
    if display_caption('Gaussian Blur') ~= 0 then
        return 0
    end

    -- [gaussianblur]
    for i = 1, MAX_KERNEL_LENGTH - 1, 2 do
        dst = cv.GaussianBlur(src, { i, i }, 0)
        if display_dst(DELAY_BLUR) ~= 0 then
            return 0
        end
    end
    -- [gaussianblur]

    -- Applying Median blur
    if display_caption('Median Blur') ~= 0 then
        return 0
    end

    -- [medianblur]
    for i = 1, MAX_KERNEL_LENGTH - 1, 2 do
        dst = cv.medianBlur(src, i)
        if display_dst(DELAY_BLUR) ~= 0 then
            return 0
        end
    end
    -- [medianblur]

    -- Applying Bilateral Filter
    if display_caption('Bilateral Blur') ~= 0 then
        return 0
    end

    -- [bilateralfilter]
    -- Remember, bilateral is a bit slow, so as value go higher, it takes long time
    for i = 1, MAX_KERNEL_LENGTH - 1, 2 do
        dst = cv.bilateralFilter(src, i, i * 2, i / 2)
        if display_dst(DELAY_BLUR) ~= 0 then
            return 0
        end
    end
    -- [bilateralfilter]

    --  Done
    display_caption('Done!')

    return 0
end


local parser = argparse() {}
parser:argument('input'):description('Path to input image.'):default('lena.jpg')
local args = parser:parse()

main(args)
