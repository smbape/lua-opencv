local opencv_lua = require(string.gsub(arg[0], "[^/\\]+%.lua", "init"))
local cv = opencv_lua.cv

local cap = cv.VideoCapture(cv.samples.findFile("vtest.avi"))
if not cap:isOpened() then error("!>Error: cannot open the video file.") end

local frame, read

while true do
    read, frame = cap:read()

    if not read then
        io.stderr:write("!>Error: cannot read the camera.\n")
    end

    -- Flip the image horizontally to give the mirror impression
    frame = cv.flip(frame, 1)

    cv.imshow("capture camera", frame)

    local key = cv.waitKey(30)
    if key == 0x1b or key == string.byte("q") or key == string.byte("Q") then break end
end

cv.destroyAllWindows()
