package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/ShapeDescriptors/hull/hull_demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

-- Make the starting point predictable
local rng = cv.RNG(12345)

local src_gray
local thresh_offset = 10

local function thresh_callback(val)
    local threshold = val + thresh_offset

    -- Detect edges using Canny
    local canny_output = cv.Canny(src_gray, threshold, threshold * 2)

    -- Find contours
    local contours, hierarchy = cv.findContours(canny_output, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

    -- Find the convex hull object for each contour
    local hull_list = {}
    for i, c in contours:pairs() do
        local hull = cv.convexHull(c)
        hull_list[i] = hull
    end

    -- Draw contours + hull results
    local drawing = cv.Mat.zeros(canny_output:size(), cv.CV_8UC3)
    for i = 1, #contours do
        local color = { rng:uniform(0, 256), rng:uniform(0, 256), rng:uniform(0, 256) }
        cv.drawContours(drawing, contours, i - 1, color)
        cv.drawContours(drawing, hull_list, i - 1, color)
    end

    -- Show in a window
    cv.imshow('Contours', drawing)
end

-- Load source image
-- parser = argparse.ArgumentParser(description='Code for Convex Hull tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='stuff.jpg')
-- args = parser.parse_args()

local args = {
    input = 'stuff.jpg',
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

-- Convert image to gray and blur it
src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
src_gray = cv.blur(src_gray, { 3, 3 })

-- Create Window
local source_window = 'Source'
cv.namedWindow(source_window)
cv.imshow(source_window, src)
local max_thresh = 255 - thresh_offset
local thresh = 100 - thresh_offset -- initial threshold
cv.createTrackbar('Canny Thresh:', source_window, thresh, max_thresh, thresh_callback)
thresh_callback(thresh)

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
