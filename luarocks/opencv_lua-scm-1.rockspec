rockspec_format = "3.0"
package = "opencv_lua"
version = "scm-1"
source = {
   url = "git+ssh://git@gitlab.smbape.com/smbape/lua-opencv.git"
}
description = {
   summary = "OpenCV bindings for lua",
   detailed = [[
      OpenCV bindings for luajit and lua 5.1/5.2/5.3/5.4.
   ]],
   homepage = "https://github.com/smbape/lua-opencv",
   issues_url = "https://github.com/smbape/lua-opencv/issues",
   license = "MIT",
   labels = { "opencv" },
}
dependencies = {
   "lua >= 5.1, < 5.5"
}
build = {
   type = "cmake",
   variables = {
      WITH_LUA_ROCKS = "ON",
      CMAKE_INSTALL_PREFIX = "$(PREFIX)",
      LUA_BINDIR = "$(LUA_BINDIR)",
      LUA_DIR = "$(LUA_DIR)",
      LUA_INCDIR = "$(LUA_INCDIR)",
   },
   copy_directories = {
      "docs",
      "samples"
   },
}
