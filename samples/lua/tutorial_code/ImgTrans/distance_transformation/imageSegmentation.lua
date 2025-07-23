#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/ImgTrans/distance_transformation/imageSegmentation.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global
local INDEX_BASE = 1 -- lua is 1-based indexed

-- Make the starting point predictable
local rng = cv.RNG(12345)

-- [load_image]
-- Load the image
local parser = argparse() {description='Code for Image Segmentation with Distance Transform and Watershed Algorithm.\
    Sample code showing how to segment overlapping objects using Laplacian filtering, \
    in addition to Watershed and Distance Transformation'}
parser:option('--input'):description('Path to input image.'):default('cards.png')
local args = parser:parse()

local t = os.clock()
local src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    error('Could not open or find the image: ' .. args.input)
end

-- Show source image
cv.imshow('Source Image', src)
-- [load_image]

-- [black_bg]
-- Change the background from white to black, since that will help later to extract
-- better results during the use of Distance Transform
local mask = cv.inRange(src, { 255, 255, 255 }, { 255, 255, 255 })
src:setTo(0, mask);

-- Show output image
cv.imshow('Black Background Image', src)
-- [black_bg]

-- [sharp]
-- Create a kernel that we will use to sharpen our image
-- an approximation of second derivative, a quite strong kernel
local kernel = cv.Mat.createFromArray({ { 1, 1, 1 }, { 1, -8, 1 }, { 1, 1, 1 } }, cv.CV_32F)

-- do the laplacian filtering as it is
-- well, we need to convert everything in something more deeper then CV_8U
-- because the kernel has some negative values,
-- and we can expect in general to have a Laplacian image with negative values
-- BUT a 8bits unsigned int (the one we are working with) can contain values from 0 to 255
-- so the possible negative number will be truncated
local imgLaplacian = cv.filter2D(src, cv.CV_32F, kernel)
local sharp = src:convertTo(cv.CV_32F)
local imgResult = sharp - imgLaplacian

-- convert back to 8bits gray scale
imgResult = imgResult:convertTo(cv.CV_8UC3)
imgLaplacian = imgLaplacian:convertTo(cv.CV_8UC3)

--cv.imshow('Laplace Filtered Image', imgLaplacian)
cv.imshow('New Sharped Image', imgResult)
-- [sharp]

-- [bin]
-- Create binary image from source image
local bw = cv.cvtColor(imgResult, cv.COLOR_BGR2GRAY)
_, bw = cv.threshold(bw, 40, 255, bit.bor(cv.THRESH_BINARY, cv.THRESH_OTSU))
cv.imshow('Binary Image', bw)
-- [bin]

-- [dist]
-- Perform the distance transform algorithm
local dist = cv.distanceTransform(bw, cv.DIST_L2, 3)

-- Normalize the distance image for range = {0.0, 1.0}
-- so we can visualize and threshold it
cv.normalize(dist, dist, 0, 1.0, cv.NORM_MINMAX)
cv.imshow('Distance Transform Image', dist)
-- [dist]

-- [peaks]
-- Threshold to obtain the peaks
-- This will be the markers for the foreground objects
_, dist = cv.threshold(dist, 0.4, 1.0, cv.THRESH_BINARY)

-- Dilate a bit the dist image
local kernel1 = cv.Mat.ones(3, 3, cv.CV_8U)
dist = cv.dilate(dist, kernel1)
cv.imshow('Peaks', dist)
-- [peaks]

-- [seeds]
-- Create the CV_8U version of the distance image
-- It is needed for findContours()
local dist_8u = dist:convertTo(cv.CV_8U)

-- Find total markers
local contours, _ = cv.findContours(dist_8u, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
local n_contours = #contours

-- Create the marker image for the watershed algorithm
local markers = cv.Mat.zeros(dist:size(), cv.CV_32S)

-- Draw the foreground markers
for i = 0, n_contours - 1 do
    cv.drawContours(markers, contours, i, (i + 1), -1)
end

-- Draw the background marker
cv.circle(markers, { 5, 5 }, 3, { 255, 255, 255 }, -1)
local markers_8u = markers:convertTo(cv.CV_8U, nil, 10)
cv.imshow('Markers', markers_8u)
-- [seeds]

-- [watershed]
-- Perform the watershed algorithm
cv.watershed(imgResult, markers)

local mark = markers:convertTo(cv.CV_8U)
mark = cv.bitwise_not(mark)
-- uncomment this if you want to see how the mark
-- image looks like at that point
-- cv.imshow('Markers_v2', mark)

-- Generate random colors
local colors = {}
for i = 1, n_contours do
    colors[i] = { rng:uniform_int(0, 256), rng:uniform_int(0, 256), rng:uniform_int(0, 256) }
end

-- Create the result image
local dst = cv.Mat.zeros(markers:size(), cv.CV_8UC3)

local rows, cols, channels, depth = dst.rows, dst.cols, dst:channels(), dst:depth()

-- transform into an lua table for faster processing in lua
markers = markers:table()
dst = dst:table()

-- Fill labeled objects with random colors
for i = 1, rows do
    for j = 1, cols do
        local index = markers[i][j]
        if index > 0 and index <= n_contours then
            dst[i][j] = colors[index]
        end
    end
end
print(string.format("Hand written function time passed in seconds: %.3f", os.clock() - t))

-- transform back to matrix
dst = cv.Mat.createFromArray(dst, depth):reshape(channels, { rows, cols })

-- Visualize the final image
cv.imshow('Final Result', dst)
-- [watershed]

cv.waitKey()
