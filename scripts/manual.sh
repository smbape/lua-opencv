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

bash -c 'source scripts/tasks.sh && SKIP_SET_URL=1 build_windows' && \
bash -c 'source scripts/tasks.sh && SKIP_SET_URL=1 build_wsl'

bash -c 'source scripts/tasks.sh && prepublish_stash_pop'


# ================================
# Windows prepublish check
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_build_windows'


# ================================
# Windows README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_install_windows'


# ================================
# Windows README.md install source rock
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_source_windows'


# ================================
# WSL prepublish check
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_build_wsl'


# ================================
# WSL README.md samples check
# ================================
bash -c 'source scripts/tasks.sh && prepublish_test_install_wsl'


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
