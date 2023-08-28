-- http://lua-users.org/lists/lua-l/2019-08/msg00132.html
package = "lua-opencv"
version = "dev-1"
source = {
   url = "git+ssh://git@gitlab.smbape.com/smbape/lua-opencv.git"
}
description = {
   summary = "OpenCV bindings for lua",
   detailed = [[
      OpenCV bindings for luajit and lua 5.1/5.2/5.3/5.4.
   ]],
   homepage = "https://github.com/smbape/lua-opencv",
   license = "MIT"
}
dependencies = {
   "lua >= 5.1"
}
build = {
   type = "cmake",
   variables = {
      CMAKE_INSTALL_PREFIX = "$(PREFIX)",
      BUILD_SHARED_LIBS = "ON",
      LUA_BINDIR = "$(LUA_BINDIR)",
      LUA_DIR = "$(LUA_DIR)",
      LUA_INCDIR = "$(LUA_INCDIR)",
   },
   copy_directories = {
      "docs",
      "samples"
   },
}
