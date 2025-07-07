#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.12.0/samples/python/tutorial_code/core/mat_mask_operations/mat_mask_operations.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

-- [basic_method]
local function saturated(sum_value)
    if sum_value > 255 then return 255 end
    if sum_value < 0 then return 0 end
    return sum_value
end

local function copy_matrix(src)
    if type(src) ~= 'table' then
        return src
    end

    local dst = {}
    for i = 1, #src do
        dst[i] = copy_matrix(src[i])
    end
    return dst
end

local function sharpen(my_image)
    local is_grayscale = my_image:channels() == 1

    if not is_grayscale then
        my_image = cv.cvtColor(my_image, cv.COLOR_BGR2BGRA)
    end

    local height, width, n_channels, type = my_image.height, my_image.width, my_image:channels(), my_image:type()

    local t

    -- it is way faster (5 to 20 times) to do all the image manipulations in lua
    t = os.clock()
    my_image = my_image:table() -- get a table representation of the image
    print(string.format("table time passed in seconds: %.3f", os.clock() - t))

    -- intialize the result image
    t = os.clock()
    local result = copy_matrix(my_image)
    print(string.format("initialize time passed in seconds: %.3f", os.clock() - t))

    -- [basic_method_loop]
    t = os.clock()
    for j = 2, height - 1 do
        for i = 2, width - 1 do
            if is_grayscale then
                local sum_value = 5 * my_image[j][i] - my_image[j + 1][i] - my_image[j - 1][i]
                    - my_image[j][i + 1] - my_image[j][i - 1]
                result[j][i] = saturated(sum_value)
            else
                for k = 1, n_channels do
                    local sum_value = 5 * my_image[j][i][k] - my_image[j + 1][i][k]
                        - my_image[j - 1][i][k] - my_image[j][i + 1][k]
                        - my_image[j][i - 1][k]
                    result[j][i][k] = saturated(sum_value)
                end
            end
        end
    end
    print(string.format("sharpen time passed in seconds: %.3f", os.clock() - t))
    -- [basic_method_loop]

    t = os.clock()
    result = cv.Mat.createFromArray(result, type)
    print(string.format("createFromArray time passed in seconds: %.3f", os.clock() - t))

    return result
end
-- [basic_method]

local function main(argv)
    local filename = 'lena.jpg'

    local img_codec = cv.IMREAD_COLOR
    if #argv > 1 then
        filename = argv[1]
        if #argv >= 3 and argv[2] == "G" then
            img_codec = cv.IMREAD_GRAYSCALE
        end
    end

    local src = cv.imread(cv.samples.findFile(filename), img_codec)

    if src:empty() then
        print("Can't open image [" .. filename .. "]")
        print("Usage:")
        print("mat_mask_operations.py [image_path -- default lena.jpg] [G -- grayscale]")
        return -1
    end

    cv.namedWindow("Input", cv.WINDOW_AUTOSIZE)
    cv.namedWindow("Output", cv.WINDOW_AUTOSIZE)

    cv.imshow("Input", src)
    local t = os.clock()

    local dst0 = sharpen(src)

    print(string.format("Hand written function time passed in seconds: %.3f", os.clock() - t))

    cv.imshow("Output", dst0)
    cv.waitKey()

    t = os.clock()
    -- [kern]
    local kernel = cv.Mat.createFromArray({ { 0, -1, 0 },
        { -1, 5,  -1 },
        { 0,  -1, 0 } }, cv.CV_32F) -- kernel should be floating point type
    -- [kern]
    -- [filter2D]
    local dst1 = cv.filter2D(src, -1, kernel)
    -- ddepth = -1, means destination image has depth same as input image
    -- [filter2D]

    print(string.format("Built-in filter2D     time passed in seconds: %.3f", os.clock() - t))

    cv.imshow("Output", dst1)

    cv.waitKey(0)
    cv.destroyAllWindows()
    return 0
end

main(arg)
