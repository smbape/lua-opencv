#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/Histograms_Matching/histogram_equalization/EqualizeHist_Demo.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv

-- [Load image]
local parser = argparse() {description='Code for Histogram Equalization tutorial.'}
parser:option('--input'):description('Path to input image.'):default('lena.jpg')
local args = parser:parse()

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
