#!/usr/bin/env bash

PATH_BCK="${PATH}"

PATH="${PATH//$HOME\/.local\/bin:/}"
PATH="${PATH//$HOME\/.local\/sbin:/}"
PATH="${PATH//$HOME\/bin:/}"
PATH="${PATH//$HOME\/sbin:/}"
PATH="${PATH//\/usr\/local\/bin:/}"
PATH="${PATH//\/usr\/local\/sbin:/}"
PATH="${PATH//\/usr\/bin:/}"
PATH="${PATH//\/usr\/sbin:/}"
PATH="${PATH//\/mingw64\/local\/bin:/}"
PATH="${PATH//\/mingw64\/local\/sbin:/}"
PATH="${PATH//\/mingw64\/bin:/}"
PATH="${PATH//\/mingw64\/sbin:/}"
PATH="${PATH//\/mingw32\/local\/bin:/}"
PATH="${PATH//\/mingw32\/local\/sbin:/}"
PATH="${PATH//\/mingw32\/bin:/}"
PATH="${PATH//\/mingw32\/sbin:/}"
PATH="${PATH//\/bin:/}"
PATH="${PATH//\/usr\/bin\/vendor_perl:/}"
PATH="${PATH//\/usr\/bin\/core_perl:/}"

export PATH
