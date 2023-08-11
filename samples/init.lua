local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

cv.TM_EXACT = -1

-- node generator/func_kwargs.js opencv_lua findTemplate \
--   image \
--   templ \
--   "['threshold', null, 0.95]" \
--   "['methodMatch', null, 'cv.TM_CCOEFF_NORMED']" \
--   mask \
--   "['limit', null, '20']" \
--   "['code', null, '-1']" \
--   "['overlapping', null, '2']" \
--   channels \
--   histSize \
--   ranges \
--  "['methodCompareHist', null, 'cv.HISTCMP_CORREL']" \
--  dstCn \
--  accumulate
function opencv_lua.findTemplate ( ... )
    local args={...}
    local has_kwarg = opencv_lua.kwargs.is_instance(args[#args])
    local kwargs = has_kwarg and args[#args] or opencv_lua.kwargs()
    local usedkw = 0

    -- get argument image
    local image
    local has_image = false
    if (not has_kwarg) or #args > 1 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("image") then
            error('image was both specified as a Positional and NamedParameter')
        end
        has_image = #args >= 1
        if has_image then
            image = args[1]
        end
    elseif kwargs:has("image") then
        -- named parameter
        has_image = true
        image = kwargs:get("image")
        usedkw = usedkw + 1
    elseif true then
        error('image is mandatory')
    end

    -- get argument templ
    local templ
    local has_templ = false
    if (not has_kwarg) or #args > 2 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("templ") then
            error('templ was both specified as a Positional and NamedParameter')
        end
        has_templ = #args >= 2
        if has_templ then
            templ = args[2]
        end
    elseif kwargs:has("templ") then
        -- named parameter
        has_templ = true
        templ = kwargs:get("templ")
        usedkw = usedkw + 1
    elseif true then
        error('templ is mandatory')
    end

    -- get argument threshold
    local threshold = 0.95
    local has_threshold = false
    if (not has_kwarg) or #args > 3 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("threshold") then
            error('threshold was both specified as a Positional and NamedParameter')
        end
        has_threshold = #args >= 3
        if has_threshold then
            threshold = args[3]
        end
    elseif kwargs:has("threshold") then
        -- named parameter
        has_threshold = true
        threshold = kwargs:get("threshold")
        usedkw = usedkw + 1
    elseif false then
        error('threshold is mandatory')
    end

    -- get argument methodMatch
    local methodMatch = cv.TM_CCOEFF_NORMED
    local has_methodMatch = false
    if (not has_kwarg) or #args > 4 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("methodMatch") then
            error('methodMatch was both specified as a Positional and NamedParameter')
        end
        has_methodMatch = #args >= 4
        if has_methodMatch then
            methodMatch = args[4]
        end
    elseif kwargs:has("methodMatch") then
        -- named parameter
        has_methodMatch = true
        methodMatch = kwargs:get("methodMatch")
        usedkw = usedkw + 1
    elseif false then
        error('methodMatch is mandatory')
    end

    -- get argument mask
    local mask
    local has_mask = false
    if (not has_kwarg) or #args > 5 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("mask") then
            error('mask was both specified as a Positional and NamedParameter')
        end
        has_mask = #args >= 5
        if has_mask then
            mask = args[5]
        end
    elseif kwargs:has("mask") then
        -- named parameter
        has_mask = true
        mask = kwargs:get("mask")
        usedkw = usedkw + 1
    elseif false then
        error('mask is mandatory')
    end

    -- get argument limit
    local limit = 20
    local has_limit = false
    if (not has_kwarg) or #args > 6 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("limit") then
            error('limit was both specified as a Positional and NamedParameter')
        end
        has_limit = #args >= 6
        if has_limit then
            limit = args[6]
        end
    elseif kwargs:has("limit") then
        -- named parameter
        has_limit = true
        limit = kwargs:get("limit")
        usedkw = usedkw + 1
    elseif false then
        error('limit is mandatory')
    end

    -- get argument code
    local code = -1
    local has_code = false
    if (not has_kwarg) or #args > 7 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("code") then
            error('code was both specified as a Positional and NamedParameter')
        end
        has_code = #args >= 7
        if has_code then
            code = args[7]
        end
    elseif kwargs:has("code") then
        -- named parameter
        has_code = true
        code = kwargs:get("code")
        usedkw = usedkw + 1
    elseif false then
        error('code is mandatory')
    end

    -- get argument overlapping
    local overlapping = 2
    local has_overlapping = false
    if (not has_kwarg) or #args > 8 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("overlapping") then
            error('overlapping was both specified as a Positional and NamedParameter')
        end
        has_overlapping = #args >= 8
        if has_overlapping then
            overlapping = args[8]
        end
    elseif kwargs:has("overlapping") then
        -- named parameter
        has_overlapping = true
        overlapping = kwargs:get("overlapping")
        usedkw = usedkw + 1
    elseif false then
        error('overlapping is mandatory')
    end

    -- get argument channels
    local channels
    local has_channels = false
    if (not has_kwarg) or #args > 9 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("channels") then
            error('channels was both specified as a Positional and NamedParameter')
        end
        has_channels = #args >= 9
        if has_channels then
            channels = args[9]
        end
    elseif kwargs:has("channels") then
        -- named parameter
        has_channels = true
        channels = kwargs:get("channels")
        usedkw = usedkw + 1
    elseif false then
        error('channels is mandatory')
    end

    -- get argument histSize
    local histSize
    local has_histSize = false
    if (not has_kwarg) or #args > 10 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("histSize") then
            error('histSize was both specified as a Positional and NamedParameter')
        end
        has_histSize = #args >= 10
        if has_histSize then
            histSize = args[10]
        end
    elseif kwargs:has("histSize") then
        -- named parameter
        has_histSize = true
        histSize = kwargs:get("histSize")
        usedkw = usedkw + 1
    elseif false then
        error('histSize is mandatory')
    end

    -- get argument ranges
    local ranges
    local has_ranges = false
    if (not has_kwarg) or #args > 11 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("ranges") then
            error('ranges was both specified as a Positional and NamedParameter')
        end
        has_ranges = #args >= 11
        if has_ranges then
            ranges = args[11]
        end
    elseif kwargs:has("ranges") then
        -- named parameter
        has_ranges = true
        ranges = kwargs:get("ranges")
        usedkw = usedkw + 1
    elseif false then
        error('ranges is mandatory')
    end

    -- get argument methodCompareHist
    local methodCompareHist = cv.HISTCMP_CORREL
    local has_methodCompareHist = false
    if (not has_kwarg) or #args > 12 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("methodCompareHist") then
            error('methodCompareHist was both specified as a Positional and NamedParameter')
        end
        has_methodCompareHist = #args >= 12
        if has_methodCompareHist then
            methodCompareHist = args[12]
        end
    elseif kwargs:has("methodCompareHist") then
        -- named parameter
        has_methodCompareHist = true
        methodCompareHist = kwargs:get("methodCompareHist")
        usedkw = usedkw + 1
    elseif false then
        error('methodCompareHist is mandatory')
    end

    -- get argument dstCn
    local dstCn
    local has_dstCn = false
    if (not has_kwarg) or #args > 13 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("dstCn") then
            error('dstCn was both specified as a Positional and NamedParameter')
        end
        has_dstCn = #args >= 13
        if has_dstCn then
            dstCn = args[13]
        end
    elseif kwargs:has("dstCn") then
        -- named parameter
        has_dstCn = true
        dstCn = kwargs:get("dstCn")
        usedkw = usedkw + 1
    elseif false then
        error('dstCn is mandatory')
    end

    -- get argument accumulate
    local accumulate
    local has_accumulate = false
    if (not has_kwarg) or #args > 14 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("accumulate") then
            error('accumulate was both specified as a Positional and NamedParameter')
        end
        has_accumulate = #args >= 14
        if has_accumulate then
            accumulate = args[14]
        end
    elseif kwargs:has("accumulate") then
        -- named parameter
        has_accumulate = true
        accumulate = kwargs:get("accumulate")
        usedkw = usedkw + 1
    elseif false then
        error('accumulate is mandatory')
    end

    if not (usedkw == kwargs:size()) then
        error('there are ' .. (kwargs:size() - usedkw) .. ' unknown named parameters')
    end

    if not has_channels then
        channels = {}
        for i=1,image:channels() do
            channels[i] = i - 1
        end
    end

    if not has_histSize then
        histSize = {}
        for i=1,image:channels() do
            histSize[i] = methodMatch == cv.TM_EXACT and i - 1 or 32
        end
    end

    if not has_ranges then
        ranges = {}
        for i=1,image:channels() do
            ranges[2 * i] = methodMatch == cv.TM_EXACT and -(1 - threshold) * 256 or 0
            ranges[2 * i + 1] = methodMatch == cv.TM_EXACT and -(1 - threshold) * 256 or 0
        end
    end

    local tresult = {}

    if limit < 0 then limit = 0 end
    if limit == 0 then return tresult end

    local width = image.width
    local height = image.height

    local w = templ.width
    local h = templ.height

    local matchRect = { 0, 0, w, h }

    local rw = width - w + 1
    local rh = height - h + 1

    if rw <= 0 or rh <= 0 then return tresult end

    local methodAcceptsMask = methodMatch == cv.TM_EXACT or cv.TM_SQDIFF == methodMatch or methodMatch == cv.TM_CCORR_NORMED
    local isNormed = methodMatch == cv.TM_EXACT or methodMatch == cv.TM_SQDIFF_NORMED or methodMatch == cv.TM_CCORR_NORMED or methodMatch == cv.TM_CCOEFF_NORMED

    if not methodAcceptsMask then has_mask = false end

    local result = image.new()

    kwargs = {}
    if has_mask then kwargs["mask"] = mask end

    cv.matchTemplate(image, templ, methodMatch, result, opencv_lua.kwargs(kwargs))

    local matchLoc
    local histScore = 1
    local score = 0
    local found = 0

    if not isNormed then
        cv.normalize(result, result, 0, 1, cv.NORM_MINMAX)
    end

    -- there are rh rows and rw cols in the result matrix
    -- create a mask with the same number of rows and cols
    local resultMask = image.ones(rh, rw, cv.CV_8UC1)
    local minVal, maxVal, minLoc, maxLoc

    while limit > 0 do
        limit = limit - 1

        minVal, maxVal, minLoc, maxLoc = cv.minMaxLoc(result, resultMask)

        -- For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
        if methodMatch == cv.TM_SQDIFF or methodMatch == cv.TM_SQDIFF_NORMED then
            matchLoc = minLoc
            score = 1 - minVal
        else
            matchLoc = maxLoc
            score = maxVal
        end

        if (not isNormed) and (found == 0) then
            matchRect[1] = matchLoc[1]
            matchRect[2] = matchLoc[2]

            local imgMatch = image.new(image, matchRect)
            if has_mask and not (mask.channels() == 1) then has_mask = false end
            -- histScore = opencv_lua.compareMatHist(imgMatch, templ, mask, channels, histSize, ranges, methodCompareHist, accumulate)
        end

        score = score * histScore

        if score < threshold then
            break
        end

        found = found + 1
        tresult[found] = { matchLoc[1], matchLoc[2], score }

        local tw = math.max(1, math.ceil(overlapping * w))
        local th = math.max(1, math.ceil(overlapping * h))
        local x = math.max(0, matchLoc[1] - tw / 2)
        local y = math.max(0, matchLoc[2] - th / 2)

        -- ensure that the template stays within the mask
        if x + tw > rw then tw = rw - x end
        if y + th > rh then th = rh - y end

        -- mask the locations that should not be matched again
        local maskedRect = image.new(resultMask, { x, y, tw, th })
        maskedRect:setTo(0.0)
    end

    return tresult
end

local sysPath = {
    dirname = function (file)
        local index;

        for i=#file,1,-1 do
            local c = file:sub(i,i)
            if c == "/" or c == "\\" then
                if i == 1 then return c end
                return file:sub(1, i - 1)
            end
        end

        if #file == 0 or (file:find(":") == nil and not (file:sub(1,1) == "/")) then
            return "."
        end

        return file
    end
}

opencv_lua.path = sysPath

local OPENCV_SAMPLES_DATA_PATH = sysPath.dirname(opencv_lua.fs_utils.findFile("samples/data/lena.jpg", opencv_lua.kwargs({
    hints={
        ".",
        "out/build/x64-Debug/opencv/opencv-src",
        "out/build/x64-Release/opencv/opencv-src",
        "out/build/Linux-GCC-Debug/opencv/opencv-src",
        "out/build/Linux-GCC-Release/opencv/opencv-src",
    }
})))
cv.samples.addSamplesDataSearchPath(OPENCV_SAMPLES_DATA_PATH)

local LOCAL_SAMPLES_DATA_PATH = opencv_lua.fs_utils.findFile("samples/data")
cv.samples.addSamplesDataSearchPath(LOCAL_SAMPLES_DATA_PATH)

return opencv_lua
