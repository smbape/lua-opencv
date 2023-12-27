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

git reset --hard HEAD && git pull || exit $?

rsync -t --delete -v -r \
    --exclude=.vs \
    --exclude=.git '--exclude=*.rock' '--exclude=*.rockspec' \
    --exclude=build.luarocks \
    --exclude=luarocks/lua.bat \
    --exclude=luarocks/lua \
    --exclude=luarocks/luarocks.bat \
    --exclude=luarocks/luarocks \
    --exclude=luarocks/lua_modules \
    --exclude=luarocks/.luarocks \
    --exclude=generated \
    --exclude=node_modules \
    --exclude=out "${projectDir}/" \
    "${sources}" || exit $?

LUAROCKS_SERVER="$projectDir/out/prepublish/server" node scripts/prepublish.js "$@"
