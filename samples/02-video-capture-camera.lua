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
    local camId = 0
    local cap = cv.VideoCapture(camId)
    if not cap:isOpened() then error("!>Error: cannot open the camera " .. camId) end

    local CAP_FPS = 60
    local CAP_SPF = math.floor(1000 / CAP_FPS)

    cap:set(cv.CAP_PROP_FRAME_WIDTH, 1280)
    cap:set(cv.CAP_PROP_FRAME_HEIGHT, 720)
    cap:set(cv.CAP_PROP_FPS, CAP_FPS)

    local frame = cv.Mat.zeros(1280, 720, cv.CV_8UC3)
    local start, fps, key

    while true do
        start = cv.getTickCount()
        if cap:read(frame) then
            -- Flip the image horizontally to give the mirror impression
            frame = cv.flip(frame, 1)
        else
            io.stderr:write("!>Error: cannot read the camera.\n")
        end
        fps = cv.getTickFrequency() / (cv.getTickCount() - start)

        cv.putText(frame, string.format("FPS : %.2f", fps), { 10, 30 }, cv.FONT_HERSHEY_PLAIN, 2, {255, 0, 255}, 3)
        cv.imshow("capture camera", frame)

        key = cv.waitKey(CAP_SPF)
        if key == 0x1b or key == string.byte("Q") then break end
    end

    cv.destroyAllWindows()
end

example()
