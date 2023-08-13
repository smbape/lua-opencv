package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1,1)) .. package.path
local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/samples/python/tutorial_code/core/mat_mask_operations/mat_mask_operations.py
--]]

-- [basic_method]
local function saturated(sum_value)
    if sum_value > 255 then return 255 end
    if sum_value < 0 then return 0 end
    return sum_value
end

local function sharpen(my_image)
    local is_grayscale = my_image:channels() == 1

    if not is_grayscale then
        my_image = cv.cvtColor(my_image, cv.CV_8U)
    end

    local height, width, n_channels = my_image.height, my_image.width, my_image:channels()
    local result = cv.Mat.zeros(my_image.rows, my_image.cols, cv.CV_MAKETYPE(my_image:depth(), my_image:channels()))

    -- make channels a new dimension to be able to do :at(j, i, k)
    if not is_grayscale then
        my_image = my_image:reshape(1, { height, width, n_channels })
        result = result:reshape(1, { height, width, n_channels })
    end

    -- [basic_method_loop]
    for j = 1, height - 2 do
        local image_row_before = my_image[j - 1]
        local image_row = my_image[j]
        local image_row_after = my_image[j + 1]
        local result_row = result[j]

        for i = 1, width - 2 do
            local image_center = image_row[i]
            local image_bottom = image_row_after[i]
            local image_top = image_row_before[i]
            local image_right = image_row[i + 1]
            local image_left = image_row[i - 1]

            if is_grayscale then
                local sum_value = 5 * image_center - image_bottom - image_top
                            - image_right - image_left
                result_row[i] = saturated(sum_value)
            else
                local result_col = result_row[i]
                for k = 0, n_channels - 1 do
                    local sum_value = 5 * image_center[k] - image_bottom[k] - image_top[k]
                            - image_right[k] - image_left[k]
                    result_col[k] = saturated(sum_value)
                end
            end
        end
    end
    -- [basic_method_loop]

    -- restore channels
    if not is_grayscale then
        result = result:reshape(n_channels, { height, width })
    end

    return result
end
-- [basic_method]

function main(argv)
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

    t = (os.clock() - t)
    print(string.format("Hand written function time passed in seconds: %.3f", t))

    cv.imshow("Output", dst0)
    cv.waitKey()

    t = os.clock()
    -- [kern]
    local kernel = cv.Mat.createFromArray({{0, -1, 0},
                       {-1, 5, -1},
                       {0, -1, 0}}, cv.CV_32F)  -- kernel should be floating point type
    -- [kern]
    -- [filter2D]
    local dst1 = cv.filter2D(src, -1, kernel)
    -- ddepth = -1, means destination image has depth same as input image
    -- [filter2D]

    t = (os.clock() - t)
    print(string.format("Built-in filter2D time passed in seconds:     %.3f", t))

    cv.imshow("Output", dst1)

    cv.waitKey(0)
    cv.destroyAllWindows()
    return 0
end

main(arg)
