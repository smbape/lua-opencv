#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/erosion_dilatation/morphology_1.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local src = nil
local max_elem = 2
local max_kernel_size = 21
local title_trackbar_element_shape = 'Element:\n 0: Rect \n 1: Cross \n 2: Ellipse'
local title_trackbar_kernel_size = 'Kernel size:\n 2n + 1'
local title_erosion_window = 'Erosion Demo'
local title_dilation_window = 'Dilation Demo'


-- optional mapping of values with morphological shapes
local function morph_shape(val)
    if val == 0 then
        return cv.MORPH_RECT
    elseif val == 1 then
        return cv.MORPH_CROSS
    elseif val == 2 then
        return cv.MORPH_ELLIPSE
    end
end

-- [erosion]
local function erosion(val)
    local erosion_size = cv.getTrackbarPos(title_trackbar_kernel_size, title_erosion_window)
    local erosion_shape = morph_shape(cv.getTrackbarPos(title_trackbar_element_shape, title_erosion_window))

    -- [kernel]
    local element = cv.getStructuringElement(erosion_shape, { 2 * erosion_size + 1, 2 * erosion_size + 1 },
        { erosion_size, erosion_size })
    -- [kernel]
    local erosion_dst = cv.erode(src, element)
    cv.imshow(title_erosion_window, erosion_dst)
end
-- [erosion]


-- [dilation]
local function dilatation(val)
    local dilatation_size = cv.getTrackbarPos(title_trackbar_kernel_size, title_dilation_window)
    local dilation_shape = morph_shape(cv.getTrackbarPos(title_trackbar_element_shape, title_dilation_window))

    local element = cv.getStructuringElement(dilation_shape, { 2 * dilatation_size + 1, 2 * dilatation_size + 1 },
        { dilatation_size, dilatation_size })
    local dilatation_dst = cv.dilate(src, element)
    cv.imshow(title_dilation_window, dilatation_dst)
end
-- [dilation]

-- [main]
local function main(image)
    src = cv.imread(cv.samples.findFile(image))
    if src:empty() then
        error('Could not open or find the image: ' .. image)
    end

    cv.namedWindow(title_erosion_window)
    cv.createTrackbar(title_trackbar_element_shape, title_erosion_window, 0, max_elem, erosion)
    cv.createTrackbar(title_trackbar_kernel_size, title_erosion_window, 0, max_kernel_size, erosion)

    cv.namedWindow(title_dilation_window)
    cv.createTrackbar(title_trackbar_element_shape, title_dilation_window, 0, max_elem, dilatation)
    cv.createTrackbar(title_trackbar_kernel_size, title_dilation_window, 0, max_kernel_size, dilatation)

    erosion(0)
    dilatation(0)

    while true do
        -- lua is not thread safe
        -- therefore, calling callbacks from another thread
        -- will certainly causes unwanted behaviour
        -- for that reason, callbacks are registered and called from the main thread
        opencv_lua.notifyCallbacks()

        -- Wait until user press some key
        local key = cv.waitKey(1)
        if key ~= -1 then break end
    end
end
-- [main]


-- parser = argparse.ArgumentParser(description='Code for Eroding and Dilating tutorial.')
-- parser.add_argument('--input', help='Path to input image.', default='LinuxLogo.jpg')
-- args = parser.parse_args()

local args = {
    input = "LinuxLogo.jpg",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

main(args.input)
