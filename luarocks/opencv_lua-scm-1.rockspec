rockspec_format = "3.0"
package = "opencv_lua"
version = "scm-1"
source = {
   url = "git+https://github.com/smbape/lua-opencv.git"
}
description = {
   summary = "OpenCV bindings for lua",
   detailed = [[
      OpenCV bindings for luajit and lua 5.1/5.2/5.3/5.4.

      Linux/Windows users:
      I recommend you to try installing the prebuilt binary with
      luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.4 opencv_lua

      Or to specify the target lua version
      luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.4 opencv_lua 4.10.0luajit2.1
      luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.4 opencv_lua 4.10.0lua5.4
      luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.4 opencv_lua 4.10.0lua5.3
      luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.4 opencv_lua 4.10.0lua5.2
      luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.4 opencv_lua 4.10.0lua5.1

      If the prebuilt binary does not work, then I recommend to host your own binary rocks by following the instructions at
      https://github.com/smbape/lua-opencv/tree/v0.0.4#hosting-you-own-binary-rocks

      Otherwise, follow the instructions at https://github.com/smbape/lua-opencv/tree/v0.0.4#prerequisites-to-source-rock-install
      before attempting to install the source rock.
   ]],
   homepage = "https://github.com/smbape/lua-opencv",
   issues_url = "git+https://github.com/smbape/lua-opencv.git",
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
   },
}
