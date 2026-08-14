#!/usr/bin/env bash

if [ "$workspaceHash" != "c0ef0985-b598-4736-af7d-1776141f784c" ]; then
export PATH="${PATH//\/mnt\/*:/}"

workspaceHash=c0ef0985-b598-4736-af7d-1776141f784c
projectDir="$(wslpath -w "$PWD" | sed -e "s#\(.*\)#/mnt/\L\1#" -e "s#\\\\#/#g" -e "s#:##")" # If docker is installed, PWD will starts with /mnt/wsl/docker-desktop-bind-mounts/
projectDirName=$(basename "$projectDir")
sourceDir="$HOME/.vs/${projectDirName}"

source "${projectDir}/scripts/tasks.sh" && open_git_project "file://${projectDir}" "${sourceDir}" || exit $?

rsync -t -v -r --include='**/.gitignore' --exclude='/.git' --filter=':- .gitignore' --delete-after \
    "${projectDir}/" "${sourceDir}" || exit $?

for ifile in $(git --git-dir "${projectDir}/.git" ls-files 2>&1); do
    touch -c -r "${projectDir}/${ifile}" "${sourceDir}/${ifile}"
done

export PATH="/snap/bin:$PATH"
export workspaceHash projectDir sourceDir
else
source "${projectDir}/scripts/tasks.sh" && open_git_project "file://${projectDir}" "${sourceDir}" || exit $?
fi
