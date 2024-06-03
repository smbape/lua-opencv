#!/usr/bin/env bash

export PATH="${PATH//\/mnt\/*:/}"

if ! command -v node &>/dev/null; then
    export NVS_HOME="$HOME/.nvs"
    git clone https://github.com/jasongin/nvs "$NVS_HOME" && \
    . "$NVS_HOME/nvs.sh" install && \
    nvs add lts && \
    nvs use lts && \
    nvs link lts
fi

workspaceHash=53b58a2f-f3e5-480b-8803-dc266ac326de
projectDir="$PWD"
projectDirName=$(basename "$projectDir")
sources="$HOME/.vs/${projectDirName}/${workspaceHash}/src"

if [ -d "${sources}/.git" ]; then
    cd "${sources}" && \
    git remote set-url origin "file://$projectDir" || exit $?
else
    git clone "file://$projectDir" "${sources}" && \
    cd "${sources}" && \
    git config pull.rebase true || exit $?
fi

git reset --hard HEAD && git clean -fd && git pull --force || exit $?

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
    --exclude=samples/dnn/object_detection/models \
    "${projectDir}/" "${sources}" || exit $?

export PATH="/snap/bin:$PATH"
