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
   cmd:flag("--repair", "Relabeling cross-distribution Linux rocks.")
end

local function read_all(file)
   local f = assert(io.open(file, "rb"))
   local content = f:read("*all")
   f:close()
   return content
end

local function write_file(file, content)
   local f = assert(io.open(file, "w"))
   f:write(content)
   f:close()
end

local function interpolate(s, tab)
   return (s:gsub('(@[^@]+@)', function(w) return tab[w:sub(1, -2)] or w end))
end

local function dump_table_as_python_array(tbl)
   local str = "["
   for k, v in pairs(tbl) do
      if #str > 1 then
         str = str .. ", "
      end
      if type(v) == "string" then
         str = str .. "'" .. v .. "'"
      elseif type(v) == "number" then
         str = str .. v
      elseif type(v) == "boolean" then
         if v then
            str = str .. "True"
         else
            str = str .. "False"
         end
      elseif type(v) == nil then
         str = str .. "None"
      end
   end
   str = str .. "]"
   return str
end

function new_version.command(args)
   local prefix = args.prefix or ""
   local abi = args.abi

   if prefix ~= "" and prefix:sub(-1) ~= "/" then
      prefix = prefix .. "/"
   end

   local persist = require("luarocks.persist")
   local load_into_table = persist.load_into_table
   local first_pass = true

   function persist.load_into_table(filename, tbl)
      local out_rs, err, errcode = load_into_table(filename, tbl)
      if out_rs == nil then
         return out_rs, err, errcode
      end

      local name, version = out_rs.package, out_rs.version

      out_rs.source.url = ""
      out_rs.build.type = "none"
      out_rs.build.variables = nil

      local dependencies = out_rs.dependencies
      for k, dependency in pairs(dependencies) do
         if dependency:sub(1, 4) == "lua " then
            dependencies[k] = "lua == " .. abi
         end
      end

      local install_libdir = prefix .. "lib/lua/" .. abi

      if args.platform == "win32" then
         out_rs.build.install = {
            bin = {
               prefix .. "bin/opencv_videoio_ffmpeg4100_64.dll",
            },
            lib = {
               install_libdir .. "/opencv_lua.dll",
            }
         }

         local OpenCV_DIR = os.getenv("OpenCV_DIR")
         if OpenCV_DIR ~= nil then
            local dir_sep = package.config:sub(1, 1)
            local ffmpeg = OpenCV_DIR:gsub("[/\\]", dir_sep) .. dir_sep .. "bin" .. dir_sep .. "opencv_videoio_ffmpeg4100_64.dll"
            print("ffmpeg", ffmpeg)
            if fs.exists(ffmpeg) then
               out_rs.build.install.bin = { ffmpeg }
            end
         end
      else
         out_rs.build.install = {
            lib = {
               install_libdir .. "/opencv_lua.so",
            }
         }

         local package_data = { "opencv_lua.so" }
         local install_lib = out_rs.build.install.lib

         local libqxcb_relpath = "opencv_lua/qt/plugins/platforms/libqxcb.so"
         local libqxcb_fullpath = install_libdir .. "/" .. libqxcb_relpath
         local have_qt = fs.exists(libqxcb_fullpath)
         if have_qt then
            install_lib[libqxcb_relpath:sub(1, -4):gsub("/", ".")] = libqxcb_fullpath -- :sub(1, -4) removes '.so' extenxion to have the corresponding lua module name
            package_data[#package_data + 1] = libqxcb_relpath
         end

         if args.repair then
            -- add repaired libs
            local libs = "opencv_lua.libs"
            local copy_directories = out_rs.build.copy_directories
            copy_directories[#copy_directories + 1] = libs

            if first_pass then
               first_pass = false

               local path = require("luarocks.path")
               local dir = require("luarocks.dir")
               local install_libs = dir.path(path.install_dir(name, args.new_version), libs)
               local install_prefix = fs.current_dir()

               fs.change_dir(prefix .. "../..")
               local ok, err = fs.execute("cmake",
                  "-DENABLE_REPAIR=ON",
                  "-DPACKAGE_DATA=" .. dump_table_as_python_array(package_data),
                  "-DCMAKE_INSTALL_PREFIX=" .. install_prefix,
                  "-DCMAKE_INSTALL_LIBDIR=lib/lua/" .. abi,
                  "-DCMAKE_INSTALL_LIBSDIR=" .. install_libs,
                  "-P", "opencv_lua/audiwheel_repair.cmake"
               )
               fs.pop_dir()

               if not ok then
                  return nil, err
               end
            end
         end

         -- add fonts for Qt5
         if have_qt and args.platform == "linux" then
            local dejavu = install_libdir .. "/opencv_lua/qt/fonts/dejavu"
            local files = fs.list_dir(dejavu)
            for _, filename in ipairs(files or {}) do
               if filename:sub(-4) == ".ttf" then
                  local font_dst = "opencv_lua/qt/fonts/" .. filename
                  local font_src = dejavu .. "/" .. filename
                  install_lib[font_dst:sub(1, -5):gsub("/", ".")] = font_src -- :sub(1, -5) removes '.ttf' extenxion to have the corresponding lua module name
               end
            end
         end
      end

      return out_rs, err, errcode
   end

   return new_version_command(args)
end

cmd.run_command(description, commands, "luarocks.cmd.external", "new_version", ...)
