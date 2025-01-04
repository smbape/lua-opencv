#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/imgProc/changing_contrast_brightness_image/changing_contrast_brightness_image.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs
local int = opencv_lua.math.int

local function saturate_cast(value, minval, maxval)
    if value > maxval then return maxval end
    if value < minval then return minval end
    return value
end

local alpha = 1.0
local alpha_max = 500
local beta = 0
local beta_max = 200
local gamma = 1.0
local gamma_max = 200
local img_original

local function basicLinearTransform()
    local res = cv.convertScaleAbs(img_original, kwargs({ alpha = alpha, beta = beta }))
    local img_corrected = cv.hconcat({ img_original, res })
    cv.imshow("Brightness and contrast adjustments", img_corrected)
end

local function gammaCorrection()
    -- [changing-contrast-brightness-gamma-correction]
    local lookUpTable = cv.Mat(1, 256, cv.CV_8U)
    for i = 0, 255 do
        lookUpTable[i] = saturate_cast(((i / 255.0) ^ gamma) * 255.0, 0, 255)
    end

    local res = cv.LUT(img_original, lookUpTable)
    -- [changing-contrast-brightness-gamma-correction]

    local img_gamma_corrected = cv.hconcat({ img_original, res })
    cv.imshow("Gamma correction", img_gamma_corrected)
end

local function on_linear_transform_alpha_trackbar(val)
    alpha = val / 100
    basicLinearTransform()
end

local function on_linear_transform_beta_trackbar(val)
    beta = val - 100
    basicLinearTransform()
end

local function on_gamma_correction_trackbar(val)
    gamma = val / 100
    gammaCorrection()
end

local parser = argparse() {description='Code for Changing the contrast and brightness of an image! tutorial.'}
parser:option('--input'):description('Path to input image.'):default('lena.jpg')
local args = parser:parse()

img_original = cv.imread(cv.samples.findFile(args.input))
if img_original:empty() then
    error('Could not open or find the image: ' .. args.input)
end

cv.namedWindow('Brightness and contrast adjustments')
cv.namedWindow('Gamma correction')

local alpha_init = int(alpha * 100)
cv.createTrackbar('Alpha gain (contrast)', 'Brightness and contrast adjustments', alpha_init, alpha_max,
    on_linear_transform_alpha_trackbar)
local beta_init = beta + 100
cv.createTrackbar('Beta bias (brightness)', 'Brightness and contrast adjustments', beta_init, beta_max,
    on_linear_transform_beta_trackbar)
local gamma_init = int(gamma * 100)
cv.createTrackbar('Gamma correction', 'Gamma correction', gamma_init, gamma_max, on_gamma_correction_trackbar)

on_linear_transform_alpha_trackbar(alpha_init)
on_gamma_correction_trackbar(gamma_init)

while true do
    -- lua is not thread safe
    -- therefore, calling callbacks from another thread
    -- will certainly cause unwanted behaviour
    -- for that reason, callbacks are registered and called from the main thread
    opencv_lua.notifyCallbacks()

    -- Wait until user press some key
    local key = cv.waitKey(1)
    if key ~= -1 then break end
end
