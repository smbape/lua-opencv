#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/features2D/Homography/perspective_correction.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local int_converter = function(val) return opencv_lua.math.int(tonumber(val)) end

-- Make the starting point unpredictable
local rng = cv.RNG(cv.getTickCount())

local function randomColor()
    return { rng:uniform_int(0, 256), rng:uniform_int(0, 256), rng:uniform_int(0, 256) }
end

local function perspectiveCorrection(img1Path, img2Path, patternSize)
    local img1 = cv.imread(cv.samples.findFile(img1Path))
    local img2 = cv.imread(cv.samples.findFile(img2Path))

    -- [find-corners]
    local ret1, corners1 = cv.findChessboardCorners(img1, patternSize)
    local ret2, corners2 = cv.findChessboardCorners(img2, patternSize)
    -- [find-corners]

    if not ret1 or not ret2 then
        error("Error, cannot find the chessboard corners in both images.")
    end

    -- [estimate-homography]
    local H, _ = cv.findHomography(corners1, corners2)
    print(H)
    -- [estimate-homography]

    -- [warp-chessboard]
    local img1_warp = cv.warpPerspective(img1, H, { img1.width, img1.height })
    -- [warp-chessboard]

    local img_draw_warp = cv.hconcat({ img2, img1_warp })
    cv.imshow("Desired chessboard view / Warped source chessboard view", img_draw_warp)

    -- [compute-transformed-corners]
    local img_draw_matches = cv.hconcat({ img1, img2 })
    for i = 0, #corners1 - 1 do
        local pt1 = cv.Mat.createFromArray({ corners1[i][0], corners1[i][1], 1 })
        pt1 = pt1:reshape(1, 3)
        local pt2 = cv.dot(H, pt1)
        pt2 = pt2 / pt2[2]
        local start = { int(corners1[i][0]), int(corners1[i][1]) }
        local end_ = { int(img1.width + pt2[0]), int(pt2[1]) }
        cv.line(img_draw_matches, start, end_, randomColor(), 2)
    end

    cv.imshow("Draw matches", img_draw_matches)
    cv.waitKey(0)
    -- [compute-transformed-corners]
end

local function main()
    local argparse = require("argparse")
    local parser = argparse() {}
    parser:option('-I1 --image1'):description("Path to the first image"):default("left02.jpg")
    parser:option('-I2 --image2'):description("Path to the second image"):default("left01.jpg")
    parser:option('-H --height'):description("Height of pattern size"):default(6):convert(int_converter)
    parser:option('-W --width'):description("Width of pattern size"):default(9):convert(int_converter)
    local args = parser:parse()

    local img1Path = args.image1
    local img2Path = args.image2
    local h = args.height
    local w = args.width
    perspectiveCorrection(img1Path, img2Path, { w, h })
end

main()
