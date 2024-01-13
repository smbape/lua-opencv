#!/usr/bin/env lua

package.path = arg[0]:gsub("[^/\\]+%.lua", '../?.lua;'):gsub('/', package.config:sub(1, 1)) .. package.path

--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/coherence.py
--]]

local opencv_lua = require("init")
local cv = opencv_lua.cv
local INDEX_BASE = 1 -- lua is 1-based indexed

local __doc__ = [[
Coherence-enhancing filtering example
=====================================

inspired by
  Joachim Weickert "Coherence-Enhancing Shock Filters"
  http://www.mia.uni-saarland.de/Publications/weickert-dagm03.pdf
]]

local function coherence_filter(...)
    local args = { ... }
    local has_kwarg = opencv_lua.kwargs.is_instance(args[#args])
    local kwargs = has_kwarg and args[#args] or opencv_lua.kwargs()
    local usedkw = 0

    -- get argument img
    local img
    local has_img = false
    if (not has_kwarg) or #args > 1 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("img") then
            error('img was both specified as a Positional and NamedParameter')
        end
        has_img = #args >= 1
        if has_img then
            img = args[1]
        end
    elseif kwargs:has("img") then
        -- named parameter
        has_img = true
        img = kwargs:get("img")
        usedkw = usedkw + 1
    else
        error('img is mandatory')
    end

    -- get argument sigma
    local sigma = 11
    local has_sigma = false
    if (not has_kwarg) or #args > 2 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("sigma") then
            error('sigma was both specified as a Positional and NamedParameter')
        end
        has_sigma = #args >= 2
        if has_sigma then
            sigma = args[2]
        end
    elseif kwargs:has("sigma") then
        -- named parameter
        has_sigma = true
        sigma = kwargs:get("sigma")
        usedkw = usedkw + 1
    end

    -- get argument str_sigma
    local str_sigma = 11
    local has_str_sigma = false
    if (not has_kwarg) or #args > 3 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("str_sigma") then
            error('str_sigma was both specified as a Positional and NamedParameter')
        end
        has_str_sigma = #args >= 3
        if has_str_sigma then
            str_sigma = args[3]
        end
    elseif kwargs:has("str_sigma") then
        -- named parameter
        has_str_sigma = true
        str_sigma = kwargs:get("str_sigma")
        usedkw = usedkw + 1
    end

    -- get argument blend
    local blend = 0.5
    local has_blend = false
    if (not has_kwarg) or #args > 4 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("blend") then
            error('blend was both specified as a Positional and NamedParameter')
        end
        has_blend = #args >= 4
        if has_blend then
            blend = args[4]
        end
    elseif kwargs:has("blend") then
        -- named parameter
        has_blend = true
        blend = kwargs:get("blend")
        usedkw = usedkw + 1
    end

    -- get argument iter_n
    local iter_n = 4
    local has_iter_n = false
    if (not has_kwarg) or #args > 5 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("iter_n") then
            error('iter_n was both specified as a Positional and NamedParameter')
        end
        has_iter_n = #args >= 5
        if has_iter_n then
            iter_n = args[5]
        end
    elseif kwargs:has("iter_n") then
        -- named parameter
        has_iter_n = true
        iter_n = kwargs:get("iter_n")
        usedkw = usedkw + 1
    end

    if usedkw ~= kwargs:size() then
        error('there are ' .. (kwargs:size() - usedkw) .. ' unknown named parameters')
    end

    -- ... YOUR CODE HERE
    local h, w = img.height, img.width

    for i = 1, iter_n do
        print(i)

        local gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
        local eigen = cv.cornerEigenValsAndVecs(gray, str_sigma, 3)
        eigen = eigen:reshape(1, { h, w, 3, 2 }) -- [[e1, e2], v1, v2]

        -- Reshaping of n-dimensional non-continuous matrices is not supported yet
        -- copy() will create a continuous n-dimensional matrix
        local x = eigen:new({
            cv.Range(0, h),
            cv.Range(0, w),
            cv.Range(1, 2),
            cv.Range(0, 1),
        }):copy():reshape(1, { h, w })

        -- Reshaping of n-dimensional non-continuous matrices is not supported yet
        -- copy() will create a continuous n-dimensional matrix
        local y = eigen:new({
            cv.Range(0, h),
            cv.Range(0, w),
            cv.Range(1, 2),
            cv.Range(1, 2),
        }):copy():reshape(1, { h, w })

        local gxx = cv.Sobel(gray, cv.CV_32F, 2, 0, opencv_lua.kwargs({ ksize = sigma }))
        local gxy = cv.Sobel(gray, cv.CV_32F, 1, 1, opencv_lua.kwargs({ ksize = sigma }))
        local gyy = cv.Sobel(gray, cv.CV_32F, 0, 2, opencv_lua.kwargs({ ksize = sigma }))
        local gvv = x * x * gxx + 2 * x * y * gxy + y * y * gyy
        local m = cv.compare(gvv, 0, cv.CMP_LT)

        local ero = cv.erode(img, nil)
        local dil = cv.dilate(img, nil)
        local img1 = ero

        dil:copyTo(opencv_lua.kwargs({ dst = img1, mask = m }))
        img = (img * (1.0 - blend) + img1 * blend):convertTo(cv.CV_8UC3)
    end
    print('done')
    return img
end

local function main()
    local fn = arg[1] or 'baboon.jpg'

    local src = cv.imread(cv.samples.findFile(fn))

    local function nothing()
        -- Nothing to do
    end

    local function update()
        local sigma = cv.getTrackbarPos('sigma', 'control') * 2 + 1
        local str_sigma = cv.getTrackbarPos('str_sigma', 'control') * 2 + 1
        local blend = cv.getTrackbarPos('blend', 'control') / 10.0
        print(string.format('sigma: %d  str_sigma: %d  blend_coef: %f', sigma, str_sigma, blend))
        local dst = coherence_filter(src, opencv_lua.kwargs({ sigma = sigma, str_sigma = str_sigma, blend = blend }))
        cv.imshow('dst', dst)
    end

    cv.namedWindow('control', 0)
    cv.createTrackbar('sigma', 'control', 9, 15, nothing)
    cv.createTrackbar('blend', 'control', 7, 10, nothing)
    cv.createTrackbar('str_sigma', 'control', 9, 15, nothing)


    print('Press SPACE to update the image\n')

    cv.imshow('src', src)
    update()

    while true do
        -- Without this, memory grows indefinitely
        collectgarbage()

        -- lua is not thread safe
        -- therefore, calling callbacks from another thread
        -- will certainly causes unwanted behaviour
        -- for that reason, callbacks are registered and called from the main thread
        opencv_lua.notifyCallbacks()

        local ch = cv.waitKey()
        if ch == string.byte(' ') then
            update()
        elseif ch == 27 then
            break
        end
    end

    print('Done')
end

print(__doc__)
main()
cv.destroyAllWindows()
