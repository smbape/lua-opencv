local unpack = table.unpack or unpack ---@diagnostic disable-line: deprecated

-- Load cfg first so that the loader knows it is running inside LuaRocks
local cfg = require("luarocks.core.cfg")
local fs = require("luarocks.fs")

local loader = require("luarocks.loader")
local cmd = require("luarocks.cmd")

local description = "LuaRocks main command-line interface"

local commands = {
   new_version = "luarocks.cmd.new_version",
}

local util = require("luarocks.util")
local new_version = require(commands.new_version)
local new_version_command = new_version.command

function new_version.add_to_parser(parser)
   local cmd = parser:command("new_version", [[
This is a utility function that writes a new rockspec, updating data from a
previous one.

If a package name is given, it downloads the latest rockspec from the default
server. If a rockspec is given, it uses it instead. If no argument is given, it
looks for a rockspec same way 'luarocks make' does.

If the version number is not given and tag is passed using --tag, it is used as
the version, with 'v' removed from beginning.  Otherwise, it only increments the
revision number of the given (or downloaded) rockspec.

If a URL is given, it replaces the one from the old rockspec with the given URL.
If a URL is not given and a new version is given, it tries to guess the new URL
by replacing occurrences of the version number in the URL or tag; if the guessed
URL is invalid, the old URL is restored. It also tries to download the new URL
to determine the new MD5 checksum.

If a tag is given, it replaces the one from the old rockspec. If there is an old
tag but no new one passed, it is guessed in the same way URL is.

If a directory is not given, it defaults to the current directory.

WARNING: it writes the new rockspec to the given directory, overwriting the file
if it already exists.]], util.see_also())
       :summary("Auto-write a rockspec for a new version of a rock.")

   cmd:argument("rock", "Package name or rockspec.")
       :args("?")
   cmd:argument("new_version", "New version of the rock.")
       :args("?")
   cmd:argument("abi", "Lua ABI version of the rock.")
       :args("?")

   cmd:option("--dir", "Output directory for the new rockspec.")
   cmd:option("--tag", "New SCM tag.")
   cmd:option("--prefix", "Install prefix.")
   cmd:option("--platform", "OS platform.")
end

---@param directory string
---@param source string
---@param destination table<integer|string, string>
local function recursive_include(directory, source, destination)
   ---@type string[]
   local stack = { source .. "/" .. directory }
   while #stack ~= 0 do
      ---@type string
      local filepath = table.remove(stack)
      if fs.is_dir(filepath) then
         ---@type string[]
         local files = fs.list_dir(filepath)
         for i = #files, 1, -1 do
            stack[#stack + 1] = filepath .. "/" .. files[i]
         end
      elseif fs.is_file(filepath) then
         local module_name = filepath:sub(#source + 2) ---@type string

         local ext = module_name:match("(%..+)$") ---@type string?
         if ext ~= nil then
            module_name = module_name:sub(1, -#ext - 1):gsub("%.", "#"):gsub("/", ".")
            if ext ~= ".lua" then
               module_name = module_name .. ext:gsub("%.", "#")
            end
         end

         destination[module_name] = filepath
      end
   end

end

function new_version.command(args)
   local prefix = args.prefix or "" ---@type string
   local abi = args.abi ---@type string

   if prefix ~= "" and prefix:sub(-1) ~= "/" then
      prefix = prefix .. "/"
   end

   local persist = require("luarocks.persist")
   local load_into_table = persist.load_into_table

   function persist.load_into_table(filename, tbl)
      local out_rs, err, errcode = load_into_table(filename, tbl)
      if out_rs == nil then
         return out_rs, err, errcode
      end

      out_rs.source.url = ""
      out_rs.build.type = "none"
      out_rs.build.variables = nil

      if not out_rs.build.install then
         out_rs.build.install = {}
      end

      local dependencies = out_rs.dependencies
      for k, dependency in pairs(dependencies) do
         if dependency:sub(1, 4) == "lua " then
            dependencies[k] = "lua == " .. abi
         end
      end

      local install_sharedir = prefix .. "share/lua/" .. abi
      local install_libdir = prefix .. "lib/lua/" .. abi
      local shared_library_suffix ---@type string

      if args.platform == "win32" then
         shared_library_suffix = ".dll"
      else
         shared_library_suffix = ".so"
      end

      ---@type table<integer|string, string>
      local install_lib = {
         install_libdir .. "/opencv_lua" .. shared_library_suffix,
      }
      recursive_include("opencv_lua", install_libdir, install_lib)
      out_rs.build.install.lib = install_lib

      ---@type table<integer|string, string>
      local install_lua = {}
      recursive_include("opencv_lua", install_sharedir, install_lua)
      out_rs.build.install.lua = install_lua

      return out_rs, err, errcode
   end

   return new_version_command(args)
end

cmd.run_command(description, commands, "luarocks.cmd.external", "new_version", ...)
