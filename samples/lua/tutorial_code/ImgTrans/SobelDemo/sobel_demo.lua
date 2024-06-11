#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.10.0/samples/python/tutorial_code/ImgTrans/SobelDemo/sobel_demo.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

local function main(args)
    -- [variables]
    -- First we declare the variables we are going to use
    local window_name = 'Sobel Demo - Simple Edge Detector'
    local scale = 1
    local delta = 0
    local ddepth = cv.CV_16S
    -- [variables]

    -- [load]
    -- As usual we load our source image (src)
    -- Check number of arguments
    -- if len(argv) < 1:
    --     print ('Not enough parameters')
    --     print ('Usage:\nsobel_demo.py < path_to_image >')
    --     return -1

    -- Load the image
    local src = cv.imread(cv.samples.findFile(args.input), cv.IMREAD_COLOR)

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image: ' + args.input)
        return -1
    end
    -- [load]

    -- [reduce_noise]
    -- Remove noise by blurring with a Gaussian filter ( kernel size = 3 )
    local src = cv.GaussianBlur(src, { 3, 3 }, 0)
    -- [reduce_noise]

    -- [convert_to_gray]
    -- Convert the image to grayscale
    local gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    -- [convert_to_gray]

    -- [sobel]
    -- Gradient-X
    -- grad_x = cv.Scharr(gray,ddepth,1,0)
    local grad_x = cv.Sobel(gray, ddepth, 1, 0, kwargs({ ksize = 3, scale = scale, delta = delta,
        borderType = cv.BORDER_DEFAULT }))

    -- Gradient-Y
    -- grad_y = cv.Scharr(gray,ddepth,0,1)
    local grad_y = cv.Sobel(gray, ddepth, 0, 1, kwargs({ ksize = 3, scale = scale, delta = delta,
        borderType = cv.BORDER_DEFAULT }))
    -- [sobel]

    -- [convert]
    -- converting back to uint8
    local abs_grad_x = cv.convertScaleAbs(grad_x)
    local abs_grad_y = cv.convertScaleAbs(grad_y)
    -- [convert]

    -- [blend]
    -- Total Gradient (approximate)
    local grad = cv.addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0)
    -- [blend]

    -- [display]
    cv.imshow(window_name, grad)
    cv.waitKey(0)
    -- [display]

    return 0
end

local parser = argparse() {description='Sample code using Sobel and/or Scharr OpenCV functions to make a simple Edge Detector'}
parser:argument('input'):description('Path to input image.'):default('lena.jpg')
local args = parser:parse()

main(args)
