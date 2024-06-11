#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/TrackingMotion/generic_corner_detector/cornerDetector_Demo.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local INDEX_BASE = 1 -- lua is 1-based indexed

if not math.pow then
    math.pow = function(x, y) return x ^ y end
end

local myHarris_window = 'My Harris corner detector'
local myShiTomasi_window = 'My Shi Tomasi corner detector'
local myHarris_qualityLevel = 50
local myShiTomasi_qualityLevel = 50
local max_qualityLevel = 100

local rng = cv.RNG(12345)

local src
local Mc, myHarris_minVal, myHarris_maxVal
local myShiTomasi_dst, myShiTomasi_minVal, myShiTomasi_maxVal

local function myHarris_function(val)
    local myHarris_copy = src:copy()
    local myHarris_qualityLevel = math.max(val, 1)

    for i = 0, src.rows - 1 do
        for j = 0, src.cols - 1 do
            if Mc[i + INDEX_BASE][j + INDEX_BASE] > myHarris_minVal + (myHarris_maxVal - myHarris_minVal) * myHarris_qualityLevel / max_qualityLevel then
                cv.circle(myHarris_copy, { j, i }, 4, { rng:uniform(0, 256), rng:uniform(0, 256), rng:uniform(0, 256) },
                    cv.FILLED)
            end
        end
    end

    cv.imshow(myHarris_window, myHarris_copy)
end

local function myShiTomasi_function(val)
    local myShiTomasi_copy = src:copy()
    local myShiTomasi_qualityLevel = math.max(val, 1)

    for i = 0, src.rows - 1 do
        for j = 0, src.cols - 1 do
            if myShiTomasi_dst[i + INDEX_BASE][j + INDEX_BASE] > myShiTomasi_minVal + (myShiTomasi_maxVal - myShiTomasi_minVal) * myShiTomasi_qualityLevel / max_qualityLevel then
                cv.circle(myShiTomasi_copy, { j, i }, 4,
                    { rng:uniform(0, 256), rng:uniform(0, 256), rng:uniform(0, 256) },
                    cv.FILLED)
            end
        end
    end

    cv.imshow(myShiTomasi_window, myShiTomasi_copy)
end

-- Load source image and convert it to gray
local parser = argparse() {description='Code for Creating your own corner detector tutorial.'}
parser:option('--input'):description('Path to input image.'):default('building.jpg')
local args = parser:parse()

src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    print('Could not open or find the image:', args.input)
    os.exit(0)
end

local src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)

-- Set some parameters
local blockSize = 3
local apertureSize = 3

-- My Harris matrix -- Using cornerEigenValsAndVecs
local myHarris_dst = cv.cornerEigenValsAndVecs(src_gray, blockSize, apertureSize)

-- transform into an lua table for faster processing in lua
myHarris_dst = myHarris_dst:table()

-- calculate Mc
Mc = {}
for i = 1, src.rows do
    Mc[i] = {}
    for j = 1, src.cols do
        local lambda_1 = myHarris_dst[i][j][0 + INDEX_BASE]
        local lambda_2 = myHarris_dst[i][j][1 + INDEX_BASE]
        Mc[i][j] = lambda_1 * lambda_2 - 0.04 * math.pow((lambda_1 + lambda_2), 2)
    end
end

myHarris_minVal, myHarris_maxVal, _, _ = cv.minMaxLoc(cv.Mat.createFromArray(Mc, cv.CV_32F))

-- Create Window and Trackbar
cv.namedWindow(myHarris_window)
cv.createTrackbar('Quality Level:', myHarris_window, myHarris_qualityLevel, max_qualityLevel, myHarris_function)
myHarris_function(myHarris_qualityLevel)

-- My Shi-Tomasi -- Using cornerMinEigenVal
myShiTomasi_dst = cv.cornerMinEigenVal(src_gray, blockSize, kwargs({ ksize = apertureSize }))

myShiTomasi_minVal, myShiTomasi_maxVal, _, _ = cv.minMaxLoc(myShiTomasi_dst)

-- transform into an lua table for faster processing in lua
myShiTomasi_dst = myShiTomasi_dst:table()

-- Create Window and Trackbar
cv.namedWindow(myShiTomasi_window)
cv.createTrackbar('Quality Level:', myShiTomasi_window, myShiTomasi_qualityLevel, max_qualityLevel, myShiTomasi_function)
myShiTomasi_function(myShiTomasi_qualityLevel)

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly causes unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    -- Wait until user press some key
    local key = cv.waitKey(1)
    if key ~= -1 then break end
end
