opencv_lua = require("opencv_lua")
cv = opencv_lua.cv
kwargs = opencv_lua.kwargs
fs_utils = opencv_lua.fs_utils

OPENCV_SAMPLES_DATA_PATH = fs_utils.findFile("samples/data", kwargs({
    hints={
        ".",
        "out/build/x64-Debug/opencv/opencv-src",
        "out/build/x64-Release/opencv/opencv-src",
        "out/build/Linux-GCC-Debug/opencv/opencv-src",
        "out/build/Linux-GCC-Release/opencv/opencv-src",
    }
}))

cv.samples.addSamplesDataSearchPath(OPENCV_SAMPLES_DATA_PATH)

function example()
    local img = cv.imread(cv.samples.findFile("pic1.png"))
    local img_grey = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
    local ret, thresh = cv.threshold(img_grey, 100, 255, cv.THRESH_BINARY)
    local contours, hierarchy = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

    print("Found " .. #contours .. " contours")

    cv.drawContours(img, contours, -1, { 0, 0, 255 }, 2)

    cv.imshow("Image", img)
    cv.waitKey()
    cv.destroyAllWindows()
end

example()
