local path_sep = package.config:sub(1, 1)
local is_windows = path_sep == "\\"
local shared_library_suffix = (function() if is_windows then return ".dll" else return ".so" end end)()
local abi = _VERSION:match("%d+%.%d+")

local package_path = {} ---@type string[]
local package_cpath = {} ---@type string[]

local function add_debug_lua_modules(intall_prefix)
    if intall_prefix == nil then
        return
    end

    package_path[#package_path + 1] = intall_prefix .. "/lua/?.lua"
    package_path[#package_path + 1] = intall_prefix .. "/lua/?/init.lua"

    package_cpath[#package_cpath + 1] = intall_prefix .. "/lib/?" .. shared_library_suffix
    package_cpath[#package_cpath + 1] = intall_prefix .. "/lib/loadall" .. shared_library_suffix
end

local function add_lua_modules(lua_modules)
    if lua_modules == nil then
        return
    end

    package_path[#package_path + 1] = lua_modules .. "/share/lua/" .. abi .. "/?.lua"
    package_path[#package_path + 1] = lua_modules .. "/share/lua/" .. abi .. "/?/init.lua"

    package_cpath[#package_cpath + 1] = lua_modules .. "/lib/lua/" .. abi .. "/?" .. shared_library_suffix
end

local genv = _G or _ENV
local arg = rawget(genv, "arg") ---@type string[]?

if arg ~= nil then
    local lua_executable_arg_index = -1
    while arg[lua_executable_arg_index] ~= nil do
        lua_executable_arg_index = lua_executable_arg_index - 1
    end
    local lua_executable = arg[lua_executable_arg_index + 1]:gsub("[\\/]", '/')

    local _, dend = string.find(lua_executable, "/x64-Debug/", 1, true)
    if dend == nil then
        _, dend = string.find(lua_executable, "/Linux-GCC-Debug/", 1, true)
    end

    if dend ~= nil then
        local OpenCV_DIR = lua_executable:sub(1, dend - 1):gsub("/build/", "/install/", 1)
        add_debug_lua_modules(OpenCV_DIR)
    end
end

if os.getenv("LUA_MPATH") ~= nil then
    for lua_modules in os.getenv("LUA_MPATH"):gmatch("[^;]+") do -- PATH_SEP
        add_lua_modules(lua_modules)
    end
end

add_lua_modules("luarocks/lua_modules")
add_lua_modules("lua_modules")

if is_windows and os.getenv("APPDATA") ~= nil then
    add_lua_modules(os.getenv("APPDATA") .. "/luarocks")
end

package_path[#package_path + 1] = package.path
package_cpath[#package_cpath + 1] = package.cpath

package.path = table.concat(package_path, ";"):gsub("[\\/]", path_sep)
package.cpath = table.concat(package_cpath, ";"):gsub("[\\/]", path_sep)
