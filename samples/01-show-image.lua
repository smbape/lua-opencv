local opencv_lua = require(string.gsub(arg[0], "[^/\\]+%.lua", "init"))
local cv = opencv_lua.cv

local img = cv.imread(cv.samples.findFile("lena.jpg"))
cv.imshow("Image", img)
cv.waitKey()
cv.destroyAllWindows()
