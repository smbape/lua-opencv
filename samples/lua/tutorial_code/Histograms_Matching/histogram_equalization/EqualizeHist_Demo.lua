#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/Histograms_Matching/histogram_equalization/EqualizeHist_Demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

-- [Load image]
-- parser = argparse.ArgumentParser(description='Code for Histogram Equalization tutorial.')
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

-- [Convert to grayscale]
src = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
-- [Convert to grayscale]

-- [Apply Histogram Equalization]
local dst = cv.equalizeHist(src)
-- [Apply Histogram Equalization]

-- [Display results]
cv.imshow('Source image', src)
cv.imshow('Equalized Image', dst)
-- [Display results]

-- [Wait until user exits the program]
cv.waitKey()
-- [Wait until user exits the program]
