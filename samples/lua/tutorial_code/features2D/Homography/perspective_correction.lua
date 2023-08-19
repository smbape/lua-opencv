package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1,1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/features2D/Homography/perspective_correction.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local round = opencv_lua.math.round

-- Make the starting point unpredictable
cv.theRNG().state = cv.getTickCount()

local function randomColor()
    local color = cv.Mat.zeros(3, 1, cv.CV_8UC1)
    cv.randu(color, 0.0, 255.0)
    return color:tolist()
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
        local pt2 = H * pt1
        pt2 = pt2 / pt2[2]
        local start = { round(corners1[i][0]), round(corners1[i][1]) }
        local end_ = { round(img1.width + pt2[0]), round(pt2[1]) }
        cv.line(img_draw_matches, start, end_, randomColor(), 2)
    end

    cv.imshow("Draw matches", img_draw_matches)
    cv.waitKey(0)
    -- [compute-transformed-corners]
end

local function main()
    -- import argparse
    -- parser = argparse.ArgumentParser()
    -- parser.add_argument('-I1', "--image1", help="Path to the first image", default="left02.jpg")
    -- parser.add_argument('-I2', "--image2", help="Path to the second image", default="left01.jpg")
    -- parser.add_argument('-H', "--height", help="Height of pattern size", default=6)
    -- parser.add_argument('-W', "--width", help="Width of pattern size", default=9)
    -- args = parser.parse_args()

    local args = {
        image1 = "left02.jpg",
        image2 = "left01.jpg",
        height = 6,
        width = 9,
    }

    local aliases = {}
    aliases["-I1"] = "image1"
    aliases["-I2"] = "image2"
    aliases["-H"] = "height"
    aliases["-W"] = "width"

    for i=1, #arg, 2 do
        local name = arg[i]
        if name:sub(1,2) == "--" then name = name:sub(3) end
        if aliases[name] ~= nil then name = aliases[name] end
        if args[name] == nil or i == #arg then
            error('unexpected argument ' .. name)
        end
        args[name] = arg[i + 1]
    end

    local img1Path = args.image1
    local img2Path = args.image2
    local h = args.height
    local w = args.width
    perspectiveCorrection(img1Path, img2Path, { w, h })
end

main()
