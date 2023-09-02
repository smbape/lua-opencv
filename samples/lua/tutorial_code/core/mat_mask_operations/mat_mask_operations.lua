package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/core/mat_mask_operations/mat_mask_operations.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

-- [basic_method]
local function saturated(sum_value)
    if sum_value > 255 then return 255 end
    if sum_value < 0 then return 0 end
    return sum_value
end

local function sharpen(my_image)
    local is_grayscale = my_image:channels() == 1

    if not is_grayscale then
        my_image = cv.cvtColor(my_image, cv.COLOR_BGR2BGRA)
    end

    local height, width, n_channels = my_image.height, my_image.width, my_image:channels()
    local result = cv.Mat.zeros(my_image:size(), my_image:type())

    -- give channels a new dimension to be able to do mat(j, i, k), which is a speed sweet spot
    if not is_grayscale then
        my_image = my_image:reshape(1, { height, width, n_channels })
        result = result:reshape(1, { height, width, n_channels })
    end

    -- [basic_method_loop]
    for j = 1, height - 2 do
        for i = 1, width - 2 do
            if is_grayscale then
                local sum_value = 5 * my_image(j, i) - my_image(j + 1, i) - my_image(j - 1, i)
                    - my_image(j, i + 1) - my_image(j, i - 1)
                result:set(saturated(sum_value), j, i)
            else
                for k = 0, n_channels - 1 do
                    local sum_value = 5 * my_image(j, i, k) - my_image(j + 1, i, k)
                        - my_image(j - 1, i, k) - my_image(j, i + 1, k)
                        - my_image(j, i - 1, k)
                    result:set(saturated(sum_value), j, i, k)
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
        { 0,  -1, 0 } }, cv.CV_32F)            -- kernel should be floating point type
    -- [kern]
    -- [filter2D]
    local dst1 = cv.filter2D(src, -1, kernel)
    -- ddepth = -1, means destination image has depth same as input image
    -- [filter2D]

    print(string.format("Hand written function time passed in seconds: %.3f", os.clock() - t))

    cv.imshow("Output", dst1)

    cv.waitKey(0)
    cv.destroyAllWindows()
    return 0
end

main(arg)
