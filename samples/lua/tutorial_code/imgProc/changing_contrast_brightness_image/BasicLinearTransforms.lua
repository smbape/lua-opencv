#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/imgProc/changing_contrast_brightness_image/BasicLinearTransforms.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv

local function saturate_cast(value, minval, maxval)
    if value > maxval then return maxval end
    if value < minval then return minval end
    return value
end

-- Read image given by user
-- [basic-linear-transform-load]
local parser = argparse() {description='Code for Changing the contrast and brightness of an image! tutorial.'}
parser:option('--input'):description('Path to input image.'):default('lena.jpg')
local args = parser:parse()

local image = cv.imread(cv.samples.findFile(args.input))
if image:empty() then
    error('Could not open or find the image: ' .. args.input)
end
-- [basic-linear-transform-load]

-- [basic-linear-transform-output]
local new_image = cv.Mat.zeros(image:size(), image:type())
-- [basic-linear-transform-output]

-- [basic-linear-transform-parameters]
local alpha = 1.0 -- Simple contrast control
local beta = 0    -- Simple brightness control

-- Initialize values
print(' Basic Linear Transforms ')
print('-------------------------')

io.write('* Enter the alpha value [1.0-3.0]: ')
local alpha = io.read("*n") -- read a number

io.write('* Enter the beta value [0-100]: ')
local beta = io.read("*n") -- read a number
-- [basic-linear-transform-parameters]

local rows, cols, channels, type_ = image.rows, image.cols, image:channels(), image:type()

-- transform into an lua table for faster processing in lua
image = image:table()
new_image = new_image:table()

-- Do the operation new_image(i,j) = alpha*image(i,j) + beta
-- Instead of these 'for' loops we could have used simply:
-- new_image = cv.convertScaleAbs(image, alpha=alpha, beta=beta)
-- but we wanted to show you how to access the pixels :)
-- [basic-linear-transform-operation]
for y = 1, rows do
    for x = 1, cols do
        for c = 1, channels do
            new_image[y][x][c] = saturate_cast(alpha * image[y][x][c] + beta, 0, 255)
        end
    end
end

-- restore matrices
image = cv.Mat.createFromArray(image, type_)
new_image = cv.Mat.createFromArray(new_image, type_)

-- [basic-linear-transform-operation]

-- [basic-linear-transform-display]
-- Show stuff
cv.imshow('Original Image', image)
cv.imshow('New Image', new_image)

-- Wait until user press some key
cv.waitKey()
-- [basic-linear-transform-display]
