package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1,1)) .. package.path

local opencv_lua = require("init")
local cv = opencv_lua.cv

--[[
Sources:
    https://docs.opencv.org/4.x/d4/dc6/tutorial_py_template_matching.html
--]]

local img_rgb = cv.imread(cv.samples.findFile("mario.png"))
assert(img_rgb, "file could not be read, check with os.path.exists()")
local img_gray = cv.cvtColor(img_rgb, cv.COLOR_BGR2GRAY)
local template = cv.imread(cv.samples.findFile("mario_coin.png"), cv.IMREAD_GRAYSCALE)
assert(template, "file could not be read, check with os.path.exists()")

local h, w = unpack(template.shape)

local res = cv.matchTemplate(img_gray,template,cv.TM_CCOEFF_NORMED)
local threshold = 0.8

for j=1,res.rows do
    local y = j - 1

    -- it is faster to make a temp variable
    -- than doing res[y][x] or res:at(y, x)
    local row = res[y]

    for i=1,res.cols do
        local x = i - 1
        if row[x] >= threshold then
            cv.rectangle(img_rgb, { x, y }, { x + w, y + h }, { 0, 0, 255 }, 2)
        end
    end
end

cv.imshow("res.png", img_rgb)
cv.waitKey()

cv.destroyAllWindows()
