#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

local opencv_lua = require("init")
local cv = opencv_lua.cv

local img = cv.imread(cv.samples.findFile("lena.jpg"))
cv.imshow("Image", img)
cv.waitKey()
cv.destroyAllWindows()
