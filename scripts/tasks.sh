#!/usr/bin/env bash

set -o pipefail

DIST_VERSION=3

WSL="$(command -v wsl)"
function wsl() {
    version="${version}" suffix="${suffix}" DIST_VERSION="${DIST_VERSION}" WSLENV=version/u:suffix/u:DIST_VERSION/u "$WSL" -e bash -li "$@"
}

function stash_push() {
    git stash push --include-untracked
}

function prepublish_stash_push() {
    for version in luajit-2.1 5.{4,3,2,1}; do
        for suffix in '' '-contrib'; do
            bash -c "cd out/prepublish/${version}/lua-opencv${suffix} && git stash push --include-untracked --all -- samples"
            wsl -c 'source scripts/wsl_init.sh && cd out/prepublish/${version}/lua-opencv${suffix} && git stash push --include-untracked --all -- samples'
        done
    done
}

function prepublish_stash_pop() {
    for version in luajit-2.1 5.{4,3,2,1}; do
        for suffix in '' '-contrib'; do
            bash -c "cd out/prepublish/${version}/lua-opencv${suffix} && git reset --hard HEAD && git stash pop"
            wsl -c 'source scripts/wsl_init.sh && cd out/prepublish/${version}/lua-opencv${suffix} && git reset --hard HEAD && git stash pop'
        done
    done
}

function prepublish_test_build_windows() {
    for version in luajit-2.1 5.{4,3,2,1}; do
        for suffix in '' '-contrib'; do
            bash -c '
version='${version}'
suffix='${suffix}'
source scripts/vcvars_restore_start.sh || exit $?
cd out/prepublish/${version}/lua-opencv${suffix} && ./luarocks/luarocks.bat install --deps-only samples/samples-scm-1.rockspec
' && \
            WORKSPACE_ROOT="out/prepublish/${version}/lua-opencv${suffix}" node scripts/test.js --Release "$@" || return $?
        done
    done
}

function prepublish_test_build_wsl() {
    local script='
source scripts/wsl_init.sh || exit $?
cd out/prepublish/${version}/lua-opencv${suffix} && ./luarocks/luarocks install --deps-only samples/samples-scm-1.rockspec && \
cd "${sources}" && \
WORKSPACE_ROOT="out/prepublish/${version}/lua-opencv${suffix}" node scripts/test.js --Release'

    for arg in "$@"; do
        script="$script '$arg'"
    done

    # ecluded due to camera device missing
    script="$script '!02-video-capture-camera.lua' '!threshold_inRange.lua' '!objectDetection.lua'"

    for version in luajit-2.1 5.{4,3,2,1}; do
        for suffix in '' '-contrib'; do
            wsl -c "$script" || return $?
        done
    done
}

function prepublish_test_install_windows() {
    for version in luajit-2.1 5.{4,3,2,1}; do
        for suffix in '' '-contrib'; do
            bash -c '
version='${version}'
suffix='${suffix}'
projectDir="$PWD"

[ "${version:0:6}" == luajit ] && target=luajit || target=lua
[ "${version:0:6}" == luajit ] && binary=4.9.0luajit2.1

test -d out/test || mkdir out/test
test -d out/test/opencv/.git || git clone --depth 1 --branch 4.9.0 https://github.com/opencv/opencv.git out/test/opencv || exit $?

test -d out/test/${version} || mkdir out/test/${version} || exit $?
if [ -d out/test/${version}/lua-opencv${suffix} ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv${suffix} && \
    rm -rf .git && \
    git init && \
    git remote add origin "file://${projectDir}" && \
    git fetch origin && \
    git switch main --force && \
    git reset --hard HEAD && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "file://${projectDir}" out/test/${version}/lua-opencv${suffix} || exit $?
fi

source scripts/vcvars_restore_start.sh || exit $?

cd out/test/${version}/lua-opencv${suffix} || exit $?

./build.bat "-DLua_VERSION=${version}" --target ${target} --install && \
./build.bat "-DLua_VERSION=${version}" --target luarocks && \
time ./luarocks/luarocks.bat install "--server=${projectDir}/out/prepublish/server" opencv_lua${suffix} ${binary} && \
./luarocks/luarocks.bat install --deps-only samples/samples-scm-1.rockspec || exit $?

source "${projectDir}/scripts/vcvars_restore_end.sh" || exit $?

npm ci && \
node scripts/test.js --Release
' || return $?
        done
    done
}

function prepublish_test_install_wsl() {
    for version in luajit-2.1 5.{4,3,2,1}; do
        for suffix in '' '-contrib'; do
            wsl -c '
source scripts/wsl_init.sh || exit $?

[ "${version:0:6}" == luajit ] && target=luajit || target=lua
[ "${version:0:6}" == luajit ] && binary=4.9.0luajit2.1

test -d out/test || mkdir out/test
test -d out/test/opencv/.git || git clone --depth 1 --branch 4.9.0 https://github.com/opencv/opencv.git out/test/opencv || exit $?

test -d out/test/${version} || mkdir out/test/${version} || exit $?
if [ -d out/test/${version}/lua-opencv${suffix} ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv${suffix} && \
    rm -rf .git && \
    git init && \
    git remote add origin "file://${projectDir}" && \
    git fetch origin && \
    git switch main --force && \
    git reset --hard HEAD && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "file://${projectDir}" out/test/${version}/lua-opencv${suffix} || exit $?
fi

cd out/test/${version}/lua-opencv${suffix} || exit $?

./build.sh "-DLua_VERSION=${version}" --target ${target} --install && \
./build.sh "-DLua_VERSION=${version}" --target luarocks && \
time ./luarocks/luarocks install "--server=${projectDir}/out/prepublish/server" opencv_lua${suffix} ${binary} && \
./luarocks/luarocks install --deps-only samples/samples-scm-1.rockspec || exit $?

npm ci && \
node scripts/test.js --Release
' || return $?
        done
    done
}

function prepublish_test_source_windows() {
    for version in luajit-2.1; do
        for suffix in '' '-contrib'; do
            bash -c '
version='${version}'
suffix='${suffix}'
projectDir="$PWD"

[ "${version:0:6}" == luajit ] && target=luajit || target=lua

test -d out/test/${version} || mkdir out/test/${version} || exit $?
test -d out/test/${version}/lua-project || mkdir out/test/${version}/lua-project || exit $?

if [ -d out/test/${version}/lua-opencv${suffix} ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv${suffix} && \
    git remote set-url origin "file://${projectDir}" && \
    git reset --hard HEAD && \
    git clean -fd && \
    git pull --force && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "file://${projectDir}" out/test/${version}/lua-opencv${suffix} || exit $?
fi

PWD_BCK="$PWD" && \
cd out/test/${version}/lua-opencv${suffix} && \
./build.bat "-DLua_VERSION=${version}" --target ${target} --install && \
./build.bat "-DLua_VERSION=${version}" --target luarocks && \
cd "${PWD_BCK}" || exit $?

cd "${PWD_BCK}" || exit $?
QUOTE="'"'"'"
DOUBLE_QUOTE='"'"'"'"'"'
luarocks="$(sed -nre "s/^.*[${DOUBLE_QUOTE}${QUOTE}]([^${DOUBLE_QUOTE}${QUOTE}]+)[${DOUBLE_QUOTE}${QUOTE}] --project-tree .+$/\1/p" out/test/${version}/lua-opencv${suffix}/luarocks/luarocks.bat)"
cd out/test/${version}/lua-project && \
"$luarocks" init --lua-versions "5.1,5.2,5.3,5.4" && \
time ./luarocks.bat install "--server=${projectDir}/out/prepublish/server" opencv_lua${suffix} 4.9.0 && \
source "${PWD_BCK}/scripts/vcvars_restore_start.sh" && \
./luarocks.bat install --deps-only ../lua-opencv${suffix}/samples/samples-scm-1.rockspec && \
source "${PWD_BCK}/scripts/vcvars_restore_end.sh" && \
LUAROCKS_BINDIR="$PWD" node ../lua-opencv${suffix}/scripts/test.js --Release
' || return $?
        done
    done
}

function prepublish_test_source_wsl() {
    for version in luajit-2.1; do
        for suffix in '' '-contrib'; do
            wsl -c '
source scripts/wsl_init.sh || exit $?

[ "${version:0:6}" == luajit ] && target=luajit || target=lua

test -d out/test/${version} || mkdir out/test/${version} || exit $?
test -d out/test/${version}/lua-project || mkdir out/test/${version}/lua-project || exit $?

if [ -d out/test/${version}/lua-opencv${suffix} ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv${suffix} && \
    git remote set-url origin "file://${projectDir}" && \
    git reset --hard HEAD && \
    git clean -fd && \
    git pull --force && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "file://${projectDir}" out/test/${version}/lua-opencv${suffix} || exit $?
fi

PWD_BCK="$PWD" && \
cd out/test/${version}/lua-opencv${suffix} && \
./build.sh "-DLua_VERSION=${version}" --target ${target} --install && \
./build.sh "-DLua_VERSION=${version}" --target luarocks && \
cd "${PWD_BCK}" || exit $?
QUOTE="'"'"'"
DOUBLE_QUOTE='"'"'"'"'"'
luarocks="$(sed -nre "s/^.*[${DOUBLE_QUOTE}${QUOTE}]([^${DOUBLE_QUOTE}${QUOTE}]+)[${DOUBLE_QUOTE}${QUOTE}] --project-tree .+$/\1/p" out/test/${version}/lua-opencv${suffix}/luarocks/luarocks)"
cd out/test/${version}/lua-project && \
"$luarocks" init --lua-versions "5.1,5.2,5.3,5.4" && \
./luarocks config --scope project cmake_generator Ninja && \
./luarocks config --scope project cmake_build_args -- -j$(( $(nproc) - 2 )) && \
time ./luarocks install "--server=${projectDir}/out/prepublish/server" opencv_lua${suffix} 4.9.0 && \
./luarocks install --deps-only ../lua-opencv${suffix}/samples/samples-scm-1.rockspec && \
LUAROCKS_BINDIR="$PWD" node ../lua-opencv${suffix}/scripts/test.js --Release
' || return $?
        done
    done
}

function publish() {
    ./luarocks/luarocks.bat upload out/prepublish/server/opencv_lua-4.9.0-${DIST_VERSION}.rockspec --api-key=${LUA_ROCKS_API_KEY} && \
    ./luarocks/luarocks.bat upload out/prepublish/server/opencv_lua-contrib-4.9.0-${DIST_VERSION}.rockspec --api-key=${LUA_ROCKS_API_KEY} && \
    echo "Upload the content of $(cygpath -w "$PWD/out/prepublish/server") to github"
}

function tidy() {
    node ../${ESLINT_CONFIG_PROJECT}/node_modules/eslint/bin/eslint.js --config=../${ESLINT_CONFIG_PROJECT}/.eslintrc --fix 'generator/**/*.js' 'scripts/*.js'
}

function doctoc() {
    node node_modules/doctoc/doctoc.js *.md docs/*.md
}

function force_push() {
    git push --all --follow-tags origin --force && git push --all --follow-tags github --force
}

function new_version() {
    npm version patch
}

function new_version_rollback() {
    local msg version

    local continue=1
    while [ $continue -eq 1 ]; do
        continue=0

        msg="$(git log -1 --pretty=format:%s)"

        if [ "v$msg" == "$(git tag -l "v$msg")" ];  then
            git tag -d "v$msg" || return $?
            continue=1
        fi

        if [ "$msg" == "$(node -pe "require('./package').version")" ]; then
            git reset --hard HEAD~1 || return $?
            continue=1
        fi
    done

    set_url_github
}

function set_url() {
    if [ ${SKIP_SET_URL:-0} -eq 1 ]; then
        return
    fi

    local url="$1"
    sed -r 's#url = "[^"]+"#url = "'$url'"#' -i luarocks/opencv_lua-scm-1.rockspec && \
    git add luarocks/opencv_lua-scm-1.rockspec && \
    git commit --amend --no-edit
}

function set_url_github() {
    set_url "git+https://github.com/smbape/lua-opencv.git"
}

function set_url_windows() {
    set_url "git+file://$(cygpath -m "$PWD")"
}

function set_url_wsl() {
    set_url "git+file:///mnt$(cygpath -u "$PWD")"
}

function build_clean_windows() {
    find out/prepublish/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
}

function build_clean_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
find out/prepublish/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
'
}

function test_debug_windows() {
    node scripts/test.js --Debug "$@"
}

function test_debug_wsl() {
    local script='
source scripts/wsl_init.sh || exit $?
node scripts/test.js --Debug'

    for arg in "$@"; do
        script="$script '$arg'"
    done

    wsl -c "$script"
}

function build_debug_clean_windows() {
    find out/build/x64-Debug/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
}

function build_debug_clean_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
find out/build/Linux-GCC-Debug/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
'
}

function build_debug_windows() {
    time ./build.bat -d "-DLua_VERSION=luajit-2.1" --install --target luajit "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=C:/vcpkg/scripts/buildsystems/vcpkg.cmake" && \
    time ./build.bat -d "-DLua_VERSION=luajit-2.1" --install "-DBUILD_contrib:BOOL=ON" "-DWITH_FREETYPE:BOOL=ON" "-DENABLE_EXPERIMENTAL_WIDE_CHAR:BOOL=ON" "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
}

function build_debug_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
time ./build.sh -d "-DLua_VERSION=luajit-2.1" --install --target luajit && \
time ./build.sh -d "-DLua_VERSION=luajit-2.1" --install "-DBUILD_contrib:BOOL=ON" "-DWITH_FREETYPE:BOOL=ON"
'
}

function prepublish_windows() {
    time DIST_VERSION=${DIST_VERSION} node scripts/prepublish.js --pack && \
    time ./build.bat "-DLua_VERSION=luajit-2.1" --target luajit --install && \
    time ./build.bat "-DLua_VERSION=luajit-2.1" --target luarocks
}

function prepublish_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?

time LUAROCKS_SERVER="$projectDir/out/prepublish/server" node scripts/prepublish.js --pack && \
time ./build.sh "-DLua_VERSION=luajit-2.1" --target luajit --install && \
time ./build.sh "-DLua_VERSION=luajit-2.1" --target luarocks
'
}

function use_luajit_opencv_lua_modules() {
    rm -rf luarocks/lua_modules && \
    cmd.exe //c mklink //j "$(cygpath -w "$PWD/luarocks/lua_modules")" "$(cygpath -w "$PWD/out/prepublish/luajit-2.1/lua-opencv/luarocks/lua_modules")"
}

function use_luajit_opencv_lua_contrib_modules() {
    rm -rf luarocks/lua_modules && \
    cmd.exe //c mklink //j "$(cygpath -w "$PWD/luarocks/lua_modules")" "$(cygpath -w "$PWD/out/prepublish/luajit-2.1/lua-opencv-contrib/luarocks/lua_modules")"
}

function compile_debug_windows() {
    local file="$1"
    compile_debug_strict_windows "opencv_lua/CMakeFiles/opencv_lua.dir/__/${file}.obj"
}

function compile_debug_wsl() {
    local file="$1"
    compile_debug_strict_wsl "opencv_lua/CMakeFiles/opencv_lua.dir/__/${file}.o"
}

function compile_debug_strict_windows() {
    local file="$1"
    bash -c "cd out/build/x64-Debug/ && ninja ${file}.obj"
}

function compile_debug_strict_wsl() {
    local file="$1"
    wsl -c "source scripts/wsl_init.sh && cd out/build/Linux-GCC-Debug/ && ninja ${file}.o"
}

function build_windows() {
    set_url_windows && \
    new_version && \
    time prepublish_windows && \
    new_version_rollback && \
    use_luajit_opencv_lua_modules
}

function build_wsl() {
    set_url_wsl && \
    new_version && \
    time prepublish_wsl && \
    new_version_rollback
}

function build_contrib_custom_windows() {
    local projectDir="$PWD"
    local BUILD_DIR="/d/opencv-lua-custom/build"

    export PATH="${projectDir}/out/install/x64-Release/bin:$PATH"
    export PATH="${projectDir}/out/build.luaonly/x64-Release/luarocks/luarocks-prefix/src/luarocks:$PATH"

    mkdir -p "${BUILD_DIR}" && \
    cd "${BUILD_DIR}" || return $?

    if [ -d lua-opencv ]; then
        cd lua-opencv && \
        git remote set-url origin "file://${projectDir}" && \
        git reset --hard HEAD && \
        git pull --force || return $?
    else
        git clone "file://${projectDir}" lua-opencv && \
        cd lua-opencv || return $?
    fi

    cp -f luarocks/opencv_lua-scm-1.rockspec opencv_lua-contrib-custom-scm-1.rockspec && \
    sed -e 's/package = "opencv_lua"/package = "opencv_lua-contrib-custom"/' -i opencv_lua-contrib-custom-scm-1.rockspec && \
    sed -e 's@LUA_INCDIR = "\$(LUA_INCDIR)",@LUA_INCDIR = "\$(LUA_INCDIR)",\n      BUILD_contrib = "ON",\n      CMAKE_TOOLCHAIN_FILE = "C:/vcpkg/scripts/buildsystems/vcpkg.cmake",\n      ENABLE_EXPERIMENTAL_WIDE_CHAR = "ON",\n      WITH_FREETYPE = "ON",@' -i opencv_lua-contrib-custom-scm-1.rockspec && \
    cd luarocks && \
    luarocks --lua-version "5.1" --lua-dir "$(cygpath -w "$(dirname "$(dirname "$(command -v luajit.exe)")")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
    cd .. && \
    ./luarocks/luarocks.bat make opencv_lua-contrib-custom-scm-1.rockspec && \
    cd luarocks && \
    cmake "-DLUAROCKS_EXE=$(command -v luarocks.exe)" "-DLUA_BINDIR=$(cygpath -w "$(dirname "$(command -v luajit.exe)")")" "-DLUA_INTERPRETER_NAME=luajit.exe" "-DABIVER=5.1" "-DEXTNAME=.bat" -P luarocks-init.cmake && \
    cd .. && \
    LUAROCKS_SERVER="/d/opencv-lua-custom/server" DIST_VERSION=1 ROCKSPEC=opencv_lua-contrib-custom-scm-1.rockspec node --trace-uncaught scripts/pack.js && \
    cd "${projectDir}"
}

function build_contrib_custom_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
BUILD_DIR="${sources}/../opencv-lua-custom/build"

export PATH="${sources}/out/install/Linux-GCC-Release/bin:$PATH"
export PATH="${sources}/out/build.luaonly//Linux-GCC-Release/luarocks/luarocks-prefix/src/luarocks-build/bin:$PATH"

mkdir -p "${BUILD_DIR}" && \
cd "${BUILD_DIR}" || exit $?

if [ -d lua-opencv ]; then
    cd lua-opencv && \
    git remote set-url origin "file://${projectDir}" && \
    git reset --hard HEAD && \
    git pull --force || exit $?
else
    git clone "file://${projectDir}" lua-opencv && \
    cd lua-opencv || exit $?
fi

cp -f luarocks/opencv_lua-scm-1.rockspec opencv_lua-contrib-custom-scm-1.rockspec && \
sed -e '"'"'s/package = "opencv_lua"/package = "opencv_lua-contrib-custom"/'"'"' -i opencv_lua-contrib-custom-scm-1.rockspec && \
sed -e '"'"'s/LUA_INCDIR = "\$(LUA_INCDIR)",/LUA_INCDIR = "\$(LUA_INCDIR)",\n      BUILD_contrib = "ON",\n      WITH_FREETYPE = "ON",/'"'"' -i opencv_lua-contrib-custom-scm-1.rockspec && \
cd luarocks && \
luarocks --lua-version "5.1" --lua-dir "$(dirname "$(dirname "$(command -v luajit)")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
luarocks config --scope project cmake_generator Ninja && \
luarocks config --scope project cmake_build_args -- -j$(( $(nproc) - 2 )) && \
cd .. && \
./luarocks/luarocks make opencv_lua-contrib-custom-scm-1.rockspec && \
LUAROCKS_SERVER="${sources}/../opencv-lua-custom/server" DIST_VERSION=1 ROCKSPEC=opencv_lua-contrib-custom-scm-1.rockspec node --trace-uncaught scripts/pack.js
'
}

function copy_samples_windows() {
    for version in luajit-2.1 5.{4,3,2,1}; do
        if [ "${version:0:6}" != luajit ]; then
            robocopy out/prepublish/{luajit-2.1,${version}}/lua-opencv/samples //MT //E //FFT //R:1 //W:1 //Z //SL //XJD //XJF //XA:HS //FP //NP //NDL //MIR //ETA || return $?
        fi
        robocopy out/prepublish/{luajit-2.1/lua-opencv,${version}/lua-opencv-contrib}/samples //MT //E //FFT //R:1 //W:1 //Z //SL //XJD //XJF //XA:HS //FP //NP //NDL //MIR //ETA || return $?
    done
}

function copy_samples_wsl() {
    for version in luajit-2.1 5.{4,3,2,1}; do
        wsl -c '
source scripts/wsl_init.sh || exit $?

if [ "${version:0:6}" != luajit ]; then
    rsync -t --delete -v -r -l out/prepublish/{luajit-2.1,${version}}/lua-opencv/samples/ || exit $?
fi
rsync -t --delete -v -r -l out/prepublish/{luajit-2.1/lua-opencv,${version}/lua-opencv-contrib}/samples/ || exit $?
'
    done
}

function build_full() {
    build_windows && \
    build_wsl && \
    build_debug_windows && \
    build_debug_wsl && \
    build_contrib_custom_windows && \
    build_contrib_custom_wsl
}
