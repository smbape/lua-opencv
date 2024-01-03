#!/bin/bash

export PATH="${PATH//\/mnt\/*:/}"

projectDir="$PWD"
projectDirName=$(basename "$projectDir")
sources="$HOME/.vs/${projectDirName}/${workspaceHash}/src"

cd "${sources}" || exit $?

if [ -d .git ]; then
    git remote set-url origin "$projectDir"
else
    git init
    git remote add origin "$projectDir"
fi

git reset --hard HEAD && git clean -fd && git pull || exit $?

rsync -t --delete -v -r \
    --exclude=.git \
    --exclude=.idea \
    --exclude=.venv \
    --exclude=.vs \
    --exclude='*.rock' \
    --exclude='*.rockspec' \
    --exclude=build.luarocks \
    --exclude=.luarocks \
    --exclude=luarocks/lua.bat \
    --exclude=luarocks/lua \
    --exclude=luarocks/luarocks.bat \
    --exclude=luarocks/luarocks \
    --exclude=luarocks/lua_modules \
    --exclude=luarocks/.luarocks \
    --exclude=generated \
    --exclude=node_modules \
    --exclude=out \
    "${projectDir}/" "${sources}" || exit $?

LUAROCKS_SERVER="$projectDir/out/prepublish/server" node scripts/prepublish.js "$@"

$HOME/.local/bin
$HOME/.local/sbin
$HOME/bin
$HOME/sbin
/usr/local/bin
/usr/local/sbin
/usr/bin
/usr/sbin
/mingw64/local/bin
/mingw64/local/sbin
/mingw64/bin
/mingw64/sbin
/mingw32/local/bin
/mingw32/local/sbin
/mingw32/bin
/mingw32/sbin
/bin
/usr/bin/vendor_perl
/usr/bin/core_perl

PATH="${PATH//$HOME\/.local\/bin:/}"
PATH="${PATH//$HOME\/.local\/sbin:/}"
PATH="${PATH//$HOME\/bin:/}"
PATH="${PATH//$HOME\/sbin:/}"
PATH="${PATH//\/usr\/local\/bin:/}"
PATH="${PATH//\/usr\/local\/sbin:/}"
PATH="${PATH//\/usr\/bin:/}"
PATH="${PATH//\/usr\/sbin:/}"
PATH="${PATH//\/mingw64\/local\/bin:/}"
PATH="${PATH//\/mingw64\/local\/sbin:/}"
PATH="${PATH//\/mingw64\/bin:/}"
PATH="${PATH//\/mingw64\/sbin:/}"
PATH="${PATH//\/mingw32\/local\/bin:/}"
PATH="${PATH//\/mingw32\/local\/sbin:/}"
PATH="${PATH//\/mingw32\/bin:/}"
PATH="${PATH//\/mingw32\/sbin:/}"
PATH="${PATH//\/bin:/}"
PATH="${PATH//\/usr\/bin\/vendor_perl:/}"
PATH="${PATH//\/usr\/bin\/core_perl:/}"
