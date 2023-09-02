package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1,1)) .. package.path

local opencv_lua = require("init")
local cv = opencv_lua.cv

local img = cv.imread(cv.samples.findFile("pic1.png"))
local img_grey = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
local ret, thresh = cv.threshold(img_grey, 100, 255, cv.THRESH_BINARY)
local contours, hierarchy = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

print("Found " .. #contours .. " contours")

cv.drawContours(img, contours, -1, { 0, 0, 255 }, 2)

cv.imshow("Image", img)
cv.waitKey()
cv.destroyAllWindows()
