package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/TrackingMotion/harris_detector/cornerHarris_Demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local int = opencv_lua.math.int

local source_window = 'Source image'
local corners_window = 'Corners detected'
local max_thresh = 255

local src_gray

local function cornerHarris_demo(val)
    local thresh = val

    -- Detector parameters
    local blockSize = 2
    local apertureSize = 3
    local k = 0.04

    -- Detecting corners
    local dst = cv.cornerHarris(src_gray, blockSize, apertureSize, k)

    -- Normalizing
    local dst_norm = cv.Mat(dst:size(), cv.CV_32F)
    cv.normalize(dst, dst_norm, kwargs({ alpha = 0, beta = 255, norm_type = cv.NORM_MINMAX }))
    local dst_norm_scaled = cv.convertScaleAbs(dst_norm)

    -- transform into an lua table for faster processing in lua
    local rows, cols = dst_norm.rows, dst_norm.cols
    dst_norm = dst_norm:table()

    -- Drawing a circle around corners
    for i = 1, rows do
        for j = 1, cols do
            if int(dst_norm[i][j]) > thresh then
                cv.circle(dst_norm_scaled, { j - 1, i - 1 }, 5, { 0 }, 2)
            end
        end
    end

    -- Showing the result
    cv.namedWindow(corners_window)
    cv.imshow(corners_window, dst_norm_scaled)
end

-- Load source image and convert it to gray
-- parser = argparse.ArgumentParser(description='Code for Harris corner detector tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='building.jpg')
-- args = parser.parse_args()

local args = {
    input = 'building.jpg',
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

local src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    print('Could not open or find the image:', args.input)
    os.exit(0)
end

src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)

-- Create a window and a trackbar
cv.namedWindow(source_window)
local thresh = 200 -- initial threshold
cv.createTrackbar('Threshold: ', source_window, thresh, max_thresh, cornerHarris_demo)
cv.imshow(source_window, src)
cornerHarris_demo(thresh)

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
