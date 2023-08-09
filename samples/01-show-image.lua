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
    local img = cv.imread(cv.samples.findFile("lena.jpg"))
    cv.imshow("Image", img)
    cv.waitKey()
    cv.destroyAllWindows()
end

example()
