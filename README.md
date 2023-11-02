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
  - [Rotate an image](#rotate-image)
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
- [Keyword arguments](#keyword-arguments)
- [How to translate python/c++ code](#how-to-translate-pythonc-code)
  - [Python translation example](#python-translation-example)
- [Python Gotchas](#python-gotchas)
  - [1-indexed](#1-indexed)
  - [Instance method calls](#instance-method-calls)
  - [Strict compliance with documentation](#strict-compliance-with-documentation)
  - [Memory](#memory)
  - [Matrix manipulation](#matrix-manipulation)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

## Installation

Prebuilt binaries are available for [LuaJIT 2.1](https://luajit.org/) and [Lua 5.1/5.2/5.3/5.4](https://www.lua.org/versions.html), and only on Windows and Linux.

### Prerequisites to source rock install

#### Windows

  - Install [CMake >= 3.25](https://cmake.org/download/)
  - Install [Git](https://git-scm.com/)
  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install [Python](https://www.python.org/downloads/)
  - Install [Visual Studio 2022 >= 17.7.2](https://visualstudio.microsoft.com/fr/downloads/)
  - In your windows search, search and open the `x64 Native Tools Command Prompt for VS 2022`

#### Linux

  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install needed packages `sudo snap install cmake --classic && sudo apt -y install build-essential git python3-pip libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev libtbb2 libtbb-dev libjpeg-dev libpng-dev libtiff-dev libdc1394-dev libreadline-dev`
  - For faster build time, you can use [Ninja](https://ninja-build.org/) `luarocks config --scope project cmake_generator Ninja`

### Automatic guess install

This will install suitable prebuilt binaries if found, otherwise will build from the source rock

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.2 opencv_lua
```

### Install the source rock

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.2 opencv_lua 4.8.0
```

### Install the prebuilt binaries for lua 5.4

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.2 opencv_lua 4.8.0lua5.4
```

### Install the prebuilt binaries for LuaJIT 2.1

```sh
luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.2 opencv_lua 4.8.0luajit2.1
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
    -- Without this, memory grows indefinitely
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

### Rotate an image

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
assert(not img_rgb:empty(), "file could not be read, check with os.path.exists()")
local img_gray = cv.cvtColor(img_rgb, cv.COLOR_BGR2GRAY)
local template = cv.imread("mario_coin.png", cv.IMREAD_GRAYSCALE)
assert(not template:empty(), "file could not be read, check with os.path.exists()")

local h, w = template.height, template.width

local res = cv.matchTemplate(img_gray, template, cv.TM_CCOEFF_NORMED)
local threshold = 0.8

-- transform into an lua table for faster processing in lua
local rows, cols = res.rows, res.cols
res = res:table()

for j = 1, rows do
    local y = j - 1
    for i = 1, cols do
        local x = i - 1
        if res[j][i] >= threshold then
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

  - Install [Git](https://git-scm.com/)
  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install [Visual Studio 2022 >= 17.7.2](https://visualstudio.microsoft.com/fr/downloads/)

#### Linux

  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install needed packages `sudo snap install cmake --classic && sudo apt -y install build-essential git ninja-build libreadline-dev unzip`

### Initialize the project

#### Windows

```cmd
git clone --depth 1 --branch 4.8.0 https://github.com/opencv/opencv.git
git clone --depth 1 --branch v0.0.2 https://github.com/smbape/lua-opencv.git
cd lua-opencv
build.bat --target lua "-DLua_VERSION=5.4" --install
build.bat --target luarocks
luarocks\luarocks.bat install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.2 opencv_lua
npm ci
node scripts\test.js --Release
```

#### Linux

```sh
git clone --depth 1 --branch 4.8.0 https://github.com/opencv/opencv.git
git clone --depth 1 --branch v0.0.2 https://github.com/smbape/lua-opencv.git
cd lua-opencv
./build.sh --target lua "-DLua_VERSION=5.4" --install
./build.sh --target luarocks
./luarocks/luarocks install --server=https://github.com/smbape/lua-opencv/releases/download/v0.0.2 opencv_lua
npm ci
node scripts/test.js --Release
```

## Hosting you own binary rocks

If the provided binray rocks are not suitable for your environnment, you can install the source rock.  
Installing the source rock takes a long time (01h25mn on my computer).  
Therefore, it is not practical to repeat that process again.  
To avoid that long install time, you can host your own prebuilt binary rocks.

```sh
git clone --depth 1 --branch v0.0.2 https://github.com/smbape/lua-opencv.git
cd lua-opencv/luarocks
luarocks --lua-version "<Which Lua version to use>" --lua-dir "<Which Lua installation to use>" init --lua-versions "5.1,5.2,5.3,5.4"
luarocks --lua-version "<Which Lua version to use>" config --scope project cmake_generator Ninja
cd ..
./luarocks/luarocks make luarocks/opencv_lua-scm-1.rockspec
LUAROCKS_SERVER="<Where to install the prebuilt binary>" DIST_VERSION=1 node scripts/pack.js
```

Now you can install the prebuilt binary with

```sh
luarocks install --server="$LUAROCKS_SERVER"
```

Alternatively, If you want an installation over http, upload the contents of `LUAROCKS_SERVER` into an HTTP/S server

For example, if you uploaded it into http://example.com/binary-rock/, you can install the prebuilt binary with

```sh
luarocks install --server=http://example.com/binary-rock opencv_lua
```

## Keyword arguments

Similar to python [keyword arguments](https://docs.python.org/3/glossary.html#term-argument) or c# [Named parameters](https://learn.microsoft.com/en-us/dotnet/csharp/programming-guide/classes-and-structs/named-and-optional-arguments#named-arguments), keyword arguments free you from matching the order of arguments to the order of parameters in the parameter lists of called methods.

When a function has multiple default parameters, keyword arguments allows you to override only the needed parameters without specifying the previous default parameters.

As an example, given the documentation of [cv.normalize](https://docs.opencv.org/4.x/d2/de8/group__core__array.html#ga87eef7ee3970f86906d69a92cbf064bd)

```cpp
void cv::normalize  (   InputArray       src,
                        InputOutputArray dst,
                        double           alpha = 1,
                        double           beta = 0,
                        int              norm_type = NORM_L2,
                        int              dtype = -1,
                        InputArray       mask = noArray()
    )       
Python:
    cv.normalize( src, dst[, alpha[, beta[, norm_type[, dtype[, mask]]]]] ) -> dst
```

the following expressions are equivalent:

```lua
cv.normalize(src, dst, alpha, beta, norm_type, dtype, mask)
cv.normalize(src, dst, alpha, beta, norm_type, dtype, opencv_lua.kwargs({ mask = mask }))
cv.normalize(src, dst, alpha, beta, norm_type, opencv_lua.kwargs({ dtype = dtype, mask = mask }))
cv.normalize(src, dst, alpha, beta, opencv_lua.kwargs({ norm_type = norm_type, dtype = dtype, mask = mask }))
cv.normalize(src, dst, alpha, opencv_lua.kwargs({ beta = beta, norm_type = norm_type, dtype = dtype, mask = mask }))
cv.normalize(src, dst, opencv_lua.kwargs({ alpha = alpha, beta = beta, norm_type = norm_type, dtype = dtype, mask = mask }))
cv.normalize(src, opencv_lua.kwargs({ dst = dst, alpha = alpha, beta = beta, norm_type = norm_type, dtype = dtype, mask = mask }))
cv.normalize(opencv_lua.kwargs({ src = src, dst = dst, alpha = alpha, beta = beta, norm_type = norm_type, dtype = dtype, mask = mask }))
```

Of course, optional parameters are not mandatory. In other words, if you only want to change the `norm_type` parameter, you can do

```lua
cv.normalize(src, dst, opencv_lua.kwargs({ norm_type = cv.NORM_L1 }))
```

## How to translate python/c++ code

The transformation will usually be straight from python.

`tuples` and `arrays` becomes `tables`.

`numpy` calls and `arrays` manipulations have their `cv.Mat` counter parts.

`keyword arguments` will be wrapped within `opencv_lua.kwargs`.

### Python translation example

```python
import cv2 as cv

img = cv.imread(cv.samples.findFile("pic1.png"))
img_grey = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
ret, thresh = cv.threshold(img_grey, 100, 255, cv.THRESH_BINARY)
contours, hierarchy = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

print(f"Found { len(contours) } contours")

cv.drawContours(img, contours, -1, ( 0, 0, 255 ) , 2)

cv.imshow("Image", img)
cv.waitKey()
cv.destroyAllWindows()
```

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

local img = cv.imread(cv.samples.findFile("pic1.png"))
local img_grey = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
local ret, thresh = cv.threshold(img_grey, 100, 255, cv.THRESH_BINARY)
local contours, hierarchy = cv.findContours(thresh, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)

print("Found " .. #contours .. " contours")

cv.drawContours(img, contours, -1, { 0, 0, 255 }, 2)

cv.imshow("Image", img)
cv.waitKey()
cv.destroyAllWindows()
```

## Python Gotchas

### 1-indexed

Arrays are 1-indexed in `lua`, and 0-indexed in python. Therefore, if you see in python `p[0]`, you should write in lua `p[1]`

More over, opencv functions still expect 0-indexed parameters. For exemple

```lua
for i = 1, #contours do
    -- Notice the i - 1
    cv.drawContours(src, contours, i - 1, {0, 0, 255}, 2)
end
```

### Instance method calls

Instance method are called with `:` not `.`.

If you see in python

```python
cap = cv.VideoCapture(camId)
read, frame = cap.read()
```

You should write in lua

```python
cap = cv.VideoCapture(camId)
read, frame = cap:read()
```

### Strict compliance with documentation

For backwards compatibility, python still allow an old syntax for enums and call constructors. This library does not.

If you see in python

```python
cv.ml.SVM_create(...)
cv.ml.SVM_C_SVC
cv.TERM_CRITERIA_MAX_ITER
```

You should write in lua

```lua
cv.ml.SVM.create(...)
cv.ml.SVM.C_SVC
cv.TermCriteria.MAX_ITER
```

### Memory

Image processing can use up a lot of memory. Relying solely on the behaviour of the garbage collector can cause your program to consume more memory than necessary.

Therefore, think about calling `collectgarbage` in those cases.

```lua
local cap = cv.VideoCapture(0)
while true do
    -- Without this, memory grows indefinitely
    collectgarbage()

    local read, frame = cap:read()
    cv.imshow("capture camera", frame)
    if cv.waitKey(0) == 0x1b then break end
end
```

### Matrix manipulation

For maximum speed performance, when you need to manipulate matrices in lua, convert them `table`, do your manipulation and convert them back to matrices

```lua
-- transform into an lua table for faster processing in lua
local rows, cols, type = res.rows, res.cols, res:type()
res = res:table()

for j = 1, rows do
    local y = j - 1
    for i = 1, cols do
        local x = i - 1
        if res[j][i] >= threshold then
            cv.rectangle(img_rgb, { x, y }, { x + w, y + h }, { 0, 0, 255 }, 2)
        end
    end
end

res = cv.Mat.createFromArray(res, type)
```
