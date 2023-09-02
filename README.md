# OpenCV bindings for lua

OpenCV bindings for luajit and lua 5.1/5.2/5.3/5.4.

The aim is to make it as easy to use as [opencv-python](https://pypi.org/project/opencv-python/).

Therefore the [OpenCV documentation](https://docs.opencv.org/4.x/index.html) should be the reference.

## Table Of Contents

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [Installation](#installation)
  - [Prerequisites to source rock install](#prerequisites-to-source-rock-install)
    - [Windows](#windows)
    - [Linux](#linux)
  - [Automatic guess install](#automatic-guess-install)
  - [Install the source rock](#install-the-source-rock)
  - [Install the prebuilt binaries for lua 5.4](#install-the-prebuilt-binaries-for-lua-54)
  - [Install the prebuilt binaries for LuaJIT 2.1](#install-the-prebuilt-binaries-for-luajit-21)
- [Examples](#examples)
  - [Show image](#show-image)
  - [Video capture Camera](#video-capture-camera)
  - [Rotate image](#rotate-image)
  - [Drawing contours](#drawing-contours)
  - [Template matching](#template-matching)
- [Runing examples](#runing-examples)
  - [Prerequisites](#prerequisites)
    - [Windows](#windows-1)
    - [Linux](#linux-1)
  - [Initialize the project](#initialize-the-project)
    - [Windows](#windows-2)
    - [Linux](#linux-2)
- [Hosting you own binary rocks](#hosting-you-own-binary-rocks)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Installation

Prebuilt binaries are available for [LuaJIT 2.1](https://luajit.org/) and [Lua 5.1/5.2/5.3/5.4](https://www.lua.org/versions.html), and only on Windows and Linux.

### Prerequisites to source rock install

#### Windows

  - Install [CMake](https://cmake.org/download/)
  - Install [Git](https://git-scm.com/)
  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install [Python](https://www.python.org/downloads/)
  - Install [Visual Studio](https://visualstudio.microsoft.com/fr/downloads/)
  - In your windows search, search and open the `x64 Native Tools Command Prompt for VS`

#### Linux

  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install needed packages `sudo apt -y install build-essential cmake git python3-pip libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-dev libreadline-dev`
  - For faster build timer, you can use [Ninja](https://ninja-build.org/) `luarocks config --scope project cmake_generator Ninja`

### Automatic guess install

This will install suitable prebuilt binaries if found, otherwise will build from the source rock

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.0 opencv_lua
```

### Install the source rock

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.0 opencv_lua 4.8.0
```

### Install the prebuilt binaries for lua 5.4

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.0 opencv_lua 4.8.0lua5.4
```

### Install the prebuilt binaries for LuaJIT 2.1

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.0 opencv_lua 4.8.0luajit2.1
```

## Examples

On Windows, the lua_modules modules should be added to PATH as show with `luarocks path`

```sh
set "PATH=%LUA_MODULES%\bin;%APPDATA%\luarocks\bin;%PATH%"
```

`LUA_MODULES` pointing to your lua_modules folder if any.

For example, in your lua project, initialized with `luarocks init`, modify the file `lua.bat` and after the line `set "LUAROCKS_SYSCONFDIR=` add

```cmd
set LUA_MODULES=%~dp0lua_modules
set "PATH=%LUA_MODULES%\bin;%APPDATA%\luarocks\bin;%PATH%"
```

### Show image

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

local img = cv.imread("lena.jpg")
cv.imshow("Image", img)
cv.waitKey()
cv.destroyAllWindows()
```

### Video capture Camera

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv
local int = opencv_lua.math.int

local camId = 0
local cap = cv.VideoCapture(camId)
if not cap:isOpened() then error("!>Error: cannot open the camera " .. camId) end

local CAP_FPS = 60
local CAP_SPF = int(1000 / CAP_FPS)

cap:set(cv.CAP_PROP_FRAME_WIDTH, 1280)
cap:set(cv.CAP_PROP_FRAME_HEIGHT, 720)
cap:set(cv.CAP_PROP_FPS, CAP_FPS)

while true do
    -- Without this, memory grows indefenitely
    collectgarbage()

    local start = cv.getTickCount()
    local read, frame = cap:read()
    if not read then
        io.stderr:write("!>Error: cannot read the camera.\n")
    end
    local fps = cv.getTickFrequency() / (cv.getTickCount() - start)

    -- Flip the image horizontally to give the mirror impression
    local frame = cv.flip(frame, 1)

    cv.putText(frame, string.format("FPS : %.2f", fps), { 10, 30 }, cv.FONT_HERSHEY_PLAIN, 2, {255, 0, 255}, 3)
    cv.imshow("capture camera", frame)

    local key = cv.waitKey(CAP_SPF)
    if key == 0x1b or key == string.byte("q") or key == string.byte("Q") then break end
end

cv.destroyAllWindows()
```

### Rotate image

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

local img = cv.imread("lena.jpg")
local angle = 20
local scale = 1

local size = { img.width, img.height }
local center = { img.width / 2, img.height / 2 }
local M = cv.getRotationMatrix2D(center, -angle, scale)
local rotated = cv.warpAffine(img, M, size)

cv.imshow("Rotation", rotated)
cv.waitKey()
cv.destroyAllWindows()
```

### Drawing contours

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

local img = cv.imread("pic1.png")
local img_grey = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
local ret, thresh = cv.threshold(img_grey, 100, 255, cv.THRESH_BINARY)
local contours, hierarchy = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

print("Found " .. #contours .. " contours")

cv.drawContours(img, contours, -1, { 0, 0, 255 }, 2)

cv.imshow("Image", img)
cv.waitKey()
cv.destroyAllWindows()
```

### Template matching

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

--[[
Sources:
    https://docs.opencv.org/4.x/d4/dc6/tutorial_py_template_matching.html
--]]

local img_rgb = cv.imread("mario.png")
assert(img_rgb, "file could not be read, check with os.path.exists()")
local img_gray = cv.cvtColor(img_rgb, cv.COLOR_BGR2GRAY)
local template = cv.imread("mario_coin.png", cv.IMREAD_GRAYSCALE)
assert(template, "file could not be read, check with os.path.exists()")

local h, w = template.height, template.width

local res = cv.matchTemplate(img_gray,template,cv.TM_CCOEFF_NORMED)
local threshold = 0.8

for j=1,res.rows do
    local y = j - 1
    for i=1,res.cols do
        local x = i - 1
        if res(y, x) >= threshold then
            cv.rectangle(img_rgb, { x, y }, { x + w, y + h }, { 0, 0, 255 }, 2)
        end
    end
end

cv.imshow("res.png", img_rgb)
cv.waitKey()
cv.destroyAllWindows()
```

## Runing examples

### Prerequisites

#### Windows

  - Install [CMake](https://cmake.org/download/)
  - Install [Git](https://git-scm.com/)
  - Install [NodeJS](https://nodejs.org/en/download/current)

#### Linux

  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install needed packages `sudo apt -y install cmake git`

### Initialize the project

#### Windows

```cmd
git clone --depth 1 --branch 4.8.0 https://github.com/opencv/opencv.git
git clone --depth 1 --branch v0.0.0 https://github.com/smbape/lua-opencv.git
cd lua-opencv
build.bat --target lua "-DLua_VERSION=5.4" --install
build.bat --target luarocks
luarocks\luarocks.bat install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.0 opencv_lua
node scripts\test.js --Release
```

#### Linux

```cmd
git clone --depth 1 --branch 4.8.0 https://github.com/opencv/opencv.git
git clone --depth 1 --branch v0.0.0 https://github.com/smbape/lua-opencv.git
cd lua-opencv
./build.sh --target lua "-DLua_VERSION=5.4" --install
./build.sh --target luarocks
./luarocks/luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.0 opencv_lua
node scripts\test.js --Release
```

## Hosting you own binary rocks

If the provided binray rocks are not suitable for your environnment, you can install the source rock.  
Installing the source rock takes a long time (01h25mn on my computer).  
Therefore, it is not practical to repeat that process again.  
To avoid that long install time, you can host your own prebuilt binary rocks.

```sh
git clone --depth 1 --branch v0.0.0 https://github.com/smbape/lua-opencv.git
cd lua-opencv/luarocks
luarocks --lua-version <Which Lua version to use> --lua-dir "<Which Lua installation to use>" init --lua-versions "5.1,5.2,5.3,5.4"
cd ..
luarocks/luarocks make luarocks/opencv_lua-scm-1.rockspec
LUAROCKS_SERVER="<Where to install the prebuilt binary>" node scripts/packs.js
```

Now you can install the prebuilt binary with

```sh
luarocks install --server=$LUAROCKS_SERVER
```

Alternatively, If you want an installation over http, upload the contents of `LUAROCKS_SERVER` into an HTTP/S server

For example, if you uploaded it into http://example.com/binary-rock/, you can install the prebuilt binary with

```sh
luarocks install --server=http://example.com/binary-rock opencv_lua
```
