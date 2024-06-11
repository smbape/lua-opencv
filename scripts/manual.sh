#!/usr/bin/env bash

# ================================
# stash for release
# ================================
bash -c 'source scripts/tasks.sh && stash_push'


# ================================
# tidy
# ================================
bash -c 'source scripts/tasks.sh && tidy'


# ================================
# generate doctoc
# ================================
bash -c 'source scripts/tasks.sh && doctoc'


# ================================
# git commit
# ================================
npm version patch && bash -c 'source scripts/tasks.sh && force_push'


# ================================
# build
# ================================
bash -c 'source scripts/tasks.sh && prepublish_stash_push'

bash -c 'source scripts/tasks.sh && build_windows' && \
bash -c 'source scripts/tasks.sh && build_manylinux'

bash -c 'source scripts/tasks.sh && prepublish_stash_pop'

cp -f out/prepublish/luajit-2.1/lua-opencv/docs/docs.md ./docs/
cp -f out/prepublish/luajit-2.1/lua-opencv/generator/ids.json ./generator/

# ================================
# Windows README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_binary_windows'

# ================================
# WSL README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_binary_wsl'

# ================================
# Docker images README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_binary_debian ubuntu-20.04 ubuntu:20.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_debian ubuntu-22.04 ubuntu:22.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_debian ubuntu-24.04 ubuntu:24.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_debian debian-10 debian:10 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_debian debian-11 debian:11 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_debian debian-12 debian:12 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_fedora almalinux-8 amd64/almalinux:8 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_fedora fedora-38 fedora:38 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_fedora fedora-39 fedora:39 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && prepublish_test_binary_fedora fedora-40 fedora:40 -- object_detection.lua'


# ================================
# Windows README.md install source rock
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_source_windows'

# ================================
# WSL README.md install source rock
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_source_wsl'


# ================================
# publish
# ================================
bash -c 'source scripts/tasks.sh && publish'


# ================================
# restore stash
# ================================
git stash pop
