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
# build
# ================================
bash -c 'source scripts/tasks.sh && prepublish_stash_push'

bash -c 'source scripts/tasks.sh && new_version_rollback && build_full'

bash -c 'source scripts/tasks.sh && prepublish_stash_pop'


# ================================
# Windows README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && test_prepublished_binary_windows'

# ================================
# WSL README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && test_prepublished_binary_wsl'

# ================================
# Docker images README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-ubuntu-22.04 ubuntu:22.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-debian-11 debian:11 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-fedora-39 fedora:39 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-almalinux-9 amd64/almalinux:9 -- object_detection.lua'


# ================================
# Windows README.md install source rock
# ================================
bash -c 'source scripts/tasks.sh && test_prepublished_source_windows'

# ================================
# WSL README.md install source rock
# ================================
bash -c 'source scripts/tasks.sh && test_prepublished_source_wsl'

# ================================
# Docker images README.md install source rock
# ================================
bash -c 'source scripts/tasks.sh && test_prepublished_source_debian test-source-ubuntu-22.04 ubuntu:22.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_source_debian test-source-debian-11 debian:11 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_source_fedora test-source-fedora-39 fedora:39 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_source_fedora test-source-almalinux-9 amd64/almalinux:9 -- object_detection.lua'


# ================================
# generate doctoc
# ================================
cp -f out/prepublish/luajit-2.1/opencv_lua/docs/docs.md ./docs/
cp -f out/prepublish/luajit-2.1/opencv_lua/generator/ids.json ./generator/
bash -c 'source scripts/tasks.sh && doctoc'


# ================================
# prepublish the new version
# ================================
bash -c 'source scripts/tasks.sh && new_version_rollback && new_version && push_all && time prepublish_windows && time prepublish_manylinux'


# ================================
# publish
# ================================
bash -c 'source scripts/tasks.sh && publish'


# ================================
# restore stash
# ================================
git stash pop
