#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

local opencv_lua = require("init")
local cv = opencv_lua.cv

local image = cv.imread(cv.samples.findFile("mario.png"))
local templ = cv.imread(cv.samples.findFile("mario_coin.png"))

-- The higher the value, the higher the match is exact
local threshold = 0.8

local matches = cv.findTemplate(image, templ, threshold)
print("Found " .. #matches .. " matches")

for i = 1, #matches do
    local pt, score = matches[i][1], matches[i][2]
    local x, y = pt[1], pt[2]

    print(x, y, score)

    -- Draw a red rectangle around the matched position
    cv.rectangle(image, { x, y, templ.width, templ.height }, { 0, 0, 255 })
end

cv.imshow("Find template example", image)
cv.waitKey()

cv.destroyAllWindows()
