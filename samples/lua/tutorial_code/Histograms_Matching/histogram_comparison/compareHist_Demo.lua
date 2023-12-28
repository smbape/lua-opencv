package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/Histograms_Matching/histogram_comparison/compareHist_Demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local int = opencv_lua.math.int

-- [Load three images with different environment settings]
-- parser = argparse.ArgumentParser(description='Code for Histogram Comparison tutorial.')
-- parser.add_argument('--input1', help='Path to input image 1.')
-- parser.add_argument('--input2', help='Path to input image 2.')
-- parser.add_argument('--input3', help='Path to input image 3.')
-- args = parser.parse_args()

local args = {
    input1 = "Histogram_Comparison_Source_0.jpg",
    input2 = "Histogram_Comparison_Source_1.jpg",
    input3 = "Histogram_Comparison_Source_2.jpg",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

local function concat(a, b)
    local c = {}
    for k, v in pairs(a) do c[k] = v end
    for k, v in pairs(b) do c[k + #a] = v end
    return c
end

local src_base = cv.imread(cv.samples.findFile(args.input1))
local src_test1 = cv.imread(cv.samples.findFile(args.input2))
local src_test2 = cv.imread(cv.samples.findFile(args.input3))
if src_base:empty() or src_test1:empty() or src_test2:empty() then
    error('Could not open or find the images!')
end
-- [Load three images with different environment settings]

-- [Convert to HSV]
local hsv_base = cv.cvtColor(src_base, cv.COLOR_BGR2HSV)
local hsv_test1 = cv.cvtColor(src_test1, cv.COLOR_BGR2HSV)
local hsv_test2 = cv.cvtColor(src_test2, cv.COLOR_BGR2HSV)
-- [Convert to HSV]

-- [Convert to HSV half]
local hsv_half_down = hsv_base:new(cv.Range(int(hsv_base.rows / 2), hsv_base.rows), cv.Range(0, hsv_base.cols))
-- [Convert to HSV half]

-- [Using 50 bins for hue and 60 for saturation]
local h_bins = 50
local s_bins = 60
local histSize = { h_bins, s_bins }

-- hue varies from 0 to 179, saturation from 0 to 255
local h_ranges = { 0, 180 }
local s_ranges = { 0, 256 }
local ranges = concat(h_ranges, s_ranges) -- concat lists

-- Use the 0-th and 1-st channels
local channels = { 0, 1 }
-- [Using 50 bins for hue and 60 for saturation]

-- [Calculate the histograms for the HSV images]
local hist_base = cv.calcHist({ hsv_base }, channels, nil, histSize, ranges, kwargs({ accumulate = false }))
cv.normalize(hist_base, hist_base, kwargs({ alpha = 0, beta = 1, norm_type = cv.NORM_MINMAX }))

local hist_half_down = cv.calcHist({ hsv_half_down }, channels, nil, histSize, ranges, kwargs({ accumulate = false }))
cv.normalize(hist_half_down, hist_half_down, kwargs({ alpha = 0, beta = 1, norm_type = cv.NORM_MINMAX }))

local hist_test1 = cv.calcHist({ hsv_test1 }, channels, nil, histSize, ranges, kwargs({ accumulate = false }))
cv.normalize(hist_test1, hist_test1, kwargs({ alpha = 0, beta = 1, norm_type = cv.NORM_MINMAX }))

local hist_test2 = cv.calcHist({ hsv_test2 }, channels, nil, histSize, ranges, kwargs({ accumulate = false }))
cv.normalize(hist_test2, hist_test2, kwargs({ alpha = 0, beta = 1, norm_type = cv.NORM_MINMAX }))
-- [Calculate the histograms for the HSV images]

-- [Apply the histogram comparison methods]
local methodNames = { "Correlation", "Chi-square", "Intersection", "Bhattacharyya" }
for compare_method = 0, 3 do
    local base_base = cv.compareHist(hist_base, hist_base, compare_method)
    local base_half = cv.compareHist(hist_base, hist_half_down, compare_method)
    local base_test1 = cv.compareHist(hist_base, hist_test1, compare_method)
    local base_test2 = cv.compareHist(hist_base, hist_test2, compare_method)

    print('Method:', methodNames[compare_method + 1], 'Perfect, Base-Half, Base-Test(1), Base-Test(2) :',
        base_base, '/', base_half, '/', base_test1, '/', base_test2)
end
-- [Apply the histogram comparison methods]
