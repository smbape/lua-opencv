# Hosting you own binary rocks on Linux

## Table Of Contents

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [Prepare](#prepare)
- [Prerequisites](#prerequisites)
- [Build System Environment](#build-system-environment)
- [Install freetype and harfbuzz](#install-freetype-and-harfbuzz)
- [Build](#build)
  - [Change directory to the **build directory**](#change-directory-to-the-build-directory)
  - [Download the source code](#download-the-source-code)
  - [Create the **opencv_lua-contrib-custom** rockspec](#create-the-opencv_lua-contrib-custom-rockspec)
    - [Change the package name to **opencv_lua-contrib-custom**](#change-the-package-name-to-opencv_lua-contrib-custom)
    - [Add our custom build variables in the build.variables](#add-our-custom-build-variables-in-the-buildvariables)
  - [Make the **opencv_lua-contrib-custom**-scm-1.rockspec rockspec](#make-the-opencv_lua-contrib-custom-scm-1rockspec-rockspec)
- [Pack the prebuilt binary on the **server directory**](#pack-the-prebuilt-binary-on-the-server-directory)
- [Testing our custom prebuilt binary](#testing-our-custom-prebuilt-binary)
  - [Prepare](#prepare-1)
  - [Initialize our test project and install our custom prebuilt binary](#initialize-our-test-project-and-install-our-custom-prebuilt-binary)
  - [Create a file with an UTF-16 name](#create-a-file-with-an-utf-16-name)
  - [Download the Microsoft JhengHei font family](#download-the-microsoft-jhenghei-font-family)
  - [Execute a test script](#execute-a-test-script)
- [Hosting on a web server](#hosting-on-a-web-server)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Here we will build a custom opencv with the folling modifications:
  - Add the contrib modules
  - Add the freetype module.

The procedure has been tested on :
  - [Windows WSL](https://learn.microsoft.com/en-us/windows/wsl/install) with [Ubuntu 22.04 (Jammy Jellyfish)](https://releases.ubuntu.com/jammy/)
  - [Windows WSL](https://learn.microsoft.com/en-us/windows/wsl/install) with [LinuxmintWSL2)](https://github.com/sileshn/LinuxmintWSL2)

## Prepare

  - Install [LuaRocks](https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Unix)
  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install needed packages `sudo snap install cmake --classic && sudo apt -y install build-essential git python3-pip python3-venv python-is-python3 ninja-build unzip zip qtbase5-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-dev libreadline-dev`

## Prerequisites

I assume you already have an [Lua](https://www.lua.org/start.html) or [LuaJIT](https://luajit.org/) installation.
I assume you have already installed [LuaRocks](https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Unix)
If not, you can install LuaJIT and LuaRocks by executing the following commands

```sh
mkdir -p "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de"
git clone --depth 1 --branch v0.0.4 https://github.com/smbape/lua-opencv.git $HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src
cd "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src"
./build.sh "-DLua_VERSION=luajit-2.1" --target luajit --install
./build.sh "-DLua_VERSION=luajit-2.1" --target luarocks
```

## Build System Environment

We will name our LuaRocks pakcage **opencv_lua-contrib-custom** in order to avoir conflict with the original package name

In this example, we will use the following directories: 
  - The **Lua binary directory** is _$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src/out/install/Linux-GCC-Release/bin_
  - The **LuaRocks binary directory** is _$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src/out/build.luaonly//Linux-GCC-Release/luarocks/luarocks-prefix/src/luarocks-build/bin_
  - The **build directory** is _$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/build_
  - The **server directory** is _$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/server_
  - The **test directory** is _$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/test_

## Install freetype and harfbuzz

Freetype and harfbuzz are needed to build the OpenCV freetype contrib module

```sh
sudo apt -y install libfreetype-dev libharfbuzz-dev
```

## Build

Add your **Lua binary directory** to the PATH environment variable
```sh
export PATH="$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src/out/install/Linux-GCC-Release/bin:$PATH"
```

Add your **LuaRocks binary directory** to the PATH environment variable
```sh
export PATH="$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src/out/build.luaonly//Linux-GCC-Release/luarocks/luarocks-prefix/src/luarocks-build/bin:$PATH"
```

### Change directory to the **build directory**

```sh
mkdir -p "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/build" && \
cd "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/build"
```

### Download the source code

```sh
git clone --depth 1 --branch v0.0.4 https://github.com/smbape/lua-opencv.git && \
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

#### Add our custom build variables in the build.variables

```sh
sed -e 's/LUA_INCDIR = "\$(LUA_INCDIR)",/LUA_INCDIR = "\$(LUA_INCDIR)",\n      BUILD_contrib = "ON",\n      WITH_FREETYPE = "ON",/' -i opencv_lua-contrib-custom-scm-1.rockspec
```

The command above adds the following variables

```lua
      BUILD_contrib = "ON",
      WITH_FREETYPE = "ON",
```

### Make the **opencv_lua-contrib-custom**-scm-1.rockspec rockspec

```sh
cd luarocks && \
luarocks --lua-version "5.1" --lua-dir "$(dirname "$(dirname "$(command -v luajit)")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
luarocks config --scope project cmake_generator Ninja && \
luarocks config --scope project cmake_build_args -- -j$(( $(nproc) - 2 )) && \
cd .. && \
./luarocks/luarocks make opencv_lua-contrib-custom-scm-1.rockspec
```

## Pack the prebuilt binary on the **server directory**

```sh
LUAROCKS_SERVER="$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/server" DIST_VERSION=1 ROCKSPEC=opencv_lua-contrib-custom-scm-1.rockspec node --trace-uncaught scripts/pack.js
```

## Testing our custom prebuilt binary

Open a new terminal.

### Prepare

Add your **Lua binary directory** to the PATH environment variable
```sh
export PATH="$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src/out/install/Linux-GCC-Release/bin:$PATH"
```

Add your **LuaRocks binary directory** to the PATH environment variable
```sh
export PATH="$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/src/out/build.luaonly//Linux-GCC-Release/luarocks/luarocks-prefix/src/luarocks-build/bin:$PATH"
```

### Initialize our test project and install our custom prebuilt binary

```sh
git clone --depth 1 --branch 4.9.0 https://github.com/opencv/opencv_extra.git "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/opencv_extra" && \
mkdir "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/test" && \
cd "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/test" && \
luarocks --lua-version "5.1" --lua-dir "$(dirname "$(dirname "$(command -v luajit)")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
luarocks install --server="$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/server" opencv_lua-contrib-custom
```

### Create a file with an UTF-16 name

```sh
cp -f "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/opencv_extra/testdata/cv/qrcode/multiple/6_qrcodes.png" "$HOME/.vs/lua-opencv/53b58a2f-f3e5-480b-8803-dc266ac326de/opencv-lua-custom/opencv_extra/testdata/cv/qrcode/multiple/6_二维码.png"
```

### Download the [Microsoft JhengHei](https://learn.microsoft.com/sr-cyrl-rs/typography/font-list/microsoft-jhenghei) font family

```sh
wget "https://github.com/taveevut/Windows-10-Fonts-Default/raw/master/msjh.ttc" -O msjh.ttc
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

```sh
./lua test.lua
```

## Hosting on a web server

Alternatively, If you want an installation over http/s, upload the contents of **server directory** into an http/s server.

For example, if you uploaded it into http://example.com/binary-rock/, you can install the prebuilt binary with

```sh
luarocks install --server=http://example.com/binary-rock opencv_lua-contrib-custom
```
