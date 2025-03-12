# Hosting you own binary rocks on Linux

## Table Of Contents

<!-- START doctoc generated TOC please keep comment here to allow auto update -->
<!-- DON'T EDIT THIS SECTION, INSTEAD RE-RUN doctoc TO UPDATE -->

- [Prerequisites](#prerequisites)
  - [[Ubuntu, Debian] Install needed packages:](#ubuntu-debian-install-needed-packages)
  - [[Fedora] Install needed packages:](#fedora-install-needed-packages)
  - [[Almalinux 8] Install needed packages:](#almalinux-8-install-needed-packages)
  - [[Almalinux 9] Install needed packages:](#almalinux-9-install-needed-packages)
- [Build System Environment](#build-system-environment)
- [Install freetype and harfbuzz](#install-freetype-and-harfbuzz)
  - [[Ubuntu, Debian] Install needed packages:](#ubuntu-debian-install-needed-packages-1)
  - [[Fedora, Almalinux] Install needed packages:](#fedora-almalinux-install-needed-packages)
- [Download the source code](#download-the-source-code)
- [Build](#build)
- [Testing our custom prebuilt binary](#testing-our-custom-prebuilt-binary)
  - [Prepare](#prepare)
  - [Initialize our test project and install our custom prebuilt binary](#initialize-our-test-project-and-install-our-custom-prebuilt-binary)
  - [Test freetype](#test-freetype)
    - [Create a file with an UTF-16 name](#create-a-file-with-an-utf-16-name)
    - [Download the Microsoft JhengHei font family](#download-the-microsoft-jhenghei-font-family)
    - [Execute a test script](#execute-a-test-script)
  - [Test CUDA](#test-cuda)
- [Hosting on a web server](#hosting-on-a-web-server)

<!-- END doctoc generated TOC please keep comment here to allow auto update -->

Here we will build a custom opencv with the folling modifications:
  - Add the contrib modules.
  - Add the freetype module.
  - Add NVIDIA CUDA support.

The procedure has been tested on :
  - [Ubuntu 22.04 (Jammy Jellyfish)](https://releases.ubuntu.com/jammy/)

## Prerequisites

  - Install [CUDA Toolkit 12.8.1](https://developer.nvidia.com/cuda-12-8-1-download-archive?target_os=Linux)
  - Download [cuDNN 9.8.0](https://developer.nvidia.com/cudnn-9-8-0-download-archive?target_os=Linux) compatible with the **CUDA Toolkit** version you installed, and extract the contents of the `bin`, `include` and `lib` directories inside the `bin`, `include` and `lib` directories of the **CUDA Toolkit** directory.
  - Download [NVIDIA Video Codec SDK 12.2.72](https://developer.nvidia.com/designworks/video-codec-sdk/secure/12.2/video_codec_sdk_12.2.72.zip) and extract the contents of the `Interface` and `Lib` directories inside the `include` and `lib` directories of the **CUDA Toolkit** directory.
  - Install [CMake >= 3.25](https://cmake.org/download/)
  - Install [LuaRocks](https://github.com/luarocks/luarocks/wiki/Installation-instructions-for-Unix)
  - Install [Ninja](https://ninja-build.org/)
  - Install [NodeJS](https://nodejs.org/en/download/current)

### [Ubuntu, Debian] Install needed packages:
```sh
sudo apt install -y build-essential curl git libavcodec-dev libavformat-dev libdc1394-dev \
        libjpeg-dev libpng-dev libreadline-dev libswscale-dev libtbb-dev \
        ninja-build patchelf pkg-config python3-pip python3-venv qtbase5-dev unzip wget zip
```

### [Fedora] Install needed packages:
```sh
sudo dnf install -y curl gcc gcc-c++ git \
        libjpeg-devel libpng-devel readline-devel make patch tbb-devel \
        libavcodec-free-devel libavformat-free-devel libdc1394-devel libswscale-free-devel \
        patchelf pkg-config python3-pip qt5-qtbase-devel unzip wget zip
```

### [Almalinux 8] Install needed packages:
```sh
sudo dnf install -y curl gcc-toolset-12-gcc gcc-toolset-12-gcc-c++ git \
        libjpeg-devel libpng-devel readline-devel make patch tbb-devel \
        patchelf pkg-config python3.12-pip qt5-qtbase-devel unzip wget zip && \
sudo config-manager --set-enabled powertools && \
sudo dnf install -y epel-release && \
sudo dnf install -y https://mirrors.rpmfusion.org/free/el/rpmfusion-free-release-8.noarch.rpm
sudo dnf install -y https://mirrors.rpmfusion.org/nonfree/el/rpmfusion-nonfree-release-8.noarch.rpm && \
sudo dnf update -y && \
sudo dnf install -y ffmpeg-devel && \
source /opt/rh/gcc-toolset-12/enable
```

### [Almalinux 9] Install needed packages:
```sh
sudo dnf install -y curl gcc gcc-c++ git \
        libjpeg-devel libpng-devel readline-devel make patch tbb-devel \
        patchelf pkg-config python3-pip qt5-qtbase-devel unzip wget zip && \
sudo config-manager --set-enabled crb && \
sudo dnf install -y epel-release && \
sudo dnf install -y https://mirrors.rpmfusion.org/free/el/rpmfusion-free-release-9.noarch.rpm
sudo dnf install -y https://mirrors.rpmfusion.org/nonfree/el/rpmfusion-nonfree-release-9.noarch.rpm && \
sudo dnf update -y && \
sudo dnf install -y libavcodec-free-devel libavformat-free-devel libdc1394-devel libswscale-free-devel
```

## Build System Environment

We will name our LuaRocks pakcage **opencv_lua-custom** in order to avoid conflict with the original package name

In this example, we will use the following directories: 
  - The **Lua binary directory** is _/io/luarocks-binaries-custom/lua-opencv/out/prepublish/build/opencv_lua-custom//out/install/Linux-GCC-Release/bin_
  - The **LuaRocks binary directory** is _/io/luarocks-binaries-custom/lua-opencv/out/prepublish/build/opencv_lua-custom/out/build.luaonly/Linux-GCC-Release/luarocks/luarocks-prefix/src/luarocks-build/bin_
  - The **build directory** is _/io/luarocks-binaries-custom/build_
  - The **server directory** is _/io/luarocks-binaries-custom/server_
  - The **test directory** is _/io/luarocks-binaries-custom/test_

## Install freetype and harfbuzz

Freetype and harfbuzz are needed to build the OpenCV freetype contrib module

### [Ubuntu, Debian] Install needed packages:
```sh
sudo apt install -y libfreetype-dev libharfbuzz-dev
```

### [Fedora, Almalinux] Install needed packages:
```sh
sudo apt install -y freetype-devel harfbuzz-devel
```

## Download the source code

```sh
git clone --depth 1 --branch v0.1.0 https://github.com/smbape/lua-opencv.git /io/luarocks-binaries-custom/lua-opencv && \
cd /io/luarocks-binaries-custom/build && \
npm ci
```

## Build

```sh
# --lua-versions luajit-2.1,5.1,5.2,5.3,5.4
node scripts/prepublish.js --pack --server=/io/luarocks-binaries-custom/server --lua-versions luajit-2.1 --name=opencv_lua-custom \
    --repair --plat auto --exclude "libc.so.*;libgcc_s.so.*;libstdc++.so.*;libxcb.so.*;libQt*;libcu*;libnp*" \
    -DBUILD_contrib=ON \
    -DWITH_FREETYPE=ON \
    -DWITH_CUDA=ON \
    -DWITH_CUDNN=ON \
    -DOPENCV_DNN_CUDA=ON \
    -DCUDA_ARCH_BIN=$(nvidia-smi --query-gpu=compute_cap --format=csv | sed /compute_cap/d)
```

  - `--repair`:            vendor in external shared library dependencies of the binary rock.
  - `--plat auto`: target this linux distribution
  - `--exclude "libc.so.*;libgcc_s.so.*;libstdc++.so.*;libxcb.so.*;libQt*;libcu*;libnp*;libnv*"`: exclude shared libraries that if vendored, may conflict (libc.so.\*;libgcc_s.so.\*;libstdc++.so.\*;libxcb.so.\*) with the system shared libraries or increase heavily (+850 Mo) the size of the binary rock (libQt*;libcu*;libnp*;libnv*).

  See [pypa/auditwheel](https://pypi.org/project/auditwheel/) for more information

## Testing our custom prebuilt binary

Open a new terminal.

### Prepare

Add your **Lua binary directory** to the PATH environment variable
```sh
export PATH="/io/luarocks-binaries-custom/lua-opencv/out/prepublish/build/opencv_lua-custom/out/install/Linux-GCC-Release/bin:$PATH"
```

Add your **LuaRocks binary directory** to the PATH environment variable
```sh
export PATH="/io/luarocks-binaries-custom/lua-opencv/out/prepublish/build/opencv_lua-custom/out/build.luaonly/Linux-GCC-Release/luarocks/luarocks-prefix/src/luarocks-build/bin:$PATH"
```

### Initialize our test project and install our custom prebuilt binary

```sh
git clone --depth 1 --branch 4.11.0 https://github.com/opencv/opencv_extra.git /io/luarocks-binaries-custom/opencv_extra && \
mkdir /io/luarocks-binaries-custom/test && \
cd /io/luarocks-binaries-custom/test && \
luarocks --lua-version 5.1 --lua-dir "$(dirname "$(dirname "$(command -v luajit)")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
luarocks install --server=/io/luarocks-binaries-custom/server opencv_lua-custom
```

### Test freetype

#### Create a file with an UTF-16 name

```sh
cp -f /io/luarocks-binaries-custom/opencv_extra/testdata/cv/qrcode/multiple/6_qrcodes.png /io/luarocks-binaries-custom/opencv_extra/testdata/cv/qrcode/multiple/6_二维码.png
```

#### Download the [Microsoft JhengHei](https://learn.microsoft.com/sr-cyrl-rs/typography/font-list/microsoft-jhenghei) font family

```sh
wget https://github.com/taveevut/Windows-10-Fonts-Default/raw/master/msjh.ttc -O msjh.ttc
```

#### Execute a test script

Create a file test-opencv.lua

```lua
local opencv_lua = require("opencv_lua")
local cv = opencv_lua.cv

-- check that the correct version is used
print(cv.getBuildInformation())

assert(cv.cuda.getCudaEnabledDeviceCount() ~= 0, "Ensure GPU is available")

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
    assert(#font ~= 0, "msjh.ttc was not fount")
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

cv.imshow("qrcodes", img_rgb)
cv.waitKey()
cv.destroyAllWindows()
```

Execute the test-opencv.lua script

```sh
./lua test-opencv.lua
```

### Test CUDA

```sh
git clone --depth 1 --branch 4.11.0 https://github.com/opencv/opencv.git /io/luarocks-binaries-custom/opencv && \
./luarocks install --deps-only /io/luarocks-binaries-custom/lua-opencv/samples/samples-scm-1.rockspec && \
./lua /io/luarocks-binaries-custom/lua-opencv/samples/dnn/object_detection/object_detection.lua ssd_tf --input Megamind.avi --backend 5 --target 6
```

## Hosting on a web server

Alternatively, If you want an installation over http/s, upload the contents of **server directory** into an http/s server.

For example, if you uploaded it into http://example.com/binary-rock/, you can install the prebuilt binary with

```sh
luarocks install --server=http://example.com/binary-rock opencv_lua-custom
```
