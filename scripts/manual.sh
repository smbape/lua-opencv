#!/usr/bin/env bash

# ================================
# stash for release
# ================================
git stash push --include-untracked


# ================================
# tidy
# ================================
node ../${ESLINT_CONFIG_PROJECT}/node_modules/eslint/bin/eslint.js --config=../${ESLINT_CONFIG_PROJECT}/.eslintrc --fix 'generator/**/*.js' 'scripts/*.js'


# ================================
# generate doctoc
# ================================
node node_modules/doctoc/doctoc.js README.md


# ================================
# git commit
# ================================
npm version patch && git push --all --follow-tags origin --force && git push --all --follow-tags github --force


# ================================
# build
# ================================
for version in luajit-2.1 5.{4,3,2,1}; do
    bash -c "cd out/prepublish/${version}/lua-opencv && git stash push --include-untracked --all -- samples"
    wsl -e bash -li -c 'source scripts/wsl_init.sh; cd out/prepublish/'${version}'/lua-opencv && git stash push --include-untracked --all -- samples'
done

time DIST_VERSION=2 node scripts/prepublish.js --pack 2>&1 | tee prepublish_win.log && \
time DIST_VERSION=2 WSLENV=DIST_VERSION/u wsl -e bash -li -c './scripts/wsl_prepublish.sh --pack' 2>&1 | tee prepublish_linux.log

for version in luajit-2.1 5.{4,3,2,1}; do
    bash -c "cd out/prepublish/${version}/lua-opencv && git reset --hard HEAD && git stash pop"
    wsl -e bash -li -c 'source scripts/wsl_init.sh; cd out/prepublish/'${version}'/lua-opencv && git reset --hard HEAD && git stash pop'
done


# ================================
# Windows prepublish check
# ================================
for version in luajit-2.1 5.{4,3,2,1}; do
    bash -c '
source scripts/vcvars_restore_start.sh || exit $?
cd out/prepublish/'${version}'/lua-opencv && ./luarocks/luarocks.bat install --deps-only samples/samples-scm-1.rockspec
' && \
    WORKSPACE_ROOT="out/prepublish/${version}/lua-opencv" node scripts/test.js --Release
done

# ================================
# Windows README.md samples check
# ================================
for version in luajit-2.1 5.{4,3,2,1}; do
    bash -c '
version='${version}'
projectDir="$PWD"

[ "${version:0:6}" == luajit ] && target=luajit || target=lua
[ "${version:0:6}" == luajit ] && binary=4.9.0luajit2.1

test -d out/test || mkdir out/test
test -d out/test/opencv/.git || git clone --depth 1 --branch 4.9.0 https://github.com/opencv/opencv.git out/test/opencv || exit $?

test -d out/test/${version} || mkdir out/test/${version} || exit $?
if [ -d out/test/${version}/lua-opencv ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv && \
    rm -rf .git && \
    git init && \
    git remote add origin "file://${projectDir}" && \
    git fetch origin && \
    git switch main --force && \
    git reset --hard HEAD && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "file://${projectDir}" out/test/${version}/lua-opencv || exit $?
fi

source scripts/vcvars_restore_start.sh || exit $?

cd out/test/${version}/lua-opencv || exit $?

./build.bat --target ${target} "-DLua_VERSION=${version}" --install && \
./build.bat --target luarocks && \
time ./luarocks/luarocks.bat install "--server=${projectDir}/out/prepublish/server" opencv_lua ${binary} && \
./luarocks/luarocks.bat install --deps-only samples/samples-scm-1.rockspec || exit $?

source "${projectDir}/scripts/vcvars_restore_end.sh" || exit $?

npm ci && \
node scripts/test.js --Release
'
done


# ================================
# Windows README.md install source rock
# ================================
for version in luajit-2.1; do
    bash -c '
version='${version}'
projectDir="$PWD"

[ "${version:0:6}" == luajit ] && target=luajit || target=lua

test -d out/test/${version} || mkdir out/test/${version} || exit $?
test -d out/test/${version}/lua-project || mkdir out/test/${version}/lua-project || exit $?

if [ -d out/test/${version}/lua-opencv ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv && \
    git remote set-url origin "file://${projectDir}" && \
    git reset --hard HEAD && \
    git clean -fd && \
    git pull && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "${projectDir}" out/test/${version}/lua-opencv || exit $?
fi

PWD_BCK="$PWD" && \
cd out/test/${version}/lua-opencv && \
./build.bat --target ${target} "-DLua_VERSION=${version}" --install && \
./build.bat --target luarocks && \
cd "${PWD_BCK}" || exit $?

cd "${PWD_BCK}" || exit $?
QUOTE="'"'"'"
DOUBLE_QUOTE='"'"'"'"'"'
luarocks="$(sed -nre "s/^.*[${DOUBLE_QUOTE}${QUOTE}]([^${DOUBLE_QUOTE}${QUOTE}]+)[${DOUBLE_QUOTE}${QUOTE}] --project-tree .+$/\1/p" out/test/${version}/lua-opencv/luarocks/luarocks.bat)"
cd out/test/${version}/lua-project && \
"$luarocks" init --lua-versions "5.1,5.2,5.3,5.4" && \
time ./luarocks.bat install "--server=${projectDir}/out/prepublish/server" opencv_lua 4.9.0 && \
source "${PWD_BCK}/scripts/vcvars_restore_start.sh" && \
./luarocks.bat install --deps-only ../lua-opencv/samples/samples-scm-1.rockspec && \
source "${PWD_BCK}/scripts/vcvars_restore_end.sh" && \
LUAROCKS_BINDIR="$PWD" node ../lua-opencv/scripts/test.js --Release
'
done


# ================================
# WSL prepublish check
# ================================
for version in luajit-2.1 5.{4,3,2,1}; do
    version=${version} WSLENV=version/u wsl -e bash -li -c '
source scripts/wsl_init.sh || exit $?
cd out/prepublish/${version}/lua-opencv && ./luarocks/luarocks install --deps-only samples/samples-scm-1.rockspec && \
cd "${sources}" && \
WORKSPACE_ROOT="out/prepublish/${version}/lua-opencv" node scripts/test.js --Release
'
done


# ================================
# WSL README.md samples check
# ================================
for version in luajit-2.1 5.{4,3,2,1}; do
    version=${version} WSLENV=version/u wsl -e bash -li -c '
source scripts/wsl_init.sh || exit $?

[ "${version:0:6}" == luajit ] && target=luajit || target=lua
[ "${version:0:6}" == luajit ] && binary=4.9.0luajit2.1

test -d out/test || mkdir out/test
test -d out/test/opencv/.git || git clone --depth 1 --branch 4.9.0 https://github.com/opencv/opencv.git out/test/opencv || exit $?

test -d out/test/${version} || mkdir out/test/${version} || exit $?
if [ -d out/test/${version}/lua-opencv ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv && \
    rm -rf .git && \
    git init && \
    git remote add origin "file://${projectDir}" && \
    git fetch origin && \
    git switch main --force && \
    git reset --hard HEAD && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "${projectDir}" out/test/${version}/lua-opencv || exit $?
fi

cd out/test/${version}/lua-opencv || exit $?

./build.sh --target ${target} "-DLua_VERSION=${version}" --install && \
./build.sh --target luarocks && \
time ./luarocks/luarocks install "--server=${projectDir}/out/prepublish/server" opencv_lua ${binary} && \
./luarocks/luarocks install --deps-only samples/samples-scm-1.rockspec || exit $?

npm ci && \
node scripts/test.js --Release
'
done


# ================================
# WSL README.md install source rock
# ================================
for version in luajit-2.1; do
    version=${version} WSLENV=version/u wsl -e bash -li -c '
source scripts/wsl_init.sh || exit $?

[ "${version:0:6}" == luajit ] && target=luajit || target=lua

test -d out/test/${version} || mkdir out/test/${version} || exit $?
test -d out/test/${version}/lua-project || mkdir out/test/${version}/lua-project || exit $?

if [ -d out/test/${version}/lua-opencv ]; then
    PWD_BCK="$PWD" && \
    cd out/test/${version}/lua-opencv && \
    git remote set-url origin "file://${projectDir}" && \
    git reset --hard HEAD && \
    git clean -fd && \
    git pull && \
    cd "${PWD_BCK}" || exit $?
else
    git clone "${projectDir}" out/test/${version}/lua-opencv || exit $?
fi

PWD_BCK="$PWD" && \
cd out/test/${version}/lua-opencv && \
./build.sh --target ${target} "-DLua_VERSION=${version}" --install && \
./build.sh --target luarocks && \
cd "${PWD_BCK}" || exit $?
QUOTE="'"'"'"
DOUBLE_QUOTE='"'"'"'"'"'
luarocks="$(sed -nre "s/^.*[${DOUBLE_QUOTE}${QUOTE}]([^${DOUBLE_QUOTE}${QUOTE}]+)[${DOUBLE_QUOTE}${QUOTE}] --project-tree .+$/\1/p" out/test/${version}/lua-opencv/luarocks/luarocks)"
cd out/test/${version}/lua-project && \
"$luarocks" init --lua-versions "5.1,5.2,5.3,5.4" && \
./luarocks config --scope project cmake_generator Ninja && \
./luarocks config --scope project cmake_build_args -- -j$(( $(nproc) - 2 )) && \
time ./luarocks install "--server=${projectDir}/out/prepublish/server" opencv_lua 4.9.0 && \
./luarocks install --deps-only ../lua-opencv/samples/samples-scm-1.rockspec && \
LUAROCKS_BINDIR="$PWD" node ../lua-opencv/scripts/test.js --Release
'
done


# ================================
# publish
# ================================
bash -c 'DIST_VERSION=2; ./luarocks/luarocks.bat upload out/prepublish/server/opencv_lua-4.9.0-${DIST_VERSION}.rockspec --api-key=${LUA_ROCKS_API_KEY}'


# ================================
# restore stash
# ================================
git stash pop
