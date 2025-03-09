#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/ml/introduction_to_pca/introduction_to_pca.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local bit = bit or opencv_lua.bit ---@diagnostic disable-line: undefined-global
local atan2 = math.atan2 or math.atan ---@diagnostic disable-line: deprecated
local int = opencv_lua.math.int
local INDEX_BASE = 1 -- lua is 1-based indexed

local function drawAxis(img, p_, q_, colour, scale)
    local i0 = 0 + INDEX_BASE
    local i1 = 1 + INDEX_BASE

    local p = { p_[i0], p_[i1] }
    local q = { q_[i0], q_[i1] }

    -- [visualization1]
    local angle = atan2(p[i1] - q[i1], p[i0] - q[i0]) -- angle in radians
    local hypotenuse = math.sqrt((p[i1] - q[i1]) * (p[i1] - q[i1]) + (p[i0] - q[i0]) * (p[i0] - q[i0]))

    -- Here we lengthen the arrow by a factor of scale
    q[i0] = p[i0] - scale * hypotenuse * math.cos(angle)
    q[i1] = p[i1] - scale * hypotenuse * math.sin(angle)
    cv.line(img, { int(p[i0]), int(p[i1]) }, { int(q[i0]), int(q[i1]) }, colour, 1, cv.LINE_AA)

    -- create the arrow hooks
    p[i0] = q[i0] + 9 * math.cos(angle + math.pi / 4)
    p[i1] = q[i1] + 9 * math.sin(angle + math.pi / 4)
    cv.line(img, { int(p[i0]), int(p[i1]) }, { int(q[i0]), int(q[i1]) }, colour, 1, cv.LINE_AA)

    p[i0] = q[i0] + 9 * math.cos(angle - math.pi / 4)
    p[i1] = q[i1] + 9 * math.sin(angle - math.pi / 4)
    cv.line(img, { int(p[i0]), int(p[i1]) }, { int(q[i0]), int(q[i1]) }, colour, 1, cv.LINE_AA)
    -- [visualization1]
end

local function getOrientation(pts, img)
    -- [pca]
    -- Construct a buffer used by the pca analysis
    local sz = pts.rows
    local cols = pts.cols

    local data_pts = cv.Mat(sz, 2, cv.CV_64F)
    for i = 0, sz - 1 do
        data_pts[{ i, 0 }] = pts[{ i, 0, 0 }]
        data_pts[{ i, 1 }] = pts[{ i, 0, 1 }]
    end

    -- Perform PCA analysis
    local mean, eigenvectors, eigenvalues = cv.PCACompute2(data_pts, nil)

    -- Store the center of the object
    local cntr = { int(mean[0]), int(mean[1]) }
    -- [pca]

    -- [visualization]
    -- Draw the principal components
    cv.circle(img, cntr, 3, { 255, 0, 255 }, 2)
    local p1 = { cntr[1] + 0.02 * eigenvectors(0, 0) * eigenvalues(0, 0), cntr[2] + 0.02 * eigenvectors(0, 1) * eigenvalues(0, 0) }
    local p2 = { cntr[1] - 0.02 * eigenvectors(1, 0) * eigenvalues(1, 0), cntr[2] - 0.02 * eigenvectors(1, 1) * eigenvalues(1, 0) }
    drawAxis(img, cntr, p1, { 0, 255, 0 }, 1)
    drawAxis(img, cntr, p2, { 255, 255, 0 }, 5)

    local angle = atan2(eigenvectors(0, 1), eigenvectors(0, 0)) -- orientation in radians
    -- [visualization]

    return angle
end

-- [pre-process]
-- Load image
local parser = argparse() {description='Code for Introduction to Principal Component Analysis (PCA) tutorial.\
                                              This program demonstrates how to use OpenCV PCA to extract the orientation of an object.'}
parser:option('--input'):description('Path to input image.'):default('pca_test1.jpg')
local args = parser:parse()

local src = cv.imread(cv.samples.findFile(args.input))
-- Check if image is loaded successfully
if src:empty() then
    print('Could not open or find the image: ', args.input)
    os.exit(0)
end

cv.imshow('src', src)

-- Convert image to grayscale
local gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)

-- Convert image to binary
local _, bw = cv.threshold(gray, 50, 255, bit.bor(cv.THRESH_BINARY, cv.THRESH_OTSU))
-- [pre-process]

-- [contours]
-- Find all the contours in the thresholded image
local contours, _ = cv.findContours(bw, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)

for i, c in ipairs(contours) do
    -- Calculate the area of each contour
    local area = cv.contourArea(c)
    -- Ignore contours that are too small or too large
    if area >= 1e2 and 1e5 >= area then
        -- Draw each contour only for visualisation purposes
        cv.drawContours(src, contours, i - 1, { 0, 0, 255 }, 2)
        -- Find the orientation of each shape
        getOrientation(c, src)
    end
end
-- [contours]

cv.imshow('output', src)
cv.waitKey()
