-- Load cfg first so that the loader knows it is running inside LuaRocks
local cfg = require("luarocks.core.cfg")

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
   cmd:argument("abi", "New URL of the rock.")
      :args("?")

   cmd:option("--dir", "Output directory for the new rockspec.")
   cmd:option("--tag", "New SCM tag.")
   cmd:option("--prefix", "Install prefix.")
   cmd:option("--platform", "OS platform.")
end

function new_version.command(args)
   local prefix = args.prefix or ""
   local abi = args.abi

   if prefix and prefix:sub(-1) ~= "/" then
      prefix = prefix .. "/"
   end

   local persist = require("luarocks.persist")
   local load_into_table = persist.load_into_table

   function persist.load_into_table(filename, tbl)
      local out_rs, err, errcode = load_into_table(filename, tbl)

      out_rs.source.url = ""
      out_rs.build.type = "none"
      out_rs.build.variables = nil

      if args.platform == "win32" then
         out_rs.build.install = {
            bin = {
               prefix .. "bin/opencv_videoio_ffmpeg480_64.dll",
               prefix .. "bin/opencv_world480.dll",
            },
            lib = {
               prefix .. "lib/lua/" .. abi .. "/opencv_lua.dll",
            }
         }
      else
         out_rs.build.install = {
            lib = {
               prefix .. "lib/lua/" .. abi .. "/libopencv_world.so",
               prefix .. "lib/lua/" .. abi .. "/opencv_lua.so",
            }
         }
      end

      return out_rs, err, errcode
   end

   return new_version_command(args)
end

cmd.run_command(description, commands, "luarocks.cmd.external", "new_version", ...)
