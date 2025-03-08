#!/usr/bin/env bash

if [ "$workspaceHash" != "c0ef0985-b598-4736-af7d-1776141f784c" ]; then
export PATH="${PATH//\/mnt\/*:/}"

workspaceHash=c0ef0985-b598-4736-af7d-1776141f784c
projectDir="$(wslpath -w "$PWD" | sed -e "s#\(.*\)#/mnt/\L\1#" -e "s#\\\\#/#g" -e "s#:##")" # If docker is installed, PWD will starts with /mnt/wsl/docker-desktop-bind-mounts/
projectDirName=$(basename "$projectDir")
sources="$HOME/.vs/${projectDirName}/${workspaceHash}/src"

source "${projectDir}/scripts/tasks.sh" && open_git_project "file://${projectDir}" "${sources}" || exit $?

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
export workspaceHash projectDir sources
else
source "${projectDir}/scripts/tasks.sh" && open_git_project "file://${projectDir}" "${sources}" || exit $?
fi
