package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/ImgTrans/Filter2D/filter2D.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local function main(args)
    local window_name = 'filter2D Demo'

    -- [load]
    local imageName = args.input

    -- Loads an image
    local src = cv.imread(cv.samples.findFile(imageName), cv.IMREAD_COLOR)

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image!')
        print('Usage: filter2D.py [image_name -- default lena.jpg] \n')
        return -1
    end
    -- [load]
    -- [init_arguments]
    -- Initialize ddepth argument for the filter
    local ddepth = -1
    -- [init_arguments]
    -- Loop - Will filter the image with different kernel sizes each 0.5 seconds
    local ind = 0
    while true do
        -- [update_kernel]
        -- Update kernel size for a normalized box filter
        local kernel_size = 3 + 2 * (ind % 5)
        local kernel = cv.Mat.ones(kernel_size, kernel_size, cv.CV_32F)
        kernel = kernel / (kernel_size * kernel_size)
        -- [update_kernel]
        -- [apply_filter]
        -- Apply filter
        local dst = cv.filter2D(src, ddepth, kernel)
        -- [apply_filter]
        cv.imshow(window_name, dst)

        local c = cv.waitKey(500)
        if c == 0x1b then
            break
        end

        ind = ind + 1
    end

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
