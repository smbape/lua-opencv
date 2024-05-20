#!/usr/bin/env bash

export PATH="${PATH//\/mnt\/*:/}"

workspaceHash=53b58a2f-f3e5-480b-8803-dc266ac326de
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
    --exclude="*.rock" \
    --exclude="*.rockspec" \
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
    --exclude=opencv_extra \
    --exclude=out \
    "${projectDir}/" "${sources}" || exit $?
