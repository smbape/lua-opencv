package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.8.0/samples/python/tutorial_code/videoio/video-input-psnr-ssim.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local round = opencv_lua.math.round
local INDEX_BASE = 1 -- lua is 1-based indexed

if not math.log10 then
    ---@diagnostic disable-next-line: duplicate-set-field
    math.log10 = function(x) return math.log(x, 10) end
end

-- [get-psnr]
local function getPSNR(I1, I2)
    local s1 = cv.absdiff(I1, I2)   --|I1 - I2|
    s1 = s1:convertTo(cv.CV_32F)    -- cannot make a square on 8 bits

    -- Note that this is not a matrix multiplication that corresponds to a simpler "\*" operator.
    -- Performs an element-wise multiplication or division of the two matrices.
    s1 = s1:mul(s1)                 -- |I1 - I2|^2

    local s = s1:sum()              -- sum elements per channel
    local sse = s[1] + s[2] + s[3]  -- sum channels
    if sse <= 1e-10 then
        return 0                    -- for small values return zero
    else
        local shape = I1.shape
        local mse = 1.0 * sse / (shape[1] * shape[2] * shape[3])
        local psnr = 10.0 * math.log10((255 * 255) / mse)
        return psnr
    end
end
-- [get-psnr]

-- [get-mssim]
local function getMSSISM(i1, i2)
    -- https://github.com/opencv/opencv/issues/16739
    local C1 = cv.Scalar.all(6.5025)
    local C2 = cv.Scalar.all(58.5225)
    -- INITS

    local I1 = i1:convertTo(cv.CV_32F) -- cannot calculate on one byte large values
    local I2 = i2:convertTo(cv.CV_32F)

    local I2_2 = I2:mul(I2)  -- I2^2
    local I1_2 = I1:mul(I1)  -- I1^2
    local I1_I2 = I1:mul(I2) -- I1 * I2
    -- END INITS

    -- PRELIMINARY COMPUTING
    local mu1 = cv.GaussianBlur(I1, { 11, 11 }, 1.5)
    local mu2 = cv.GaussianBlur(I2, { 11, 11 }, 1.5)

    local mu1_2 = mu1:mul(mu1)
    local mu2_2 = mu2:mul(mu2)
    local mu1_mu2 = mu1:mul(mu2)

    local sigma1_2 = cv.GaussianBlur(I1_2, { 11, 11 }, 1.5)
    sigma1_2 = sigma1_2 - mu1_2

    local sigma2_2 = cv.GaussianBlur(I2_2, { 11, 11 }, 1.5)
    sigma2_2 = sigma2_2 - mu2_2

    local sigma12 = cv.GaussianBlur(I1_I2, { 11, 11 }, 1.5)
    sigma12 = sigma12 - mu1_mu2

    local t1 = 2 * mu1_mu2 + C1
    local t2 = 2 * sigma12 + C2
    local t3 = t1:mul(t2) -- t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

    t1 = mu1_2 + mu2_2 + C1
    t2 = sigma1_2 + sigma2_2 + C2
    t1 = t1:mul(t2)                       -- t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

    local ssim_map = cv.divide(t3, t1) -- ssim_map =  t3./t1;

    local mssim = cv.mean(ssim_map)    -- mssim = average of ssim map
    return mssim
end
-- [get-mssim]


local function main()
    -- parser = argparse.ArgumentParser()
    -- parser.add_argument("-d", "--delay", type=int, default=30, help=" Time delay")
    -- parser.add_argument("-v", "--psnrtriggervalue", type=int, default=30, help="PSNR Trigger Value")
    -- parser.add_argument("-r", "--ref", type=str, default="Megamind.avi", help="Path to reference video")
    -- parser.add_argument("-t", "--undertest", type=str, default="Megamind_bugy.avi",
    --                     help="Path to the video to be tested")
    -- args = parser.parse_args()

    local args = {
        delay = 30,
        psnrtriggervalue = 30,
        ref = "Megamind.avi",
        undertest = "Megamind_bugy.avi",
    }

    local aliases = {}
    aliases["-d"] = "delay"
    aliases["-v"] = "psnrtriggervalue"
    aliases["-r"] = "ref"
    aliases["-t"] = "undertest"

    for i = 1, #arg, 2 do
        local name = arg[i]
        if name:sub(1, 2) == "--" then name = name:sub(3) end
        if aliases[name] ~= nil then name = aliases[name] end
        if args[name] == nil or i == #arg then
            error('unexpected argument ' .. name)
        end
        if type(args[name]) == 'number' then
            args[name] = tonumber(arg[i + 1])
        else
            args[name] = arg[i + 1]
        end
    end

    local sourceReference = args.ref
    local sourceCompareWith = args.undertest
    local delay = args.delay
    local psnrTriggerValue = args.psnrtriggervalue

    local framenum = -1 -- Frame counter

    local captRefrnc = cv.VideoCapture(cv.samples.findFileOrKeep(sourceReference))
    local captUndTst = cv.VideoCapture(cv.samples.findFileOrKeep(sourceCompareWith))

    if not captRefrnc:isOpened() then
        print("Could not open the reference " .. sourceReference)
        os.exit(-1)
    end
    if not captUndTst:isOpened() then
        print("Could not open case test " .. sourceCompareWith)
        os.exit(-1)
    end

    local refS = { int(captRefrnc:get(cv.CAP_PROP_FRAME_WIDTH)), int(captRefrnc:get(cv.CAP_PROP_FRAME_HEIGHT)) }
    local uTSi = { int(captUndTst:get(cv.CAP_PROP_FRAME_WIDTH)), int(captUndTst:get(cv.CAP_PROP_FRAME_HEIGHT)) }

    if refS[0 + INDEX_BASE] ~= uTSi[0 + INDEX_BASE] or refS[1 + INDEX_BASE] ~= uTSi[1 + INDEX_BASE] then
        print("Inputs have different size!!! Closing.")
        os.exit(-1)
    end

    local WIN_UT = "Under Test"
    local WIN_RF = "Reference"

    cv.namedWindow(WIN_RF, cv.WINDOW_AUTOSIZE)
    cv.namedWindow(WIN_UT, cv.WINDOW_AUTOSIZE)
    cv.moveWindow(WIN_RF, 400, 0)     --750,  2 (bernat =0)
    cv.moveWindow(WIN_UT, refS[0 + INDEX_BASE], 0) --1500, 2

    print(string.format("Reference frame resolution: Width=%d Height=%d of nr#: %d", refS[0 + INDEX_BASE], refS[1 + INDEX_BASE],
        captRefrnc:get(cv.CAP_PROP_FRAME_COUNT)))
    print(string.format("PSNR trigger value %d", psnrTriggerValue))

    while true do -- Show the image captured in the window and repeat
        -- Without this, memory grows indefinitely
        collectgarbage()

        local _, frameReference = captRefrnc:read()
        local _, frameUnderTest = captUndTst:read()

        if frameReference:empty() or frameUnderTest:empty() then
            print(" < < <  Game over!  > > > ")
            break
        end

        framenum = framenum + 1
        local psnrv = getPSNR(frameReference, frameUnderTest)
        io.write(string.format("Frame: %d# %.3fdB ", framenum, round(psnrv, 3)))

        if psnrv < psnrTriggerValue and psnrv > 0 then
            local mssimv = getMSSISM(frameReference, frameUnderTest)
            io.write(string.format("MSSISM: R %.2f%% G %.2f%% B %.2f%% ", round(mssimv[2 + INDEX_BASE] * 100, 2), round(mssimv[1 + INDEX_BASE] * 100, 2),
                round(mssimv[0 + INDEX_BASE] * 100, 2)))
        end

        print()

        cv.imshow(WIN_RF, frameReference)
        cv.imshow(WIN_UT, frameUnderTest)

        local k = cv.waitKey(delay)
        if k == 27 then
            break
        end
    end

    os.exit(0)
end

main()
