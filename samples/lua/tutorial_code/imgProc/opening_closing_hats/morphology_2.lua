#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/opening_closing_hats/morphology_2.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local morph_size = 0
local max_operator = 4
local max_elem = 2
local max_kernel_size = 21
local title_trackbar_operator_type = 'Operator:\n 0: Opening - 1: Closing  \n 2: Gradient - 3: Top Hat \n 4: Black Hat'
local title_trackbar_element_type = 'Element:\n 0: Rect - 1: Cross - 2: Ellipse'
local title_trackbar_kernel_size = 'Kernel size:\n 2n + 1'
local title_window = 'Morphology Transformations Demo'
local morph_op_dic = { cv.MORPH_OPEN, cv.MORPH_CLOSE, cv.MORPH_GRADIENT, cv.MORPH_TOPHAT, cv.MORPH_BLACKHAT }
local INDEX_BASE = 1 -- lua is 1-based indexed

local src

local function morphology_operations(val)
    local morph_operator = cv.getTrackbarPos(title_trackbar_operator_type, title_window)
    morph_size = cv.getTrackbarPos(title_trackbar_kernel_size, title_window)
    local morph_elem = 0
    local val_type = cv.getTrackbarPos(title_trackbar_element_type, title_window)
    if val_type == 0 then
        morph_elem = cv.MORPH_RECT
    elseif val_type == 1 then
        morph_elem = cv.MORPH_CROSS
    elseif val_type == 2 then
        morph_elem = cv.MORPH_ELLIPSE
    end

    local element = cv.getStructuringElement(morph_elem, { 2 * morph_size + 1, 2 * morph_size + 1 },
        { morph_size, morph_size })
    local operation = morph_op_dic[morph_operator + INDEX_BASE]
    local dst = cv.morphologyEx(src, operation, element)
    cv.imshow(title_window, dst)
end

-- parser = argparse.ArgumentParser(description='Code for More Morphology Transformations tutorial.')
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

src = cv.imread(cv.samples.findFile(args.input))
if src:empty() then
    error('Could not open or find the image: ' .. args.input)
end

cv.namedWindow(title_window)
cv.createTrackbar(title_trackbar_operator_type, title_window, 0, max_operator, morphology_operations)
cv.createTrackbar(title_trackbar_element_type, title_window, 0, max_elem, morphology_operations)
cv.createTrackbar(title_trackbar_kernel_size, title_window, 0, max_kernel_size, morphology_operations)

morphology_operations(0)

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
