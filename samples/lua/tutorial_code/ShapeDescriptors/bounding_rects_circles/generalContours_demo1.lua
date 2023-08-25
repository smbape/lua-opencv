package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/ShapeDescriptors/bounding_rects_circles/generalContours_demo1.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int

-- Make the starting point predictable
local rng = cv.RNG(12345)

local src_gray
local thresh_offset = 10

local function thresh_callback(val)
    local threshold = val + thresh_offset -- avoid too low values because they give thousands of contours

    -- [Canny]
    -- Detect edges using Canny
    local canny_output = cv.Canny(src_gray, threshold, threshold * 2)
    -- [Canny]

    -- [findContours]
    -- Find contours
    local contours, _ = cv.findContours(canny_output, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
    -- [findContours]

    -- [allthework]
    -- Approximate contours to polygons + get bounding rects and circles
    local contours_poly = {}
    local boundRect = {}
    local centers = {}
    local radius = {}
    for i, c in contours:pairs() do
        contours_poly[i] = cv.approxPolyDP(c, 3, true)
        boundRect[i] = cv.boundingRect(contours_poly[i])
        centers[i], radius[i] = cv.minEnclosingCircle(contours_poly[i])
    end
    -- [allthework]

    -- [zeroMat]
    local drawing = cv.Mat.zeros(canny_output:size(), cv.CV_8UC3)
    -- [zeroMat]

    -- [forContour]
    -- Draw polygonal contour + bonding rects + circles
    for i = 1, #contours do
        local color = { rng:uniform(0, 256), rng:uniform(0, 256), rng:uniform(0, 256) }
        cv.drawContours(drawing, contours_poly, i - 1, color)
        cv.rectangle(drawing, { int(boundRect[i].x), int(boundRect[i].y) },
            { int(boundRect[i].x + boundRect[i].width), int(boundRect[i].y + boundRect[i].height) }, color, 2)
        cv.circle(drawing, { int(centers[i].x), int(centers[i].y) }, int(radius[i]), color, 2)
    end
    -- [forContour]

    -- [showDrawings]
    -- Show in a window
    cv.imshow('Contours', drawing)
    -- [showDrawings]
end

-- [setup]
-- Load source image
-- parser = argparse.ArgumentParser(description='Code for Creating Bounding boxes and circles for contours tutorial.')
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
-- [setup]

-- [createWindow]
-- Create Window
local source_window = 'Source'
cv.namedWindow(source_window)
cv.imshow(source_window, src)
-- [createWindow]
-- [trackbar]
local max_thresh = 255 - thresh_offset
local thresh = 100 - thresh_offset -- initial threshold
cv.createTrackbar('Canny Thresh:', source_window, thresh, max_thresh, thresh_callback)
thresh_callback(thresh)
-- [trackbar]

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
