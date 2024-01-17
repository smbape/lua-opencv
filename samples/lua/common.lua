--[[
Sources:
    https://github.com/opencv/opencv/blob/4.9.0/samples/python/common.py
--]]

local module = {}

local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local INDEX_BASE = 1 -- lua is 1-based indexed
local inspect = require("inspect")

function module.clock()
    return cv.getTickCount() / cv.getTickFrequency()
end

function module.mosaic(...)
    local args = { ... }
    local has_kwarg = opencv_lua.kwargs.is_instance(args[#args])
    local kwargs = has_kwarg and args[#args] or opencv_lua.kwargs()
    local usedkw = 0

    -- get argument w
    local w
    local has_w = false
    if (not has_kwarg) or #args > 1 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("w") then
            error('w was both specified as a Positional and NamedParameter')
        end
        has_w = #args >= 1
        if has_w then
            w = args[1]
        end
    elseif kwargs:has("w") then
        -- named parameter
        has_w = true
        w = kwargs:get("w")
        usedkw = usedkw + 1
    else
        error('w is mandatory')
    end

    -- get argument imgs
    local imgs
    local has_imgs = false
    if (not has_kwarg) or #args > 2 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("imgs") then
            error('imgs was both specified as a Positional and NamedParameter')
        end
        has_imgs = #args >= 2
        if has_imgs then
            imgs = args[2]
        end
    elseif kwargs:has("imgs") then
        -- named parameter
        has_imgs = true
        imgs = kwargs:get("imgs")
        usedkw = usedkw + 1
    else
        error('imgs is mandatory')
    end

    if usedkw ~= kwargs:size() then
        error('there are ' .. (kwargs:size() - usedkw) .. ' unknown named parameters')
    end

    --- ====================== ---
    --- CODE LOGIC STARTS HERE ---
    --- ====================== ---
    --[[Make a grid from images.

    w    -- number of grid columns
    imgs -- images (must have the same size and format)
    --]]

    if #imgs == 0 then
        return cv.Mat()
    end

    local t = os.clock()
    local img0 = imgs[1]
    local width, height = img0.width, img0.height

    local mat_width = width * w
    local mat_height = height * math.ceil(#imgs / w)
    local grid = cv.Mat.zeros({mat_width, mat_height}, img0:type())

    for i = 1, #imgs do
        local col = (i - INDEX_BASE) % w
        local row = int((i - INDEX_BASE) / w)
        local location_on_grid = grid:new({ width * col, height * row, width, height })
        imgs[i]:copyTo(location_on_grid)
    end
    print(string.format("mosaic(%d, %d) took %.3f seconds", w, #imgs, os.clock() - t))

    return grid
end

-- http://lua-users.org/wiki/ObjectOrientationTutorial
function module.class(cls, base)
    -- failed table lookups on the instances should fallback to the class table
    cls.__index = cls

    cls.new = function(...)
        local self = setmetatable({}, cls)
        cls.__init__(self, ...)
        return self
    end

    local metatable = {
        __call = function(cls, ...)
            return cls.new(...)
        end,
    }

    -- this is what makes the inheritance work
    if base then
        cls.__super__ = base
        metatable.__index = base
    end

    setmetatable(cls, metatable)

    return cls
end

return module
