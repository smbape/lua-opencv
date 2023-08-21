package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/ImgTrans/LaPlace/laplace_demo.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local kwargs = opencv_lua.kwargs

local function main(args)
    -- [variables]
    -- Declare the variables we are going to use
    local ddepth = cv.CV_16S
    local kernel_size = 3
    local window_name = "Laplace Demo"
    -- [variables]

    -- [load]
    local imageName = args.input

    local src = cv.imread(cv.samples.findFile(imageName), cv.IMREAD_COLOR) -- Load an image

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image')
        print('Program Arguments: [image_name -- default lena.jpg]')
        return -1
    end
    -- [load]

    -- [reduce_noise]
    -- Remove noise by blurring with a Gaussian filter
    src = cv.GaussianBlur(src, { 3, 3 }, 0)
    -- [reduce_noise]

    -- [convert_to_gray]
    -- Convert the image to grayscale
    local src_gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    -- [convert_to_gray]

    -- Create Window
    cv.namedWindow(window_name, cv.WINDOW_AUTOSIZE)

    -- [laplacian]
    -- Apply Laplace function
    local dst = cv.Laplacian(src_gray, ddepth, kwargs({ ksize = kernel_size }))
    -- [laplacian]

    -- [convert]
    -- converting back to uint8
    local abs_dst = cv.convertScaleAbs(dst)
    -- [convert]

    -- [display]
    cv.imshow(window_name, abs_dst)
    cv.waitKey(0)
    -- [display]

    return 0
end


local args = {
    input = 'lena.jpg',
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

main(args)
