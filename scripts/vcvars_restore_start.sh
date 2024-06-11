#!/usr/bin/env bash

PATH_BCK="${PATH}"
export PATH="$(dirname "$(command -v cl)"):$PATH"
