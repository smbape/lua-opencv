#!/usr/bin/env bash

if command -v cl &> /dev/null; then
    # some executable, for example link, find, are expected to be cmd.exe native executables
    PATH_BCK="${PATH}"
    export PATH="$(dirname "$(command -v cl)"):$PATH"
fi
