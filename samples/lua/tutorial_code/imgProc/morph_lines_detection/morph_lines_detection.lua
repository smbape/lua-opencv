#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/morph_lines_detection/morph_lines_detection.py
    https://github.com/opencv/opencv/blob/4.9.0/samples/cpp/tutorial_code/ImgProc/morph_lines_detection/Morphology_3.cpp
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv

local function show_wait_destroy(winname, img)
    cv.imshow(winname, img)
    cv.moveWindow(winname, 500, 0)
    cv.waitKey(0)
    cv.destroyWindow(winname)
end


local function main(args)
    -- [load_image]
    -- Check number of arguments
    -- if #argv < 1 then
    --     print ('Not enough parameters')
    --     print ('Usage:\nmorph_lines_detection.py < path_to_image >')
    --     return -1
    -- end

    -- Load the image
    local src = cv.imread(cv.samples.findFile(args.input), cv.IMREAD_COLOR)

    -- Check if image is loaded fine
    if src:empty() then
        print('Error opening image: ' + args.input)
        return -1
    end

    -- Show source image
    cv.imshow("src", src)
    -- [load_image]

    -- [gray]
    -- Transform source image to gray if it is not already
    local gray
    if src:channels() ~= 1 then
        gray = cv.cvtColor(src, cv.COLOR_BGR2GRAY)
    else
        gray = src
    end

    -- Show gray image
    show_wait_destroy("gray", gray)
    -- [gray]

    -- [bin]
    -- Apply adaptiveThreshold at the bitwise_not of gray, notice the ~ symbol
    gray = cv.bitwise_not(gray)
    local bw = cv.adaptiveThreshold(gray, 255, cv.ADAPTIVE_THRESH_MEAN_C,
        cv.THRESH_BINARY, 15, -2)
    -- Show binary image
    show_wait_destroy("binary", bw)
    -- [bin]

    -- [init]
    -- Create the images that will use to extract the horizontal and vertical lines
    local horizontal = bw:copy()
    local vertical = bw:copy()
    -- [init]

    -- [horiz]
    -- Specify size on horizontal axis
    local cols = horizontal.cols
    local horizontal_size = math.floor(cols / 30)

    -- Create structure element for extracting horizontal lines through morphology operations
    local horizontalStructure = cv.getStructuringElement(cv.MORPH_RECT, { horizontal_size, 1 })

    -- Apply morphology operations
    local horizontal = cv.erode(horizontal, horizontalStructure)
    local horizontal = cv.dilate(horizontal, horizontalStructure)

    -- Show extracted horizontal lines
    show_wait_destroy("horizontal", horizontal)
    -- [horiz]

    -- [vert]
    -- Specify size on vertical axis
    local rows = vertical.rows
    local verticalsize = math.floor(rows / 30)

    -- Create structure element for extracting vertical lines through morphology operations
    local verticalStructure = cv.getStructuringElement(cv.MORPH_RECT, { 1, verticalsize })

    -- Apply morphology operations
    vertical = cv.erode(vertical, verticalStructure)
    vertical = cv.dilate(vertical, verticalStructure)

    -- Show extracted vertical lines
    show_wait_destroy("vertical", vertical)
    -- [vert]

    -- [smooth]
    -- Inverse vertical image
    vertical = cv.bitwise_not(vertical)
    show_wait_destroy("vertical_bit", vertical)

    --[[
    Extract edges and smooth image according to the logic
    1. extract edges
    2. dilate(edges)
    3. src.copyTo(smooth)
    4. blur smooth img
    5. smooth.copyTo(src, edges)
    --]]

    -- Step 1
    local edges = cv.adaptiveThreshold(vertical, 255, cv.ADAPTIVE_THRESH_MEAN_C,
        cv.THRESH_BINARY, 3, -2)
    show_wait_destroy("edges", edges)

    -- Step 2
    local kernel = cv.Mat.ones(2, 2, cv.CV_8U)
    edges = cv.dilate(edges, kernel)
    show_wait_destroy("dilate", edges)

    -- Step 3
    local smooth = vertical:copy()

    -- Step 4
    smooth = cv.blur(smooth, { 2, 2 })

    -- Step 5
    smooth:copyTo(vertical, edges)

    -- Show final result
    show_wait_destroy("smooth - final", vertical)
    -- [smooth]

    return 0
end

local args = {
    input = "notes.png",
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
