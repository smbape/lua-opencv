local exports = {}

local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated
local INDEX_BASE = 1 -- lua is 1-based indexed

local shell = require("shell-games")

local sys = {
    argv = rawget(_G, "arg") or {},
}
local __file__ = sys.argv[0]

local opencv_lua = require("init")
local sysPath = opencv_lua.path
local cv = opencv_lua.cv
local int = function(val) return opencv_lua.math.int(tonumber(val)) end
local float = tonumber
local str = tostring

exports.path = sysPath

local function toboolean(str)
    local TRUE = {
        [1] = true,
        ['1'] = true,
        ['t'] = true,
        ['T'] = true,
        ['true'] = true,
        ['True'] = true,
        ['TRUE'] = true,
    };

    return TRUE[str] == true
end
exports.toboolean = toboolean

-- node scripts/func_kwargs.js exports add_argument zoo parser name help '["required", , false]' '["default"]' '["type"]' '["action"]' '["nargs"]' '["alias"]' | clip
local function add_argument(...)
    local args = { n = select("#", ...), ... }
    local has_kwarg = opencv_lua.kwargs.isinstance(args[args.n])
    local kwargs = has_kwarg and args[args.n] or opencv_lua.kwargs()
    local usedkw = 0

    -- get argument zoo
    local zoo
    local has_zoo = false
    if (not has_kwarg) or args.n > 1 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("zoo") then
            error("zoo was both specified as a Positional and NamedParameter")
        end
        has_zoo = args.n >= 1
        if has_zoo then
            zoo = args[1]
        end
    elseif kwargs:has("zoo") then
        -- named parameter
        has_zoo = true
        zoo = kwargs:get("zoo")
        usedkw = usedkw + 1
    else
        error("zoo is mandatory")
    end

    -- get argument parser
    local parser
    local has_parser = false
    if (not has_kwarg) or args.n > 2 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("parser") then
            error("parser was both specified as a Positional and NamedParameter")
        end
        has_parser = args.n >= 2
        if has_parser then
            parser = args[2]
        end
    elseif kwargs:has("parser") then
        -- named parameter
        has_parser = true
        parser = kwargs:get("parser")
        usedkw = usedkw + 1
    else
        error("parser is mandatory")
    end

    -- get argument name
    local name
    local has_name = false
    if (not has_kwarg) or args.n > 3 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("name") then
            error("name was both specified as a Positional and NamedParameter")
        end
        has_name = args.n >= 3
        if has_name then
            name = args[3]
        end
    elseif kwargs:has("name") then
        -- named parameter
        has_name = true
        name = kwargs:get("name")
        usedkw = usedkw + 1
    else
        error("name is mandatory")
    end

    -- get argument help
    local help
    local has_help = false
    if (not has_kwarg) or args.n > 4 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("help") then
            error("help was both specified as a Positional and NamedParameter")
        end
        has_help = args.n >= 4
        if has_help then
            help = args[4]
        end
    elseif kwargs:has("help") then
        -- named parameter
        has_help = true
        help = kwargs:get("help")
        usedkw = usedkw + 1
    else
        error("help is mandatory")
    end

    -- get argument required
    local required = false
    local has_required = false
    if (not has_kwarg) or args.n > 5 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("required") then
            error("required was both specified as a Positional and NamedParameter")
        end
        has_required = args.n >= 5
        if has_required then
            required = args[5]
        end
    elseif kwargs:has("required") then
        -- named parameter
        has_required = true
        required = kwargs:get("required")
        usedkw = usedkw + 1
    end

    -- get argument default
    local default
    local has_default = false
    if (not has_kwarg) or args.n > 6 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("default") then
            error("default was both specified as a Positional and NamedParameter")
        end
        has_default = args.n >= 6
        if has_default then
            default = args[6]
        end
    elseif kwargs:has("default") then
        -- named parameter
        has_default = true
        default = kwargs:get("default")
        usedkw = usedkw + 1
    end

    -- get argument type
    local type
    local has_type = false
    if (not has_kwarg) or args.n > 7 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("type") then
            error("type was both specified as a Positional and NamedParameter")
        end
        has_type = args.n >= 7
        if has_type then
            type = args[7]
        end
    elseif kwargs:has("type") then
        -- named parameter
        has_type = true
        type = kwargs:get("type")
        usedkw = usedkw + 1
    end

    -- get argument action
    local action
    local has_action = false
    if (not has_kwarg) or args.n > 8 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("action") then
            error("action was both specified as a Positional and NamedParameter")
        end
        has_action = args.n >= 8
        if has_action then
            action = args[8]
        end
    elseif kwargs:has("action") then
        -- named parameter
        has_action = true
        action = kwargs:get("action")
        usedkw = usedkw + 1
    end

    -- get argument nargs
    local nargs
    local has_nargs = false
    if (not has_kwarg) or args.n > 9 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("nargs") then
            error("nargs was both specified as a Positional and NamedParameter")
        end
        has_nargs = args.n >= 9
        if has_nargs then
            nargs = args[9]
        end
    elseif kwargs:has("nargs") then
        -- named parameter
        has_nargs = true
        nargs = kwargs:get("nargs")
        usedkw = usedkw + 1
    end

    -- get argument alias
    local alias
    local has_alias = false
    if (not has_kwarg) or args.n > 10 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("alias") then
            error("alias was both specified as a Positional and NamedParameter")
        end
        has_alias = args.n >= 10
        if has_alias then
            alias = args[10]
        end
    elseif kwargs:has("alias") then
        -- named parameter
        has_alias = true
        alias = kwargs:get("alias")
        usedkw = usedkw + 1
    end

    if usedkw ~= kwargs:size() then
        error("there are " .. (kwargs:size() - usedkw) .. " unknown named parameters")
    end

    --- ====================== ---
    --- CODE LOGIC STARTS HERE ---
    --- ====================== ---

    local modelName

    if alias ~= nil then
        modelName = alias
    elseif #sys.argv >= 1 then
        modelName = sys.argv[1]
    end

    local value, prefix

    if sysPath.isfile(zoo) then
        local fs = cv.FileStorage(zoo, cv.FileStorage.READ)
        local node = fs:getNode(modelName)
        if not node:empty() then
            value = node:getNode(name)
            if string.find(name, "sha1") then
                prefix = name:gsub("sha1", "")
                value = node:getNode(prefix .. "load_info")
                if prefix == "config_" then
                    value = value:getNode("sha1")
                else
                    value = value:getNode(name)
                end
            end
            if string.find(name, "download_sha") then
                prefix = name:gsub("download_sha", "")
                value = node:getNode(prefix .. "load_info")
                value = value:getNode(name)
            end
            if not value:empty() then
                if value:isReal() then
                    default = value:real()
                elseif value:isString() then
                    default = value:string()
                elseif value:isInt() then
                    default = int(value:real())
                elseif value:isSeq() then
                    default = {}
                    for i = 0, value:size() - 1, 1 do
                        local v = value:at(i)
                        if v:isInt() then
                            default[#default + 1] = int(v:real())
                        elseif v:isReal() then
                            default[#default + 1] = v:real()
                        else
                            print('Unexpected value format')
                            os.exit(1)
                        end
                    end
                else
                    print('Unexpected field format')
                    os.exit(1)
                end
                required = false
            end
        end
    end

    local option = parser:option('--' .. name)

    option:description(help)

    if action == 'store_true' then
        default = (default == 'true') and 1 or (default == 'false') and 0 or default
        assert(default == nil or default == 0 or default == 1)
        option:default(toboolean(default))
        option:convert(toboolean)
    else
        if default ~= nil then
            option:default(default)
        end

        if type then
            option:convert(type)
        end

        if action ~= nil then
            option:action(action)
        end
    end

    if nargs then
        option:args(nargs)
    elseif required then
        option:args(1)
    end
end
exports.add_argument = add_argument

-- node scripts/func_kwargs.js exports add_preproc_args zoo parser sample '["alias"]' '["prefix",,""]' | clip
local function add_preproc_args(...)
    local args = { n = select("#", ...), ... }
    local has_kwarg = opencv_lua.kwargs.isinstance(args[args.n])
    local kwargs = has_kwarg and args[args.n] or opencv_lua.kwargs()
    local usedkw = 0

    -- get argument zoo
    local zoo
    local has_zoo = false
    if (not has_kwarg) or args.n > 1 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("zoo") then
            error("zoo was both specified as a Positional and NamedParameter")
        end
        has_zoo = args.n >= 1
        if has_zoo then
            zoo = args[1]
        end
    elseif kwargs:has("zoo") then
        -- named parameter
        has_zoo = true
        zoo = kwargs:get("zoo")
        usedkw = usedkw + 1
    else
        error("zoo is mandatory")
    end

    -- get argument parser
    local parser
    local has_parser = false
    if (not has_kwarg) or args.n > 2 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("parser") then
            error("parser was both specified as a Positional and NamedParameter")
        end
        has_parser = args.n >= 2
        if has_parser then
            parser = args[2]
        end
    elseif kwargs:has("parser") then
        -- named parameter
        has_parser = true
        parser = kwargs:get("parser")
        usedkw = usedkw + 1
    else
        error("parser is mandatory")
    end

    -- get argument sample
    local sample
    local has_sample = false
    if (not has_kwarg) or args.n > 3 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("sample") then
            error("sample was both specified as a Positional and NamedParameter")
        end
        has_sample = args.n >= 3
        if has_sample then
            sample = args[3]
        end
    elseif kwargs:has("sample") then
        -- named parameter
        has_sample = true
        sample = kwargs:get("sample")
        usedkw = usedkw + 1
    else
        error("sample is mandatory")
    end

    -- get argument alias
    local alias
    local has_alias = false
    if (not has_kwarg) or args.n > 4 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("alias") then
            error("alias was both specified as a Positional and NamedParameter")
        end
        has_alias = args.n >= 4
        if has_alias then
            alias = args[4]
        end
    elseif kwargs:has("alias") then
        -- named parameter
        has_alias = true
        alias = kwargs:get("alias")
        usedkw = usedkw + 1
    else
        error("alias is mandatory")
    end

    -- get argument prefix
    local prefix = ""
    local has_prefix = false
    if (not has_kwarg) or args.n > 5 then
        -- positional parameter should not be a named parameter
        if has_kwarg and kwargs:has("prefix") then
            error("prefix was both specified as a Positional and NamedParameter")
        end
        has_prefix = args.n >= 5
        if has_prefix then
            prefix = args[5]
        end
    elseif kwargs:has("prefix") then
        -- named parameter
        has_prefix = true
        prefix = kwargs:get("prefix")
        usedkw = usedkw + 1
    end

    if usedkw ~= kwargs:size() then
        error("there are " .. (kwargs:size() - usedkw) .. " unknown named parameters")
    end

    --- ====================== ---
    --- CODE LOGIC STARTS HERE ---
    --- ====================== ---

    local aliases = {}

    if sysPath.isfile(zoo) and prefix == "" then
        local fs = cv.FileStorage(zoo, cv.FileStorage.READ)
        local root = fs:root()
        for i, name in ipairs(root:keys()) do
            local model = root:getNode(name)
            if model:getNode("sample"):string() == sample then
                aliases[#aliases + 1] = name
            end
        end
    end

    if #aliases ~= 0 then
        parser:argument(prefix .. 'alias',
            'An alias name of model to extract preprocessing parameters from models.yml file.')
            :choices(aliases)
    end

    add_argument(zoo, parser, prefix .. 'model', opencv_lua.kwargs({
        required = true,
        help = 'Path to a binary file of model contains trained weights. ' ..
            'It could be a file with extensions .caffemodel (Caffe), ' ..
            '.pb (TensorFlow), .t7 or .net (Torch), .weights (Darknet), .bin (OpenVINO)',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'config',
        opencv_lua.kwargs({
            help = 'Path to a text file of model contains network configuration. ' ..
                'It could be a file with extensions .prototxt (Caffe), .pbtxt or .config (TensorFlow), .cfg (Darknet), .xml (OpenVINO)',
            alias = alias
        }))
    add_argument(zoo, parser, prefix .. 'mean', opencv_lua.kwargs({
        nargs = '+',
        type = float,
        default = { 0, 0, 0 },
        help = 'Preprocess input image by subtracting mean values. ' ..
            'Mean values should be in BGR order.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'std', opencv_lua.kwargs({
        nargs = '+',
        type = float,
        default = { 0, 0, 0 },
        help = 'Preprocess input image by dividing on a standard deviation.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'scale', opencv_lua.kwargs({
        type = float,
        default = 1.0,
        help = 'Preprocess input image by multiplying on a scale factor.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'width', opencv_lua.kwargs({
        type = int,
        help = 'Preprocess input image by resizing to a specific width.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'height', opencv_lua.kwargs({
        type = int,
        help = 'Preprocess input image by resizing to a specific height.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'rgb', opencv_lua.kwargs({
        action = 'store_true',
        help = 'Indicate that model works with RGB input images instead BGR ones.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'labels', opencv_lua.kwargs({
        help = 'Optional path to a text file with names of classes to label detected objects.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'postprocessing', opencv_lua.kwargs({
        type = str,
        help = 'Post-processing kind depends on model topology.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'background_label_id', opencv_lua.kwargs({
        type = int,
        default = -1,
        help = 'An index of background class in predictions. If not negative, exclude such class from list of classes.',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'sha1', opencv_lua.kwargs({
        type = str,
        help = 'Optional path to hashsum of downloaded model to be loaded from models.yml',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'config_sha1', opencv_lua.kwargs({
        type = str,
        help = 'Optional path to hashsum of downloaded config to be loaded from models.yml',
        alias = alias
    }))
    add_argument(zoo, parser, prefix .. 'download_sha', opencv_lua.kwargs({
        type = str,
        help = 'Optional path to hashsum of downloaded model to be loaded from models.yml',
        alias = alias
    }))
end
exports.add_preproc_args = add_preproc_args

local function findModel(filename, sha1)
    if filename then
        if sysPath.exists(filename) then
            return filename
        end

        local fpath = cv.samples.findFile(filename, false)
        if fpath then
            return fpath
        end

        if os.getenv('OPENCV_DOWNLOAD_CACHE_DIR') == nil then
            print('[WARN] Please specify a path to model download directory in OPENCV_DOWNLOAD_CACHE_DIR environment variable.')
            return exports.findFile(filename)
        end

        if sysPath.exists(sysPath.join(os.getenv('OPENCV_DOWNLOAD_CACHE_DIR'), sha1, filename)) then
            return sysPath.join(os.getenv('OPENCV_DOWNLOAD_CACHE_DIR'), sha1, filename)
        end

        if sysPath.exists(sysPath.join(os.getenv('OPENCV_DOWNLOAD_CACHE_DIR'), filename)) then
            return sysPath.join(os.getenv('OPENCV_DOWNLOAD_CACHE_DIR'), filename)
        end
    end

    error('File ' .. filename .. ' not found! Please specify a path to ' ..
            'model download directory in OPENCV_DOWNLOAD_CACHE_DIR ' ..
            'environment variable or pass a full path to ' .. filename)
end
exports.findModel = findModel

local function findFile(filename)
    if not filename or sysPath.exists(filename) then
        return filename
    end

    local fpath = cv.samples.findFile(filename, false)
    if fpath then
        return fpath
    end

    if os.getenv('OPENCV_SAMPLES_DATA_PATH') == nil then
        print('[WARN] Please specify a path to `/samples/data` in OPENCV_SAMPLES_DATA_PATH environment variable.')
        os.exit(0)
    end

    if sysPath.exists(sysPath.join(os.getenv('OPENCV_SAMPLES_DATA_PATH'), filename)) then
        return sysPath.join(os.getenv('OPENCV_SAMPLES_DATA_PATH'), filename)
    end

    for i, path in ipairs({ 'OPENCV_DNN_TEST_DATA_PATH', 'OPENCV_TEST_DATA_PATH', 'OPENCV_SAMPLES_DATA_PATH' }) do
        local extraPath = os.getenv(path)
        local absPath = sysPath.join(extraPath, 'dnn', filename)
        if sysPath.exists(absPath) then
            return absPath
        end
    end

    error(
        'File ' .. filename .. ' not found! Please specify the path to ' ..
        '/opencv/samples/data in the OPENCV_SAMPLES_DATA_PATH environment variable, ' ..
        'or specify the path to opencv_extra/testdata in the OPENCV_DNN_TEST_DATA_PATH environment variable, ' ..
        'or specify the path to the model download cache directory in the OPENCV_DOWNLOAD_CACHE_DIR environment variable, ' ..
        'or pass the full path to ' .. filename .. '.'
    )
end
exports.findFile = findFile

function exports.get_backend_id(backend_name)
    local backend_ids = {
        ["default"] = cv.dnn.DNN_BACKEND_DEFAULT,
        ["openvino"] = cv.dnn.DNN_BACKEND_INFERENCE_ENGINE,
        ["opencv"] = cv.dnn.DNN_BACKEND_OPENCV,
        ["vkcom"] = cv.dnn.DNN_BACKEND_VKCOM,
        ["cuda"] = cv.dnn.DNN_BACKEND_CUDA,
        ["webnn"] = cv.dnn.DNN_BACKEND_WEBNN,
    }

    if backend_ids[backend_name] == nil then
        error("Invalid backend name: " .. backend_name)
    end

    return backend_ids[backend_name]
end

function exports.get_target_id(target_name)
    local target_ids = {
        ["cpu"] = cv.dnn.DNN_TARGET_CPU,
        ["opencl"] = cv.dnn.DNN_TARGET_OPENCL,
        ["opencl_fp16"] = cv.dnn.DNN_TARGET_OPENCL_FP16,
        ["ncs2_vpu"] = cv.dnn.DNN_TARGET_MYRIAD,
        ["hddl_vpu"] = cv.dnn.DNN_TARGET_HDDL,
        ["vulkan"] = cv.dnn.DNN_TARGET_VULKAN,
        ["cuda"] = cv.dnn.DNN_TARGET_CUDA,
        ["cuda_fp16"] = cv.dnn.DNN_TARGET_CUDA_FP16
    }

    if target_ids[target_name] == nil then
        error("Invalid target name: " .. target_name)
    end

    return target_ids[target_name]
end

local function win32_join(args)
    local cmd = {}
    for i, argv in pairs(args) do
        if argv:find(' ') or argv:find('"') then
            cmd[i] = '"' .. argv:gsub('"', '\\"') .. '"'
        else
            cmd[i] = argv
        end
    end
    return '"' .. table.concat(cmd, " ") .. '"'
end

local function execute(args)
    local success, status, code
    if package.config:sub(1, 1) == '\\' then
        -- assume windows
        success, status, code = os.execute(win32_join(args))
    else
        -- assume linux
        success, status, code = os.execute(shell.join(args))
    end

    if _VERSION == 'Lua 5.1' then
        code = success
    elseif success then
        code = 0
    end

    if code > 0xff then
        code = code / 0xff
    end

    return code
end
exports.execute = execute

-- Function to read a file
local function read_file(file_path)
    local file = assert(io.open(file_path, "r"))
    local content = file:read("*all")
    file:close()
    return content
end
exports.read_file = read_file

-- http://lua-users.org/wiki/StringTrim
local match = string.match
local function trim(s)
    return match(s, '^()%s*$') and '' or match(s, '^%s*(.*%S)')
end
exports.trim = trim

-- https://stackoverflow.com/a/24823383
local function slice(tbl, first, last, step)
    local sliced = {}

    for i = first or 1, last or #tbl, step or 1 do
        sliced[#sliced + 1] = tbl[i]
    end

    return sliced
end
exports.slice = slice

local function argmax(_matrix)
    local max_index, max_value, index = nil, nil, 1

    local function find_argmax(matrix)
        for i = 1, #matrix do
            if type(matrix[i]) == 'number' then
                if index == 1 or matrix[i] > max_value then
                    max_value = matrix[i]
                    max_index = index
                end
                index = index + 1
            else
                find_argmax(matrix[i])
            end
        end
    end

    find_argmax(_matrix)

    return max_index
end
exports.argmax = argmax

local function get_dims(matrix)
    local dims = {}

    while (type(matrix) == 'table') do
        dims[#dims + 1] = #matrix
        matrix = matrix[1]
    end

    return dims
end
exports.get_dims = get_dims

local function is_callable(v)
    local typ = type(v)

    if typ == "number" or
        typ == "string" or
        typ == "boolean" or
        typ == "thread" then
        return false
    end

    if typ == "table" or typ == "userdata" then
        local metatable = getmetatable(v)
        return type(metatable) == "table" and type(rawget(metatable, "__call")) == "function"
    end

    return true
end

---@generic T
---@param ctor T
---@param init number|table
---@return T[]
exports.new_table = function(ctor, init)
    local array = {}
    if type(init) == "number" then
        for i = 1, init do
            if is_callable(ctor) then
                array[i] = ctor()
            else
                array[i] = ctor
            end
        end
    else
        for i, initializer_list in ipairs(init) do
            array[i] = ctor(unpack(initializer_list))
        end
    end
    return array
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

return exports
