package.path = arg[0]:gsub("[^/\\]+%.lua", '?.lua;'):gsub('/', package.config:sub(1,1)) .. package.path

local opencv_lua = require("init")
local cv = opencv_lua.cv

local cap = cv.VideoCapture(cv.samples.findFile("vtest.avi"))
if not cap:isOpened() then error("!>Error: cannot open the video file.") end

while true do
    -- Without this, memory grows indefenitely
    collectgarbage()

    local read, frame = cap:read()

    if not read then
        io.stderr:write("!>Error: cannot read the camera.\n")
    end

    -- Flip the image horizontally to give the mirror impression
    local frame = cv.flip(frame, 1)

    cv.imshow("capture camera", frame)

    local key = cv.waitKey(30)
    if key == 0x1b or key == string.byte("q") or key == string.byte("Q") then break end
end

cv.destroyAllWindows()
