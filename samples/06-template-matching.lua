local opencv_lua = require(string.gsub(arg[0], "[^/\\]+%.lua", "init"))
local cv = opencv_lua.cv

local image = cv.imread(cv.samples.findFile("mario.png"))
local templ = cv.imread(cv.samples.findFile("mario_coin.png"))

-- The higher the value, the higher the match is exact
local threshold = 0.8

local matches = opencv_lua.findTemplate(image, templ, threshold)
local matchRect = { 0, 0, templ.width, templ.height }

for i=1,#matches do
    matchRect[1] = matches[i][1]
    matchRect[2] = matches[i][2]

    -- Draw a red rectangle around the matched position
    cv.rectangle(image, matchRect, { 0, 0, 255 })
end

cv.imshow("Find template example", image)
cv.waitKey()

cv.destroyAllWindows()
