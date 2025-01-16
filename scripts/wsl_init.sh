#!/usr/bin/env bash

export PATH="${PATH//\/mnt\/*:/}"

workspaceHash=53b58a2f-f3e5-480b-8803-dc266ac326de
projectDir="$(wslpath -w "$PWD" | sed -e "s#\(.*\)#/mnt/\L\1#" -e "s#\\\\#/#g" -e "s#:##")" # If docker is installed, PWD will starts with /mnt/wsl/docker-desktop-bind-mounts/
projectDirName=$(basename "$projectDir")
sources="$HOME/.vs/${projectDirName}/${workspaceHash}/src"

source "${projectDir}/scripts/tasks.sh" && open_git_project "file://${projectDir}" "${sources}"

rsync -t --delete -v -r \
    --exclude=.git \
    --exclude=.idea \
    --exclude=.venv \
    --exclude=.vs \
    --exclude="*.rock" \
    --exclude="*.rockspec" \
    --exclude="*.sublime-workspace" \
    --exclude="~$*" \
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
