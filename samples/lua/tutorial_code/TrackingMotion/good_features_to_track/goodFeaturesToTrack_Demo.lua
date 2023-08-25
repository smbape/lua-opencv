package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/TrackingMotion/good_features_to_track/goodFeaturesToTrack_Demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local int = opencv_lua.math.int

local source_window = 'Image'
local maxTrackbar = 100
local rng = cv.RNG(12345)
local src, src_gray

local function goodFeaturesToTrack_Demo(val)
    local maxCorners = math.max(val, 1)

    -- Parameters for Shi-Tomasi algorithm
    local qualityLevel = 0.01
    local minDistance = 10
    local blockSize = 3
    local gradientSize = 3
    local useHarrisDetector = false
    local k = 0.04

    -- Copy the source image
    local copy = src:copy()

    -- Apply corner detection
    local corners = cv.goodFeaturesToTrack(src_gray, maxCorners, qualityLevel, minDistance, nil,
        kwargs({ blockSize = blockSize, gradientSize = gradientSize, useHarrisDetector = useHarrisDetector, k = k }))

    -- Draw corners detected
    print('** Number of corners detected:', #corners)
    local radius = 4
    for i = 0, #corners - 1 do
        local pt = corners:Point_at(i, 0)
        cv.circle(copy, { int(pt.x), int(pt.y) }, radius, { rng:uniform(0, 256), rng:uniform(0, 256),
            rng:uniform(0, 256) }, cv.FILLED)
    end

    -- Show what you got
    cv.namedWindow(source_window)
    cv.imshow(source_window, copy)
end

-- Load source image and convert it to gray
-- parser = argparse.ArgumentParser(description='Code for Shi-Tomasi corner detector tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='pic3.png')
-- args = parser.parse_args()

local args = {
    input = 'pic3.png',
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    print('Could not open or find the image:', args.input)
    os.exit(0)
end

src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)

-- Create a window and a trackbar
cv.namedWindow(source_window)
local maxCorners = 23 -- initial threshold
cv.createTrackbar('Threshold: ', source_window, maxCorners, maxTrackbar, goodFeaturesToTrack_Demo)
cv.imshow(source_window, src)
goodFeaturesToTrack_Demo(maxCorners)

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly causes unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thred
    opencv_lua.notifyCallbacks()

    -- Wait until user press some key
    local key = cv.waitKey(1)
    if key ~= -1 then break end
end
