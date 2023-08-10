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
    local cap = cv.VideoCapture(cv.samples.findFile("vtest.avi"))
    if not cap:isOpened() then error("!>Error: cannot open the video file.") end

    local frame, read

    while true do
        start = cv.getTickCount()
        read, frame = cap:read()

        if not read then
            io.stderr:write("!>Error: cannot read the camera.\n")
        end

        -- Flip the image horizontally to give the mirror impression
        frame = cv.flip(frame, 1)

        cv.imshow("capture camera", frame)

        key = cv.waitKey(30)
        if key == 0x1b or key == string.byte("q") or key == string.byte("Q") then break end
    end

    cv.destroyAllWindows()
end

example()
