package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/Histograms_Matching/back_projection/calcBackProject_Demo1.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local round = opencv_lua.math.round

local hue

local function Hist_and_Backproj(val)
    -- [initialize]
    local bins = val
    local histSize = math.max(bins, 2)
    local ranges = { 0, 180 } -- hue_range
    -- [initialize]

    -- [Get the Histogram and normalize it]
    local hist = cv.calcHist({ hue }, { 0 }, nil, { histSize }, ranges, kwargs({ accumulate = false }))
    cv.normalize(hist, hist, kwargs({ alpha = 0, beta = 255, norm_type = cv.NORM_MINMAX }))
    -- [Get the Histogram and normalize it]

    -- [Get Backprojection]
    local backproj = cv.calcBackProject({ hue }, { 0 }, hist, ranges, kwargs({ scale = 1 }))
    -- [Get Backprojection]

    -- [Draw the backproj]
    cv.imshow('BackProj', backproj)
    -- [Draw the backproj]

    -- [Draw the histogram]
    local w = 400
    local h = 400
    local bin_w = round(w / histSize)
    local histImg = cv.Mat.zeros(h, w, cv.CV_8UC3)

    for i = 0, bins - 1 do
        cv.rectangle(histImg, { i * bin_w, h }, { (i + 1) * bin_w, h - round(hist[i] * h / 255.0) }, { 0, 0, 255 },
            cv.FILLED)
    end

    cv.imshow('Histogram', histImg)
    -- [Draw the histogram]
end

-- [Read the image]
-- parser = argparse.ArgumentParser(description='Code for Back Projection tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='home.jpg')
-- args = parser.parse_args()

local args = {
    input = "home.jpg",
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
    error('Could not open or find the image: ' .. args.input)
end
-- [Read the image]

-- [Transform it to HSV]
local hsv = cv.cvtColor(src, cv.COLOR_BGR2HSV)
-- [Transform it to HSV]

-- [Use only the Hue value]
local ch = { 0, 0 }
hue = cv.Mat.zeros(hsv.rows, hsv.cols, hsv:type())
cv.mixChannels({ hsv }, { hue }, ch)
-- [Use only the Hue value]

-- [Create Trackbar to enter the number of bins]
local window_image = 'Source image'
cv.namedWindow(window_image)
local bins = 25
cv.createTrackbar('* Hue  bins: ', window_image, bins, 180, Hist_and_Backproj)
Hist_and_Backproj(bins)
-- [Create Trackbar to enter the number of bins]

-- [Show the image]
cv.imshow(window_image, src)

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
-- [Show the image]
