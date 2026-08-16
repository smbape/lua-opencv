--[[
Sources:
    https://github.com/opencv/opencv/blob/4.14.0/samples/python/common.py
--]]

local exports = {}

local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv
local int = opencv_lua.math.int
local INDEX_BASE = 1 -- lua is 1-based indexed
-- local inspect = require("inspect")

function exports.clock()
    return cv.getTickCount() / cv.getTickFrequency()
end

-- Function to read a file
function exports.read_file(file_path)
    local file = assert(io.open(file_path, "r"))
    local content = file:read("*all")
    file:close()
    return content
end

-- look up for `k' in list of tables `parents'
local function metatables__index(parents, k)
    for i = 1, #parents do
        local v = parents[i][k] -- try `i'-th superclass
        if v then return v end
    end
end

local function __instanceof(self, constructor)
    local stack = { getmetatable(self) }
    while #stack ~= 0 do
        local mt = table.remove(stack)
        if mt == constructor then return true end
        if mt and mt.____parents__ then
            local classes = mt.____parents__
            for i = #classes, 1, -1 do
                stack[#stack + 1] = classes[i]
            end
        end
    end
    return false
end

local function default_destroy(self)
    -- nothing to do
end

local function default__tostring(self)
    local mt = getmetatable(self)
    setmetatable(self, nil)
    local str = tostring(self)
    setmetatable(self, mt)

    -- if type(self) == "table" then
    --     str = inspect(self)
    -- end

    local name = self.__name
    if name then
        str = string.format("class<%s>: %s", name, str)
    end

    return str
end

local function default__call(cls, ...)
    return cls.new(...)
end

-- http://lua-users.org/wiki/ObjectOrientationTutorial
-- https://www.lua.org/pil/16.3.html
function exports.class(cls, ...)
    local argc = select("#", ...)
    local parents = { ... }

    -- failed table lookups on the instances should fallback to the class table
    if not cls.__index then
        cls.__index = cls
    end

    cls.new = function(...)
        local self = setmetatable({}, cls)

        local __init__ = cls.__init__
        if __init__ then __init__(self, ...) end

        local __name = cls.__name
        if type(__name) == "string" and cls[__name] then cls[__name](self, ...) end

        return self
    end

    cls.__instanceof = __instanceof

    if not cls.__destroy then
        cls.__destroy = default_destroy
    end

    if not cls.__tostring then
        cls.__tostring = default__tostring
    end

    local metatable = {
        __call = default__call,
    }

    -- this is what makes the inheritance work
    -- class will search for each method in the list of its
    -- parents (`arg' is the list of parents)
    if argc > 0 then
        cls.__super__ = setmetatable({}, {
            __index = function(self, k)
                return metatables__index(parents, k)
            end
        })
        cls.__parents__ = parents
        metatable.__index = function(self, k)
            return metatables__index(parents, k)
        end
    end

    setmetatable(cls, metatable)

    return cls
end

--[[ Module for measuring FPS for Allegro games.
    Written by Miran Amon.
    version: 1.02
    date: 27. June 2003
--]]

--[[ Underlaying structure for measuring FPS. The user should create an instance
    of this structure with create_fps() and destroy it with destroy_fps().
    The user should not use the contents of the structure directly,
    functions for manipulating with the FPS structure should be used instead.
--]]
---@class FPS
---@field samplesTimerDiff? number[]
---@field nSamples integer
---@field curSample integer
---@field timerInit number
---@field frames integer
---@field seconds number
---@overload fun(nSamples?: integer): FPS
local FPS = exports.class({
    __name = "FPS",
})
exports.FPS = FPS

--[[
Sources:
     https://github.com/liballeg/allegro5/blob/5.2.11.0/demos/skater/src/fps.c
--]]

--[[ Creates an instance of the FPS structure and initializes it. The user
    should call this function somewhere at the beginning of the program and
    use the value it returns to access the current FPS.

    Parameters:
        int fps - the frequency of the timer used to control the speed of the program

    Returns:
        FPS *fps - an instance of the FPS structure
--]]
---@param self FPS
---@param nSamples? integer
function FPS.FPS(self, nSamples)
    if nSamples == nil or nSamples <= 0 then nSamples = 100 end
    self.nSamples = nSamples
    self:reset()
end

---@param self FPS
function FPS.reset(self)
    self.samplesTimerDiff = exports.new_table(0, self.nSamples)
    self.curSample = 0
    self.timerInit = cv.getTickCount()
    self.frames = 0
    self.seconds = 0
end

--[[ Counts the number of drawn frames. The user should call this function
    every time they draw their frame.

    Parameters:
        FPS *fps - a pointer to an FPS object

    Returns:
        nothing
--]]
---@param self FPS
function FPS.frame(self)
    local timerDiff = cv.getTickCount() - self.timerInit
    self.curSample = (self.curSample + 1) % self.nSamples
    self.frames = math.min(self.frames + 1, self.nSamples)
    self.seconds = self.seconds - self.samplesTimerDiff[self.curSample + INDEX_BASE] + timerDiff
    self.samplesTimerDiff[self.curSample + INDEX_BASE] = timerDiff
    self.timerInit = cv.getTickCount()
end

--[[ Retreives the current frame rate in frames per second. This will actually
    be the average frame rate over the last second.

    Parameters:
        FPS *fps - a pointer to an FPS object

    Returns:
        int fps - the average frame rate over the last second
--]]
---@param self FPS
---@return number fps
function FPS.compute(self)
    if self.seconds == 0 then return 0 end
    return self.frames / self.seconds * cv.getTickFrequency()
end

function exports.mosaic(...)
    local args = { ... }
    local has_kwarg = opencv_lua.kwargs.isinstance(args[#args])
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

return exports
