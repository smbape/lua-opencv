#!/usr/bin/env bash

if command -v cl &> /dev/null && [ ${#PATH_BCK} -ne 0 ]; then
    export PATH="${PATH_BCK}"
    unset PATH_BCK
fi
