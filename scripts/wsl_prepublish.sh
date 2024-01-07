#!/bin/bash

#Absolute path to this script
SCRIPT=$(readlink -f "$0")
#Absolute path this script is in
SCRIPTPATH=$(dirname "$SCRIPT")

source "${SCRIPTPATH}/wsl_init.sh" || exit $?
LUAROCKS_SERVER="$projectDir/out/prepublish/server" node scripts/prepublish.js "$@"
