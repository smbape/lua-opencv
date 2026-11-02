# Hosting you own binary rocks on Windows

## Table Of Contents

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [Prerequisites](#prerequisites)
- [Build System Environment](#build-system-environment)
- [Install freetype and harfbuzz with vcpkg](#install-freetype-and-harfbuzz-with-vcpkg)
- [Open GIT BASH in the **build directory**](#open-git-bash-in-the-build-directory)
- [Download the source code](#download-the-source-code)
- [Build](#build)
- [Testing our custom prebuilt binary](#testing-our-custom-prebuilt-binary)
  - [Prepare](#prepare)
  - [Initialize our test project and install our custom prebuilt binary](#initialize-our-test-project-and-install-our-custom-prebuilt-binary)
  - [Test freetype](#test-freetype)
    - [Create a file with an UTF-16 name](#create-a-file-with-an-utf-16-name)
    - [Execute a test script](#execute-a-test-script)
  - [Test CUDA](#test-cuda)
- [Hosting on a web server](#hosting-on-a-web-server)
- [Sources](#sources)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Here we will build a custom opencv with the folling modifications:
  - Enable experimental support of UTF-16 (wide character) strings on Windows.
  - Add the contrib modules.
  - Add the freetype module.
  - Add NVIDIA CUDA support.

The procedure has been tested on:
  - Windows 11.

## Prerequisites

  - Install [CUDA Toolkit 13.3.1](https://developer.nvidia.com/cuda-13-3-1-download-archive?target_os=Windows&target_arch=x86_64&target_version=11&target_type=exe_local)
  - Download [cuDNN 9.25.1](https://developer.download.nvidia.com/compute/cudnn/redist/cudnn/windows-x86_64/cudnn-windows-x86_64-9.25.1.1_cuda13-archive.zip) compatible with the **CUDA Toolkit** version you installed, and extract the contents of the `bin`, `include` and `lib` directories inside the `bin`, `include` and `lib` directories of the **CUDA Toolkit** directory.
  - Download [NVIDIA Video Codec SDK](https://developer.nvidia.com/video-codec-sdk) and extract the contents of the `Interface` and `Lib/win` directories inside the `include` and `lib` directories of the **CUDA Toolkit** directory.
  - Install [CMake >= 3.25](https://cmake.org/download/)
  - Install [Git](https://git-scm.com/)
  - Install [NodeJS](https://nodejs.org/en/download/current)
  - Install [Python 3.9...<3.15](https://www.python.org/downloads/)
  - Install [NASM — The Netwide Assembler](https://www.nasm.us/), add the installation folder to your PATH environment variable, add a `ASM_NASM` with value `nasm.exe`
  - Install [Visual Studio 2026](https://visualstudio.microsoft.com/fr/downloads/)

In your windows search, search and open the `x64 Native Tools Command Prompt for VS`

From here on, commands will be executed within the opened Command Prompt.

## Build System Environment

We will name our LuaRocks pakcage **opencv_lua-custom** in order to avoid conflict with the original package name

In this example, we will use the following directories: 
  - The **Lua binary directory** is _D:\luarocks-binaries-custom\lua-opencv\out\prepublish\build\opencv_lua-custom\out\install\x64-Release\bin_
  - The **LuaRocks binary directory** is _D:\luarocks-binaries-custom\lua-opencv\out\prepublish\build\opencv_lua-custom\out\build.luaonly\x64-Release\luarocks\luarocks-prefix\src\luarocks_
  - The **server directory** is _D:\luarocks-binaries-custom\server_
  - The **test directory** is _D:\luarocks-binaries-custom\test\lua-opencv_

## Open GIT BASH in the **build directory**

```cmd
"%ProgramW6432%\Git\bin\bash.exe" -l -i
```

## Download the source code

```sh
git clone --depth 1 --branch v5.0.0-1 https://github.com/smbape/lua-opencv.git /d/luarocks-binaries-custom/lua-opencv && \
cd /d/luarocks-binaries-custom/lua-opencv && \
npm ci
```

### Add freetype and harfbuzz dependencies with [vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-cmd)

```sh
vcpkg new --application && \
vcpkg add port freetype harfbuzz
```

## Build

```sh
# --lua-versions luajit-2.1,5.1,5.2,5.3,5.4,5.5
node scripts/prepublish.js --pack --server="/d/luarocks-binaries-custom/server" --lua-versions luajit-2.1 --name=opencv_lua-custom \
    -DENABLE_REPAIR=ON \
    -DDELVEWHEEL_exclude="cublas*;cudnn*;cufft*;nvcuda*;nvcuvid*;nvidia*;npp*;nvEncodeAPI*; " \
    -DCMAKE_TOOLCHAIN_FILE="$(cygpath -m "$VCPKG_ROOT")/scripts/buildsystems/vcpkg.cmake" \
    -DENABLE_EXPERIMENTAL_WIDE_CHAR=ON \
    -DBUILD_contrib=ON \
    -DWITH_FREETYPE=ON \
    -DWITH_CUDA=ON \
    -DWITH_CUDNN=ON \
    -DOPENCV_DNN_CUDA=ON \
    -DCUDA_ARCH_BIN=$(nvidia-smi --query-gpu=compute_cap --format=csv | sed /compute_cap/d)
```

  - `-DDELVEWHEEL_exclude="cublas*;cudnn*;cufft*;nvcuda*;nvcuvid*;nvidia*;npp*;nvEncodeAPI*; "`: exclude shared libraries that, if vendored, may conflict with the system shared libraries or heavily increase (+850 Mo) the size of the binary rock.

## Testing our custom prebuilt binary

Open a new Command Prompt terminal. It doesn't have to be a Visual Studio command prompt

### Prepare

Add your **Lua binary directory** to the PATH environment variable
```cmd
set PATH=D:\luarocks-binaries-custom\lua-opencv\out\prepublish\build\opencv_lua-custom\out\install\x64-Release\bin;%PATH%
```

Add your **LuaRocks binary directory** to the PATH environment variable
```cmd
set PATH=D:\luarocks-binaries-custom\lua-opencv\out\prepublish\build\opencv_lua-custom\out\build.luaonly\x64-Release\luarocks\luarocks-prefix\src\luarocks;%PATH%
```

### Initialize our test project and install our custom prebuilt binary

```cmd
chcp 65001
git clone --depth 1 --branch 5.0.0 https://github.com/opencv/opencv_extra.git "D:\luarocks-binaries-custom\opencv_extra"
mkdir "D:\luarocks-binaries-custom\test\lua-opencv"
cd /d "D:\luarocks-binaries-custom\test\lua-opencv"
luarocks --lua-version "5.1" init --lua-versions "5.1,5.2,5.3,5.4,5.5"
luarocks install "--server=D:\luarocks-binaries-custom\server" opencv_lua-custom
```

### Test freetype

#### Create a file with an UTF-16 name

```cmd
copy "D:\luarocks-binaries-custom\opencv_extra\testdata\cv\qrcode\multiple\6_qrcodes.png" "D:\luarocks-binaries-custom\opencv_extra\testdata\cv\qrcode\multiple\6_二维码.png" /b /y
```

#### Execute a test script

Create a file `test-opencv.lua`

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

-- check that the correct version is used
print(cv.getBuildInformation())

assert(cv.cuda.getCudaEnabledDeviceCount() ~= 0, "Ensure GPU is available")

cv.samples.addSamplesDataSearchPath(opencv_lua.fs_utils.findFile("opencv_extra/testdata/cv/qrcode/multiple"))

local img_rgb = cv.imread(cv.samples.findFile('6_二维码.png')) -- without wide char support, cv.samples.findFile cannot find UTF-16 named files
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
    assert(#font ~= 0, "msjh.ttc was not found")
    ft2:loadFontData(font, 0)

    points = points:convertTo(cv.CV_32S):table()
    for i = 1, #points do
        local cnt = points[i]
        cv.drawContours(img_rgb, { cnt }, 0, { 0, 0, 255 }, 2)

        local position = cnt[1]

        ft2:putText(img_rgb, "中文", position, opencv_lua.kwargs({
            fontHeight = 12,
            color = { 255, 0, 255 },
            thickness = -1,
            line_type = cv.LINE_4,
            bottomLeftOrigin = false,
        }))
    end
end

cv.imshow("二维码", img_rgb) -- without wide char support, the title is ???
cv.waitKey()
cv.destroyAllWindows()

```

Execute the `test-opencv.lua` script

```cmd
lua.bat test-opencv.lua
```

### Test CUDA

Open a `x64 Native Tools Command Prompt for VS` terminal

```cmd
cd /d "D:\luarocks-binaries-custom\test\lua-opencv"
git clone --depth 1 --branch 5.0.0 https://github.com/opencv/opencv.git "D:\luarocks-binaries-custom\opencv"
luarocks.bat install --deps-only D:\luarocks-binaries-custom\lua-opencv\samples\samples-scm-1.rockspec
lua.bat D:\luarocks-binaries-custom\lua-opencv\samples\dnn\object_detection\object_detection.lua ssd_tf --input Megamind.avi --backend cuda --target cuda --fps=-1
```

## Hosting on a web server

Alternatively, If you want an installation over http/s, upload the contents of **server directory** into an http/s server.

For example, if you uploaded it into http://example.com/binary-rock/, you can install the prebuilt binary with

```sh
luarocks install --server=http://example.com/binary-rock opencv_lua-custom
```

## Sources

[Build OpenCV on Windows with CUDA!](https://medium.com/@chinssk/build-opencv-on-windows-with-cuda-f880270eadb0)
[Build OpenCV with DNN and CUDA for GPU-Accelerated Face Detection](https://medium.com/@amosstaileyyoung/build-opencv-with-dnn-and-cuda-for-gpu-accelerated-face-detection-27a3cdc7e9ce)
[How to Install cuDNN and CUDA for Windows and Linux](https://builtin.com/articles/cudnn-download)
