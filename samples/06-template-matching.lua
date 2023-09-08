package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

local opencv_lua = require("init")
local cv = opencv_lua.cv

--[[
Sources:
    https://docs.opencv.org/4.x/d4/dc6/tutorial_py_template_matching.html
--]]

local img_rgb = cv.imread(cv.samples.findFile("mario.png"))
assert(not img_rgb:empty(), "file could not be read, check with os.path.exists()")
local img_gray = cv.cvtColor(img_rgb, cv.COLOR_BGR2GRAY)
local template = cv.imread(cv.samples.findFile("mario_coin.png"), cv.IMREAD_GRAYSCALE)
assert(not template:empty(), "file could not be read, check with os.path.exists()")

local h, w = template.height, template.width

local res = cv.matchTemplate(img_gray, template, cv.TM_CCOEFF_NORMED)
local threshold = 0.8

-- transform into an lua table for faster processing in lua
local rows, cols = res.rows, res.cols
res = res:table()

for j = 1, rows do
    local y = j - 1
    for i = 1, cols do
        local x = i - 1
        if res[j][i] >= threshold then
            cv.rectangle(img_rgb, { x, y }, { x + w, y + h }, { 0, 0, 255 }, 2)
        end
    end
end

cv.imshow("res.png", img_rgb)
cv.waitKey()
cv.destroyAllWindows()
