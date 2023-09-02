package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/Histograms_Matching/histogram_calculation/calcHist_Demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local round = opencv_lua.math.round
local int = opencv_lua.math.int

-- [Load image]
-- parser = argparse.ArgumentParser(description='Code for Histogram Calculation tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='lena.jpg')
-- args = parser.parse_args()

local args = {
    input = "lena.jpg",
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
-- [Load image]

-- [Separate the image in 3 places ( B, G and R )]
local bgr_planes = cv.split(src)
-- [Separate the image in 3 places ( B, G and R )]

-- [Establish the number of bins]
local histSize = 256
-- [Establish the number of bins]

-- [Set the ranges ( for B,G,R) )]
local histRange = { 0, 256 } -- the upper boundary is exclusive
-- [Set the ranges ( for B,G,R) )]

-- [Set histogram param]
local accumulate = false
-- [Set histogram param]

-- [Compute the histograms]
local b_hist = cv.calcHist(bgr_planes, { 0 }, nil, { histSize }, histRange, kwargs({ accumulate = accumulate }))
local g_hist = cv.calcHist(bgr_planes, { 1 }, nil, { histSize }, histRange, kwargs({ accumulate = accumulate }))
local r_hist = cv.calcHist(bgr_planes, { 2 }, nil, { histSize }, histRange, kwargs({ accumulate = accumulate }))
-- [Compute the histograms]

-- [Draw the histograms for B, G and R]
local hist_w = 512
local hist_h = 400
local bin_w = int(round(hist_w / histSize))

local histImage = cv.Mat.zeros(hist_h, hist_w, cv.CV_8UC3)
-- [Draw the histograms for B, G and R]

-- [Normalize the result to ( 0, histImage.rows )]
cv.normalize(b_hist, b_hist, kwargs({ alpha = 0, beta = hist_h, norm_type = cv.NORM_MINMAX }))
cv.normalize(g_hist, g_hist, kwargs({ alpha = 0, beta = hist_h, norm_type = cv.NORM_MINMAX }))
cv.normalize(r_hist, r_hist, kwargs({ alpha = 0, beta = hist_h, norm_type = cv.NORM_MINMAX }))
-- [Normalize the result to ( 0, histImage.rows )]

-- [Draw for each channel]
for i = 1, histSize - 1 do
    cv.line(histImage, { bin_w * (i - 1), hist_h - int(b_hist[i - 1]) },
        { bin_w * (i), hist_h - int(b_hist[i]) },
        { 255, 0, 0 }, kwargs({ thickness = 2 }))
    cv.line(histImage, { bin_w * (i - 1), hist_h - int(g_hist[i - 1]) },
        { bin_w * (i), hist_h - int(g_hist[i]) },
        { 0, 255, 0 }, kwargs({ thickness = 2 }))
    cv.line(histImage, { bin_w * (i - 1), hist_h - int(r_hist[i - 1]) },
        { bin_w * (i), hist_h - int(r_hist[i]) },
        { 0, 0, 255 }, kwargs({ thickness = 2 }))
end
-- [Draw for each channel]

-- [Display]
cv.imshow('Source image', src)
cv.imshow('calcHist Demo', histImage)
cv.waitKey()
-- [Display]
