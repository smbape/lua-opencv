package.path = arg[0]:gsub("[^/\\]+%.lua", '../../../../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/tutorial_code/imgProc/match_template/match_template.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local INDEX_BASE = 1 -- lua is 1-based indexed

-- [global_variables]
local use_mask = false
local img = nil
local templ = nil
local mask = nil
local image_window = "Source Image"
local result_window = "Result window"

local match_method = 0
local max_Trackbar = 5
-- [global_variables]

local function MatchingMethod(param)
    local match_method = param
    local result, matchLoc

    -- [copy_source]
    local img_display = img:copy()
    -- [copy_source]
    -- [match_template]
    local method_accepts_mask = cv.TM_SQDIFF == match_method or match_method == cv.TM_CCORR_NORMED
    if use_mask and method_accepts_mask then
        result = cv.matchTemplate(img, templ, match_method, nil, mask)
    else
        result = cv.matchTemplate(img, templ, match_method)
    end
    -- [match_template]

    -- [normalize]
    cv.normalize(result, result, 0, 1, cv.NORM_MINMAX, -1)
    -- [normalize]
    -- [best_match]
    local _minVal, _maxVal, minLoc, maxLoc = cv.minMaxLoc(result, nil)
    -- [best_match]

    -- [match_loc]
    if (match_method == cv.TM_SQDIFF or match_method == cv.TM_SQDIFF_NORMED) then
        matchLoc = minLoc
    else
        matchLoc = maxLoc
    end
    -- [match_loc]

    -- [imshow]
    cv.rectangle(img_display, matchLoc, { matchLoc[0 + INDEX_BASE] + templ.width, matchLoc[1 + INDEX_BASE] + templ.height }, { 0, 255, 0 }, 2, 8, 0)
    cv.rectangle(result, matchLoc, { matchLoc[0 + INDEX_BASE] + templ.width, matchLoc[1 + INDEX_BASE] + templ.height }, { 0, 0, 0 }, 2, 8, 0)
    cv.imshow(image_window, img_display)
    cv.imshow(result_window, result)
    -- [imshow]
end

local function main(args)
    -- if (#argv < 3) then
    --     print('Not enough parameters')
    --     print('Usage:\nmatch_template_demo.py <image_name> <template_name> [<mask_name>]')
    --     return -1
    -- end

    -- [load_image]
    img = cv.imread(cv.samples.findFile(args.img), cv.IMREAD_COLOR)
    templ = cv.imread(cv.samples.findFile(args.templ), cv.IMREAD_COLOR)

    if args.mask ~= nil and args.mask ~= '' then
        use_mask = true
        mask = cv.imread(cv.samples.findFile(args.mask), cv.IMREAD_COLOR)
    end

    if img:empty() or templ:empty() or use_mask and mask:empty() then
        print('Can\'t read one of the images')
        return -1
    end
    -- [load_image]

    -- [create_windows]
    cv.namedWindow(image_window, cv.WINDOW_AUTOSIZE)
    cv.namedWindow(result_window, cv.WINDOW_AUTOSIZE)
    -- [create_windows]

    -- [create_trackbar]
    local trackbar_label =
    'Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED'
    cv.createTrackbar(trackbar_label, image_window, match_method, max_Trackbar, MatchingMethod)
    -- [create_trackbar]

    MatchingMethod(match_method)

    -- [wait_key]
    while true do
        -- lua is not thread safe
        -- therefore, calling callbacks from another thread
        -- will certainly causes unwanted behaviour
        -- for that reason, callbacks are registered and called from the main thread
        opencv_lua.notifyCallbacks()

        -- Wait until user press some key
        local key = cv.waitKey(1)
        if key ~= -1 then break end
    end
    return 0
    -- [wait_key]
end

local args = {
    img = "lena_tmpl.jpg",
    templ = "tmpl.png",
    mask = "mask.png",
}

for i = 1, #arg, 2 do
    local name = arg[i]
    if name:sub(1, 2) == "--" then name = name:sub(3) end
    if args[name] == nil or i == #arg then
        error('unexpected argument ' .. name)
    end
    args[name] = arg[i + 1]
end

main(args)
