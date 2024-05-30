# Hosting you own binary rocks on Windows

## Table Of Contents

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [Prepare](#prepare)
- [Prerequisites](#prerequisites)
- [Build System Environment](#build-system-environment)
- [Install freetype and harfbuzz with vcpkg](#install-freetype-and-harfbuzz-with-vcpkg)
- [Build](#build)
  - [Open GIT BASH in the **build directory**](#open-git-bash-in-the-build-directory)
  - [Download the source code](#download-the-source-code)
  - [Create the **opencv_lua-contrib-custom** rockspec](#create-the-opencv_lua-contrib-custom-rockspec)
    - [Change the package name to **opencv_lua-contrib-custom**](#change-the-package-name-to-opencv_lua-contrib-custom)
    - [Add our custom build variable](#add-our-custom-build-variable)
  - [Make the **opencv_lua-contrib-custom**-scm-1.rockspec rockspec](#make-the-opencv_lua-contrib-custom-scm-1rockspec-rockspec)
- [Pack the prebuilt binary on the **server directory**](#pack-the-prebuilt-binary-on-the-server-directory)
- [Testing our custom prebuilt binary](#testing-our-custom-prebuilt-binary)
  - [Prepare](#prepare-1)
  - [Initialize our test project and install our custom prebuilt binary](#initialize-our-test-project-and-install-our-custom-prebuilt-binary)
  - [Create a file with an UTF-16 name](#create-a-file-with-an-utf-16-name)
  - [Execute a test script](#execute-a-test-script)
- [Hosting on a web server](#hosting-on-a-web-server)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Here we will build a custom opencv with the folling modifications:
  - Add the contrib modules
  - Add the freetype module.
  - Enable experimental support of UTF-16 (wide character) strings on Windows.

The procedure has been tested on Windows 11.

## Prepare

  - Install [CMake >= 3.25](https://cmake.org/download/)
  - Install [Git](https://git-scm.com/)
  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install [Python](https://www.python.org/downloads/)
  - Install [Visual Studio 2022 >= 17.7.2](https://visualstudio.microsoft.com/fr/downloads/)

In your windows search, search and open the `x64 Native Tools Command Prompt for VS 2022`

From here on, any command will be executed within the opened Command Prompt.

## Prerequisites

I assume you already have an [Lua](https://www.lua.org/start.html) or [LuaJIT](https://luajit.org/) installation.
I assume you have already installed [LuaRocks](https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Windows)
If not, you can install LuaJIT and LuaRocks by executing the following commands

```cmd
mkdir D:\development\git
git clone --depth 1 --branch v0.0.4 https://github.com/smbape/lua-opencv.git D:\development\git\lua-opencv
cd /d D:\development\git\lua-opencv
build.bat --target luajit "-DLua_VERSION=luajit-2.1" --install
build.bat --target luarocks
```

## Build System Environment

We will name our LuaRocks pakcage **opencv_lua-contrib-custom** in order to avoir conflict with the original package name

In this example, we will use the following directories: 
  - The **Lua binary directory** is _D:\development\git\lua-opencv\out\install\x64-Release\bin_
  - The **LuaRocks binary directory** is _D:\development\git\lua-opencv\out\build\x64-Release\luarocks\luarocks-prefix\src\luarocks_
  - The **build directory** is _D:\opencv-lua-custom\build_
  - The **server directory** is _D:\opencv-lua-custom\server_
  - The **test directory** is _D:\opencv-lua-custom\test_
  - The **vcpk directory** is _C:\vcpkg_

## Install freetype and harfbuzz with [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd)

Freetype and harfbuzz are needed to build the OpenCV freetype contrib module

```sh
git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
cd vcpkg && bootstrap-vcpkg.bat
vcpkg install freetype:x64-windows harfbuzz:x64-windows
```

## Build

Add your **Lua binary directory** to the PATH environment variable
```cmd
set PATH=D:\development\git\lua-opencv\out\install\x64-Release\bin;%PATH%
```

Add your **LuaRocks binary directory** to the PATH environment variable
```cmd
set PATH=D:\development\git\lua-opencv\out\build\x64-Release\luarocks\luarocks-prefix\src\luarocks;%PATH%
```

### Open GIT BASH in the **build directory**

```cmd
mkdir "D:\opencv-lua-custom\build"
cd /d "D:\opencv-lua-custom\build"
"%ProgramW6432%\Git\bin\bash.exe" -l -i
```

### Download the source code

```sh
git clone https://github.com/smbape/lua-opencv.git && \
cd lua-opencv
```

### Create the **opencv_lua-contrib-custom** rockspec

```sh
cp luarocks/opencv_lua-scm-1.rockspec opencv_lua-contrib-custom-scm-1.rockspec
```

Edit The **opencv_lua-contrib-custom**-scm-1.rockspec file:

#### Change the package name to **opencv_lua-contrib-custom**

```sh
sed -e 's/package = "opencv_lua"/package = "opencv_lua-contrib-custom"/' -i opencv_lua-contrib-custom-scm-1.rockspec
```

#### Add our custom build variable

```sh
sed -e 's/LUA_INCDIR = "$(LUA_INCDIR)",/LUA_INCDIR = "\$(LUA_INCDIR)",\n      BUILD_contrib = "ON",\n      CMAKE_TOOLCHAIN_FILE = "C:/vcpkg/scripts/buildsystems/vcpkg.cmake",\n      ENABLE_EXPERIMENTAL_WIDE_CHAR = "ON",\n      WITH_FREETYPE = "ON",/' -i opencv_lua-contrib-custom-scm-1.rockspec
```

The command above adds the following variables

```lua
      BUILD_contrib = "ON",
      CMAKE_TOOLCHAIN_FILE = "C:/vcpkg/scripts/buildsystems/vcpkg.cmake",
      ENABLE_EXPERIMENTAL_WIDE_CHAR = "ON",
      WITH_FREETYPE = "ON",
```

### Make the **opencv_lua-contrib-custom**-scm-1.rockspec rockspec

```sh
cd luarocks && \
luarocks --lua-version "5.1" --lua-dir "$(cygpath -w "$(dirname "$(dirname "$(command -v luajit.exe)")")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
cd .. && \
./luarocks/luarocks.bat make opencv_lua-contrib-custom-scm-1.rockspec
```

## Pack the prebuilt binary on the **server directory**

```sh
cd luarocks && \
cmake "-DLUAROCKS_EXE=$(command -v luarocks.exe)" "-DLUA_BINDIR=$(cygpath -w "$(dirname "$(command -v luajit.exe)")")" "-DLUA_INTERPRETER_NAME=luajit.exe" "-DABIVER=5.1" "-DEXTNAME=.bat" -P luarocks-init.cmake && \
cd .. && \
LUAROCKS_SERVER="/d/opencv-lua-custom/server" DIST_VERSION=1 ROCKSPEC=opencv_lua-contrib-custom-scm-1.rockspec node --trace-uncaught scripts/pack.js
```

## Testing our custom prebuilt binary

Open a new Command Prompt terminal. It doesn't have to be a Visual Studio command prompt

### Prepare

Add your **Lua binary directory** to the PATH environment variable
```cmd
set PATH=D:\development\git\lua-opencv\out\install\x64-Release\bin;%PATH%
```

Add your **LuaRocks binary directory** to the PATH environment variable
```cmd
set PATH=D:\development\git\lua-opencv\out\build\x64-Release\luarocks\luarocks-prefix\src\luarocks;%PATH%
```

### Initialize our test project and install our custom prebuilt binary

```cmd
git clone --depth 1 --branch 4.9.0 https://github.com/opencv/opencv_extra.git "D:\opencv-lua-custom\opencv_extra"
mkdir "D:\opencv-lua-custom\test"
cd /d "D:\opencv-lua-custom\test"
luarocks --lua-version "5.1" --lua-dir "D:\development\git\lua-opencv\out\install\x64-Release" init --lua-versions "5.1,5.2,5.3,5.4"
luarocks install "--server=D:\opencv-lua-custom\server" opencv_lua-contrib-custom
```

Replace the content of lua.bat with the following content

```cmd
@echo off
setlocal
IF "%*"=="" (set I=-i) ELSE (set I=)
set "LUAROCKS_SYSCONFDIR=C:\Program Files\luarocks"
set LUA_MODULES=%~dp0lua_modules
set "PATH=%LUA_MODULES%\bin;%APPDATA%\luarocks\bin;%PATH%"
"D:\development\git\lua-opencv\out\install\x64-Release\bin\luajit.exe" -e "package.path=\"%LUA_MODULES:\=\\%\\share\\lua\\5.1\\?.lua;%LUA_MODULES:\=\\%\\share\\lua\\5.1\\?\\init.lua;%APPDATA:\=\\%\\luarocks\\share\\lua\\5.1\\?.lua;%APPDATA:\=\\%\\luarocks\\share\\lua\\5.1\\?\\init.lua;\"..package.path;package.cpath=\"%LUA_MODULES:\=\\%\\lib\\lua\\5.1\\?.dll;%APPDATA:\=\\%\\luarocks\\lib\\lua\\5.1\\?.dll;\"..package.cpath" %I% %*
exit /b %ERRORLEVEL%
```

### Create a file with an UTF-16 name

```cmd
copy "D:\opencv-lua-custom\opencv_extra\testdata\cv\qrcode\multiple\6_qrcodes.png" "D:\opencv-lua-custom\opencv_extra\testdata\cv\qrcode\multiple\6_二维码.png" /b /y
```

### Execute a test script

Create a file test.lua

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

-- check that the correct version is used
print(cv.getBuildInformation())

cv.samples.addSamplesDataSearchPath(opencv_lua.fs_utils.findFile("opencv_extra/testdata/cv/qrcode/multiple"))

local img_rgb = cv.imread(cv.samples.findFile('6_二维码.png'))
assert(not img_rgb:empty(), "file could not be read, check with os.path.exists()")
local img_gray = cv.cvtColor(img_rgb, cv.COLOR_BGR2GRAY)
local qrcoder = cv.QRCodeDetector()

local retval, decoded_info, points, straight_qrcode = qrcoder:detectAndDecodeMulti(img_gray)

if retval then
    local ft2 = cv.freetype.createFreeType2()
    local font = opencv_lua.fs_utils.findFile("msjh.ttc", opencv_lua.kwargs({
        hints = {
            "C:\\Windows\\Fonts",
            ".",
        }
    }))
    print("font", font)
    ft2:loadFontData(font, 0)

    points = points:convertTo(cv.CV_32S)
    for i = 1, #points do
        local cnt = points[i - 1]
        cv.drawContours(img_rgb, { cnt }, 0, { 0, 0, 255 }, 2)

        local position = cnt:table()[1]

        ft2:putText(img_rgb, "中文", position, opencv_lua.kwargs({
            fontHeight = 12,
            color = { 255, 0, 255 },
            thickness = -1,
            line_type = cv.LINE_4,
            bottomLeftOrigin = false,
        }))
    end
end

cv.imshow("二维码", img_rgb)
cv.waitKey()
cv.destroyAllWindows()
```

Execture the test.lua script

```cmd
lua.bat test.lua
```

## Hosting on a web server

Alternatively, If you want an installation over http/s, upload the contents of **server directory** into an http/s server.

For example, if you uploaded it into http://example.com/binary-rock/, you can install the prebuilt binary with

```sh
luarocks install --server=http://example.com/binary-rock opencv_lua-contrib-custom
```
