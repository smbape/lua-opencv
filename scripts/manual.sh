#!/usr/bin/env bash

# Update checks:
#   - In every cmake files, replace CMAKE_SOURCE_DIR with OPENCV_SOURCE_DIR
#   - In every cmake files, replace CMAKE_BINARY_DIR with OPENCV_BINARY_DIR
#   - In every cmake files, replace IMPORTED with IMPORTED GLOBAL
#   - check changes in modules/core/include/opencv2/core/hal/interface.h
#   - check changes in modules/core/include/opencv2/core/mat.hpp
#   - check changes in python, *.py
#   - check DOCKER_IMAGE change in https://github.com/opencv/opencv-python/blob/4.x/.github/workflows/build_wheels_manylinux.yml and update tasks.sh DOCKER_IMAGE_MANY_LINUX_x86_64 DOCKER_IMAGE_MANY_LINUX_aarch64
#   - check "OS-specific components during CI builds" change in https://github.com/opencv/opencv-python/blob/4.x/setup.py


# ================================
# stash for release
# ================================
bash -c 'source scripts/tasks.sh && stash_push'


# ================================
# tidy
# ================================
bash -c 'source scripts/tasks.sh && tidy'


# ================================
# bump a new version
# ================================
bash -c 'source scripts/tasks.sh && new_version'


# ================================
# build
# ================================
bash -c 'source scripts/tasks.sh && prepublish_stash_push'

bash -c 'source scripts/tasks.sh && build_full'

bash -c 'source scripts/tasks.sh && prepublish_stash_pop'


# ================================
# Windows README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && test_debug_windows --bash > ./out/build/x64-Debug/test_all.sh' && \
./out/build/x64-Debug/test_all.sh && \
bash -c '
source scripts/tasks.sh && \
mkdir -p out/build/x64-Release && \
truncate -s 0 out/build/x64-Release/test_all.sh && \
test_prepublished_binary_windows --upgrade --bash \>\> $PWD/out/build/x64-Release/test_all.sh
' && \
./out/build/x64-Release/test_all.sh

# ================================
# WSL README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && test_debug_wsl --bash \> ./out/build/Linux-GCC-Debug/test_all.sh' && \
bash -c 'source scripts/tasks.sh && wsl -c "source scripts/wsl_init.sh && chmod +x ./out/build/Linux-GCC-Debug/test_all.sh && ./out/build/Linux-GCC-Debug/test_all.sh"'
bash -c '
source scripts/tasks.sh && \
wsl -c "
source scripts/wsl_init.sh && \
mkdir -p out/build/Linux-GCC-Release && \
truncate -s 0 out/build/Linux-GCC-Release/test_all.sh
" && \
test_prepublished_binary_wsl --upgrade --bash \>\> \$sourceDir/out/build/Linux-GCC-Release/test_all.sh' && \
bash -c 'source scripts/tasks.sh && wsl -c "source scripts/wsl_init.sh && chmod +x ./out/build/Linux-GCC-Release/test_all.sh && ./out/build/Linux-GCC-Release/test_all.sh"'

# ================================
# Docker images README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-ubuntu-22.04 ubuntu:22.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-ubuntu-24.04 ubuntu:24.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-ubuntu-24.04 ubuntu:26.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-debian-11 debian:11 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-debian-12 debian:12 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_debian test-binary-debian-13 debian:13 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-fedora-43 fedora:43 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-fedora-44 fedora:44 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-fedora-45 fedora:45 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-almalinux-8 amd64/almalinux:8 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-almalinux-9 amd64/almalinux:9 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_binary_fedora test-binary-almalinux-10 amd64/almalinux:10 -- object_detection.lua'


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
bash -c 'source scripts/tasks.sh && test_prepublished_source_debian test-source-ubuntu-26.04 ubuntu:26.04 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_source_debian test-source-debian-13 debian:13 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_source_fedora test-source-fedora-44 fedora:44 -- object_detection.lua'
bash -c 'source scripts/tasks.sh && test_prepublished_source_fedora test-source-almalinux-9 amd64/almalinux:9 -- object_detection.lua'


# ================================
# generate doctoc
# ================================
cp -f out/prepublish/build/opencv_lua/docs/docs.md ./docs/
cp -f out/prepublish/build/opencv_lua/generator/ids.json ./generator/
bash -c 'source scripts/tasks.sh && doctoc'


# ================================
# add modified docs to the new_version
# ================================
bash -c 'source scripts/tasks.sh && update_new_version'


# ================================
# prepublish the new version
# ================================
bash -c 'source scripts/tasks.sh && time prepublish_windows && time prepublish_manylinux'


# ================================
# publish
# ================================
bash -c 'source scripts/tasks.sh && push_all && publish'


# ================================
# restore stash
# ================================
git stash pop
