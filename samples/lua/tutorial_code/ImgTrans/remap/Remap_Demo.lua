#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.11.0/samples/python/tutorial_code/ImgTrans/remap/Remap_Demo.py
--]]

local argparse = require("argparse")
local opencv_lua = require("init")
local cv = opencv_lua.cv
local INDEX_BASE = 1 -- lua is 1-based indexed

-- [Update]
local function update_map(ind, map_x, map_y, rows, cols)
    for i = 0, rows - 1 do
        for j = 0, cols - 1 do
            if ind == 0 then
                if j > cols * 0.25 and j < cols * 0.75 and i > rows * 0.25 and i < rows * 0.75 then
                    map_x[i + INDEX_BASE][j + INDEX_BASE] = 2 * (j - cols * 0.25) + 0.5
                    map_y[i + INDEX_BASE][j + INDEX_BASE] = 2 * (i - rows * 0.25) + 0.5
                else
                    map_x[i + INDEX_BASE][j + INDEX_BASE] = 0
                    map_y[i + INDEX_BASE][j + INDEX_BASE] = 0
                end
            elseif ind == 1 then
                map_x[i + INDEX_BASE][j + INDEX_BASE] = j
                map_y[i + INDEX_BASE][j + INDEX_BASE] = rows - i
            elseif ind == 2 then
                map_x[i + INDEX_BASE][j + INDEX_BASE] = cols - j
                map_y[i + INDEX_BASE][j + INDEX_BASE] = i
            elseif ind == 3 then
                map_x[i + INDEX_BASE][j + INDEX_BASE] = cols - j
                map_y[i + INDEX_BASE][j + INDEX_BASE] = rows - i
            end
        end
    end
end
-- [Update]

local parser = argparse() {description='Code for Remapping tutorial.'}
parser:option('--input'):description('Path to input image.'):default('chicky_512.png')
local args = parser:parse()

-- [Load]
local src = cv.imread(cv.samples.findFile(args.input), cv.IMREAD_COLOR)
if src:empty() then
    print('Could not open or find the image: ', args.input)
    os.exit(0)
end
-- [Load]

-- [Create]
local map_x_tbl = cv.Mat.zeros(src:size(), cv.CV_32FC1):table()
local map_y_tbl = cv.Mat.zeros(src:size(), cv.CV_32FC1):table()
-- [Create]

-- [Window]
local window_name = 'Remap demo'
cv.namedWindow(window_name)
-- [Window]

-- [Loop]
local ind = 0
while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    local t = os.clock()
    update_map(ind, map_x_tbl, map_y_tbl, src.rows, src.cols)
    local map_x = cv.Mat.createFromArray(map_x_tbl, cv.CV_32FC1)
    local map_y = cv.Mat.createFromArray(map_y_tbl, cv.CV_32FC1)
    print(string.format("Update map took %i seconds: %.3f", ind, os.clock() - t))
    ind = (ind + 1) % 4
    local dst = cv.remap(src, map_x, map_y, cv.INTER_LINEAR)
    cv.imshow(window_name, dst)
    local c = cv.waitKey(1000)
    if c == 27 then
        break
    end
end
-- [Loop]
