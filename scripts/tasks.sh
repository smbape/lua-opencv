#!/usr/bin/env bash

# ================================================================
#!/bin/sh
#
# Constants and functions for terminal colors.
# Author: Max Tsepkov <max@yogi.pw>

CLR_ESC="\033["

# All these variables has a function with the same name, but in lower case.
#
CLR_RESET=0             # reset all attributes to their defaults
CLR_RESET_UNDERLINE=24  # underline off
CLR_RESET_REVERSE=27    # reverse off
CLR_DEFAULT=39          # set underscore off, set default foreground color
CLR_DEFAULTB=49         # set default background color

CLR_BOLD=1              # set bold
CLR_BRIGHT=2            # set half-bright (simulated with color on a color display)
CLR_UNDERSCORE=4        # set underscore (simulated with color on a color display)
CLR_REVERSE=7           # set reverse video

CLR_BLACK=30            # set black foreground
CLR_RED=31              # set red foreground
CLR_GREEN=32            # set green foreground
CLR_BROWN=33            # set brown foreground
CLR_BLUE=34             # set blue foreground
CLR_MAGENTA=35          # set magenta foreground
CLR_CYAN=36             # set cyan foreground
CLR_WHITE=37            # set white foreground

CLR_BLACKB=40           # set black background
CLR_REDB=41             # set red background
CLR_GREENB=42           # set green background
CLR_BROWNB=43           # set brown background
CLR_BLUEB=44            # set blue background
CLR_MAGENTAB=45         # set magenta background
CLR_CYANB=46            # set cyan background
CLR_WHITEB=47           # set white background

# General function to wrap string with escape sequence(s).
# Ex: clr_escape foobar $CLR_RED $CLR_BOLD
clr_escape() {
    local result="$1"
    until [ -z "$2" ]; do
        if ! [ $2 -ge 0 -a $2 -le 47 ] 2>/dev/null; then
            echo "clr_escape: argument \"$2\" is out of range" >&2 && return 1
        fi
        result="${CLR_ESC}${2}m${result}${CLR_ESC}${CLR_RESET}m"
        shift || break
    done

    echo -e "$result"
}

clr_reset()           { clr_escape "$1" $CLR_RESET;           }
clr_reset_underline() { clr_escape "$1" $CLR_RESET_UNDERLINE; }
clr_reset_reverse()   { clr_escape "$1" $CLR_RESET_REVERSE;   }
clr_default()         { clr_escape "$1" $CLR_DEFAULT;         }
clr_defaultb()        { clr_escape "$1" $CLR_DEFAULTB;        }
clr_bold()            { clr_escape "$1" $CLR_BOLD;            }
clr_bright()          { clr_escape "$1" $CLR_BRIGHT;          }
clr_underscore()      { clr_escape "$1" $CLR_UNDERSCORE;      }
clr_reverse()         { clr_escape "$1" $CLR_REVERSE;         }
clr_black()           { clr_escape "$1" $CLR_BLANK;           }
clr_red()             { clr_escape "$1" $CLR_RED;             }
clr_green()           { clr_escape "$1" $CLR_GREEN;           }
clr_brown()           { clr_escape "$1" $CLR_BROWN;           }
clr_blue()            { clr_escape "$1" $CLR_BLUE;            }
clr_magenta()         { clr_escape "$1" $CLR_MAGENTA;         }
clr_cyan()            { clr_escape "$1" $CLR_CYAN;            }
clr_white()           { clr_escape "$1" $CLR_WHITE;           }
clr_blackb()          { clr_escape "$1" $CLR_BLACKB;          }
clr_redb()            { clr_escape "$1" $CLR_REDB;            }
clr_greenb()          { clr_escape "$1" $CLR_GREENB;          }
clr_brownb()          { clr_escape "$1" $CLR_BROWNB;          }
clr_blueb()           { clr_escape "$1" $CLR_BLUEB;           }
clr_magentab()        { clr_escape "$1" $CLR_MAGENTAB;        }
clr_cyanb()           { clr_escape "$1" $CLR_CYANB;           }
clr_whiteb()          { clr_escape "$1" $CLR_WHITEB;          }

# Outputs colors table
clr_dump() {
    local T='gYw'

    echo -e "\n                 40m     41m     42m     43m     44m     45m     46m     47m";

    for FGs in '   0m' '   1m' '  30m' '1;30m' '  31m' '1;31m' \
               '  32m' '1;32m' '  33m' '1;33m' '  34m' '1;34m' \
               '  35m' '1;35m' '  36m' '1;36m' '  37m' '1;37m';
    do
        FG=${FGs// /}
        echo -en " $FGs \033[$FG  $T  "
        for BG in 40m 41m 42m 43m 44m 45m 46m 47m; do
            echo -en " \033[$FG\033[$BG  $T  \033[0m";
        done
        echo;
    done

    echo
    clr_bold "    Code     Function           Variable"
    echo \
'    0        clr_reset          $CLR_RESET
    1        clr_bold           $CLR_BOLD
    2        clr_bright         $CLR_BRIGHT
    4        clr_underscore     $CLR_UNDERSCORE
    7        clr_reverse        $CLR_REVERSE

    30       clr_black          $CLR_BLACK
    31       clr_red            $CLR_RED
    32       clr_green          $CLR_GREEN
    33       clr_brown          $CLR_BROWN
    34       clr_blue           $CLR_BLUE
    35       clr_magenta        $CLR_MAGENTA
    36       clr_cyan           $CLR_CYAN
    37       clr_white          $CLR_WHITE

    40       clr_blackb         $CLR_BLACKB
    41       clr_redb           $CLR_REDB
    42       clr_greenb         $CLR_GREENB
    43       clr_brownb         $CLR_BROWNB
    44       clr_blueb          $CLR_BLUEB
    45       clr_magentab       $CLR_MAGENTAB
    46       clr_cyanb          $CLR_CYANB
    47       clr_whiteb         $CLR_WHITEB
'
}

clr_bblack()   { clr_escape "$1" $CLR_BLANK $CLR_BOLD;   }
clr_bred()     { clr_escape "$1" $CLR_RED $CLR_BOLD;     }
clr_bgreen()   { clr_escape "$1" $CLR_GREEN $CLR_BOLD;   }
clr_bbrown()   { clr_escape "$1" $CLR_BROWN $CLR_BOLD;   }
clr_bblue()    { clr_escape "$1" $CLR_BLUE $CLR_BOLD;    }
clr_bmagenta() { clr_escape "$1" $CLR_MAGENTA $CLR_BOLD; }
clr_bcyan()    { clr_escape "$1" $CLR_CYAN $CLR_BOLD;    }
clr_bwhite()   { clr_escape "$1" $CLR_WHITE $CLR_BOLD;   }
# ================================================================

die() {
    clr_bred "ERROR: $1"
    [ $2 ] && exit $2 || exit 1
}

set -o pipefail

while read -r line; do
    case "${line%% *}" in
        CYGWIN* | MINGW* | MSYS* )
            EXE_SUFFIX=.exe
            SCRIPT_SUFFIX=.bat
            LUAROCKS_SUFFIX=.bat
            IS_LINUX=0
            IS_WINDOWS=1
            ;;

        *)
            EXE_SUFFIX=
            SCRIPT_SUFFIX=.sh
            LUAROCKS_SUFFIX=
            IS_LINUX=1
            IS_WINDOWS=0
            ;;
    esac
    break
done < /proc/version

CONTAINER_NAME_MANY_LINUX=opencv-lua-manylinux2014-x86-64
DOCKER_IMAGE_MANY_LINUX=quay.io/opencv-ci/opencv-python-manylinux2014-x86-64:20240524

DIST_VERSION=${DIST_VERSION:-1}
WSL_DISTNAME=${WSL_DISTNAME:-Ubuntu}
OPENCV_VERSION=${OPENCV_VERSION:-4.10.0}
WSL_EXCLUDED_TESTS=${WSL_EXCLUDED_TESTS:-"'!02-video-capture-camera.lua' '!threshold_inRange.lua' '!objectDetection.lua'"}
CONTAINER_NAME=${CONTAINER_NAME:-${CONTAINER_NAME_MANY_LINUX}}
DOCKER_IMAGE=${DOCKER_IMAGE:-${DOCKER_IMAGE_MANY_LINUX}}

LUA_VERSIONS="${LUA_VERSIONS:-$(echo luajit-2.1 5.{4,3,2,1})}"

function export_shared_env() {
    local mount_prefix="$1"
    local projectDir="${2:-${mount_prefix}$PWD}"

    echo "
export OPENCV_VERSION='${OPENCV_VERSION}'
export EXE_SUFFIX='${EXE_SUFFIX}'
export SCRIPT_SUFFIX='${SCRIPT_SUFFIX}'
export LUAROCKS_SUFFIX='${LUAROCKS_SUFFIX}'
export DIST_VERSION='${DIST_VERSION}'
export projectDir='${projectDir}'
"
}

function install_build_essentials_from_source() {
    echo '
# https://askubuntu.com/questions/355565/how-do-i-install-the-latest-version-of-cmake-from-the-command-line/865294#865294
if ! command -v cmake &>/dev/null; then
    CMAKE_VERSION=3.29.5
    CMAKE_VERSION_SHA256=4f7aaec19167b6400a64082af1d5a7bf2fbfcdb6966524856d38a94d5f173bd2
    CMAKE_INSTALL_SCRIPT=/opt/cmake/dl/cmake-${CMAKE_VERSION}-linux-x86_64.sh

    mkdir -p /opt/cmake/dl

    if ! echo "${CMAKE_VERSION_SHA256} ${CMAKE_INSTALL_SCRIPT}" | sha256sum --check --status; then
        wget https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}-linux-x86_64.sh -O ${CMAKE_INSTALL_SCRIPT} || exit $?
        echo "${CMAKE_VERSION_SHA256} ${CMAKE_INSTALL_SCRIPT}" | sha256sum --check --status || exit $?
    fi

    sh ${CMAKE_INSTALL_SCRIPT} --skip-license --prefix=/opt/cmake && \
    ln -s /opt/cmake/bin/cmake /usr/local/bin/cmake || exit $?
fi

if ! command -v ninja &>/dev/null; then
    mkdir -p /opt/ninja

    [ -d /opt/ninja/src ] || git clone --depth 1 --branch release https://github.com/ninja-build/ninja.git /opt/ninja/src
    cmake -B /opt/ninja/build -S /opt/ninja/src -DCMAKE_INSTALL_PREFIX:PATH=/opt/ninja && \
    cmake --build /opt/ninja/build --target ninja -j$(nproc) && \
    ln -s /opt/ninja/build/ninja /usr/local/bin/ninja || exit $?
fi'
}

function create_ci_user() {
    echo '
if ! id -u ci &>/dev/null; then
    useradd ci -m -s /bin/bash -G users --uid=1001 && \
    mkdir /io && \
    chown -R ci:ci /io && \
    su ci -c "git config --global --add safe.directory /io"
fi'
}

function open_git_project() {
    local remote="$1"
    local project="$2"
    local email="${3:-'you@example.com'}"
    local name="${4:-'Your Name'}"

    if [ -d "${project}/.git" ]; then
        cd "${project}" && \
        git remote set-url origin "${remote}" && \
        git reset --hard HEAD && \
        git clean -fd && \
        git pull --force
    else
        git clone "${remote}" "${project}" && \
        cd "${project}" && \
        git config pull.rebase true && \
        git config user.email "${email}" && \
        git config user.name "${name}"
    fi
}

WSL="$(command -v wsl)"
function wsl() {
    "$WSL" -d "$WSL_DISTNAME" -e bash -li "$@"
}

function docker_run_bash() {
    local name=${1:-${CONTAINER_NAME}}
    local image=${2:-${DOCKER_IMAGE}}

    local container_id="$(docker container ls -f "name=${name}" -q)"

    # Check if the container is stopped
    if [ ${#container_id} -eq 0 ]; then
        container_id="$(docker container ls -f "name=${name}" -q -f 'status=exited')"

        # if it is stopped, then start it
        if [ ${#container_id} -ne 0 ]; then
            docker start ${name} || exit $?
        fi
    fi

    if [ ${#container_id} -eq 0 ]; then
        # https://stackoverflow.com/questions/73092750/how-to-show-gui-apps-from-docker-desktop-container-on-windows-11/73901260#73901260
        wsl -c "docker run -it \
-v '/mnt$(cygpath -u "$PWD"):/src' \
-v '/tmp/.X11-unix:/tmp/.X11-unix' \
-v '/mnt/wslg:/mnt/wslg' \
-e DISPLAY -e WAYLAND_DISPLAY -e XDG_RUNTIME_DIR -e PULSE_SERVER \
--name '${name}' -d '${image}' bash"
    fi
}

function stash_push() {
    git stash push --include-untracked
}

function tidy() {
    node ../${ESLINT_CONFIG_PROJECT}/node_modules/eslint/bin/eslint.js --config=../${ESLINT_CONFIG_PROJECT}/.eslintrc --fix 'generator/**/*.js' 'scripts/*.js'
}

function doctoc() {
    node node_modules/doctoc/doctoc.js *.md docs/*.md
}

function set_url() {
    local url="$1"
    sed -r 's#url = "[^"]+"#url = "'$url'"#' -i luarocks/opencv_lua-scm-1.rockspec || return $?
    local diff="$(git diff HEAD luarocks/opencv_lua-scm-1.rockspec)"

    if [ ${#diff} -eq 0 ]; then
        return 0
    fi

    git add luarocks/opencv_lua-scm-1.rockspec && \
    git commit --amend --no-edit
}

function set_url_github() {
    set_url "git+https://github.com/smbape/lua-opencv.git"
}

function new_version() {
    npm version patch
}

function new_version_rollback() {
    local msg version

    local continue=1
    while [ $continue -eq 1 ]; do
        continue=0

        msg="$(git log -1 --pretty=format:%s)"

        if [ "v$msg" == "$(git tag -l "v$msg")" ];  then
            git tag -d "v$msg" || return $?
            continue=1
        fi

        if [ "$msg" == "$(node -pe "require('./package').version")" ]; then
            git reset --hard HEAD~1 || return $?
            continue=1
        fi
    done

    set_url_github
}

function push_all() {
    git push --all --follow-tags origin && git push --all --follow-tags github
}

function prepublish_stash_push() {
    local script='cd out/prepublish/${version}/opencv_lua${suffix} && git stash push --include-untracked --all -- samples'

    bash -c "
for version in ${LUA_VERSIONS}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script}
        popd; PATH=\$_PATH; unset _PATH
    done
done
"

    wsl -c "
source scripts/wsl_init.sh
for version in ${LUA_VERSIONS}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script}
        popd; PATH=\$_PATH; unset _PATH
    done
done
"
}

function prepublish_stash_pop() {
    local script='cd ${CWD}/out/prepublish/${version}/opencv_lua${suffix} && git reset --hard HEAD && git stash pop'

    bash -c "
for version in ${LUA_VERSIONS}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script}
        popd; PATH=\$_PATH; unset _PATH
    done
done
"

    wsl -c "
source scripts/wsl_init.sh
for version in ${LUA_VERSIONS}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script}
        popd; PATH=\$_PATH; unset _PATH
    done
done
"
}

function prepublish_any() {
    time node scripts/prepublish.js --pack --lua-versions "${LUA_VERSIONS}" "$@" && \
    time ./build${SCRIPT_SUFFIX} "-DLua_VERSION=luajit-2.1" --target luajit --install && \
    time ./build${SCRIPT_SUFFIX} "-DLua_VERSION=luajit-2.1" --target luarocks
}

function set_url_windows() {
    set_url "git+file://$(cygpath -m "$PWD")"
}

function prepublish_windows() {
    DIST_VERSION=${DIST_VERSION} prepublish_any
}

function use_luajit_opencv_lua_modules() {
    local sources="$PWD/out/prepublish/luajit-2.1/opencv_lua"
    rm -rf luarocks/lua_modules && \
    bash -c "cd ${sources}/ && source scripts/vcvars_restore_start.sh && ./luarocks/luarocks.bat install --deps-only samples/samples-scm-1.rockspec" && \
    cmd.exe //c mklink //j "$(cygpath -w "$PWD/luarocks/lua_modules")" "$(cygpath -w "${sources}/luarocks/lua_modules")"
}

function use_luajit_opencv_lua_contrib_modules() {
    local sources="$PWD/out/prepublish/luajit-2.1/opencv_lua-contrib"
    rm -rf luarocks/lua_modules && \
    bash -c "cd ${sources}/ && source scripts/vcvars_restore_start.sh && ./luarocks/luarocks.bat install --deps-only samples/samples-scm-1.rockspec" && \
    cmd.exe //c mklink //j "$(cygpath -w "$PWD/luarocks/lua_modules")" "$(cygpath -w "${sources}/luarocks/lua_modules")"
}

function build_windows() {
    set_url_windows && \
    new_version && \
    time prepublish_windows && \
    new_version_rollback && \
    use_luajit_opencv_lua_modules
}

function set_url_docker() {
    set_url "git+file:///src"
}

function make_available_nvs() {
    if command -v nvs &>/dev/null; then
        return
    elif [ -e "$HOME/.nvs/nvs.sh" ]; then
        . "$HOME/.nvs/nvs.sh"
        if command -v nvs &>/dev/null; then
            return
        fi
    fi

    export NVS_HOME="$HOME/.nvs"
    git clone https://github.com/jasongin/nvs "$NVS_HOME" && \
    . "$NVS_HOME/nvs.sh" install && \
    nvs add lts && \
    nvs use lts && \
    nvs link lts
}

function docker_init_script() {
    echo "$(install_build_essentials_from_source); $(create_ci_user)"
}

function fix_mounted_volumes_permission_docker() {
    local name=$1; shift

    # Fix mounted volume permissions:
    # Volumes are mounted using id 1000.
    # However, that user does not exist by default.
    # By creating a user with id 1000,
    # using that user allow full access to mounted volumes
    docker exec -it -u 0 "${name}" bash -c '
if ! id 1000 &>/dev/null; then
    useradd docker -m -s /bin/bash -G users,ci --uid=1000 && \
    chmod 775 /io
fi
'
}

function prepublish_manylinux() {
    local name=${CONTAINER_NAME_MANY_LINUX}
    local image=${DOCKER_IMAGE_MANY_LINUX}

    docker_run_bash ${name} ${image} && \
    docker exec -it -u 0 "${name}" yum install -y readline-devel zip && \
    docker exec -it -u 0 "${name}" bash -c "$(docker_init_script)" || return $?

    fix_mounted_volumes_permission_docker ${name} || return $?

    docker exec -it -u 1000 ${name} bash -c '
export PATH="$HOME/bin${PATH:+:${PATH}}" && \
export PATH="${PATH//:\/bin:/:}" && \
git config --global --add safe.directory /src/.git && \
source /src/scripts/tasks.sh && \
make_available_nvs && nvs add 16 && nvs use 16 && \
open_git_project file:///src /io/opencv-lua || exit $?
[ -d node_modules ] || npm ci || exit $?
find out/prepublish/ -mindepth 5 -maxdepth 5 -type f -name lockfile.lfs -delete
node scripts/prepublish.js --pack --lua-versions '"'${LUA_VERSIONS}'"' --server=/src/out/prepublish/server --repair'
}

function build_manylinux() {
    set_url_docker && \
    new_version && \
    time prepublish_manylinux && \
    new_version_rollback
}

function set_url_wsl() {
    set_url "git+file:///mnt$(cygpath -u "$PWD")"
}

function prepublish_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
source scripts/tasks.sh && \
prepublish_any --server="${projectDir}/out/prepublish/server"
'
}

function build_wsl() {
    set_url_wsl && \
    new_version && \
    time prepublish_wsl && \
    new_version_rollback
}

function build_custom_windows() {
    set_url_windows && \
    new_version && \
    time bash -c '
projectDir="$PWD"

source ${projectDir}/scripts/tasks.sh || exit $?
WORKING_DIRECTORY="/d/opencv-lua-custom"

mkdir -p "${WORKING_DIRECTORY}" && \
open_git_project "file://${projectDir}" "${WORKING_DIRECTORY}/build" || exit $?
[ -d node_modules ] || npm ci || exit $?

node scripts/prepublish.js --pack --server="${WORKING_DIRECTORY}/server" --lua-versions luajit-2.1 --name=opencv_lua-custom \
    -DBUILD_contrib=ON \
    -DWITH_FREETYPE=ON \
    -DFREETYPE_DIR=C:/vcpkg/installed/x64-windows \
    -DHARFBUZZ_DIR=C:/vcpkg/installed/x64-windows \
    -DENABLE_EXPERIMENTAL_WIDE_CHAR=ON \
    -DWITH_CUDA=ON \
    -DWITH_CUDNN=ON \
    -DOPENCV_DNN_CUDA=ON \
    -DCUDA_ARCH_BIN=$(nvidia-smi --query-gpu=compute_cap --format=csv | sed /compute_cap/d)
' && \
    new_version_rollback
}

function build_custom_linux_script() {
    echo '
source ${projectDir}/scripts/tasks.sh || exit $?
command -v npm &>/dev/null || make_available_nvs && nvs add 16 && nvs use 16 || exit $?

mkdir -p "${WORKING_DIRECTORY}" && \
open_git_project "file://${projectDir}" "${WORKING_DIRECTORY}/build" || exit $?
[ -d node_modules ] || npm ci || exit $?

find out/prepublish/ -mindepth 5 -maxdepth 5 -type f -name lockfile.lfs -delete

node scripts/prepublish.js --pack --server="${WORKING_DIRECTORY}/server" --lua-versions luajit-2.1 --name=opencv_lua-custom \
    -DBUILD_contrib=ON \
    -DWITH_FREETYPE=ON'
}

function install_build_essentials_debian_script() {
    echo '
apt update && \
apt install -y build-essential curl git libavcodec-dev libavformat-dev libdc1394-dev \
        libjpeg-dev libpng-dev libreadline-dev libswscale-dev libtbb-dev \
        pkg-config python3-pip python3-venv qtbase5-dev unzip wget zip || exit $?
apt install -y libtbbmalloc2 || apt install -y libtbb2'
}

function build_custom_debian() {
    local name=$1; shift
    local image=$1; shift

    docker_run_bash ${name} ${image} && \
    docker exec -it -u 0 "${name}" bash -c "$(install_build_essentials_debian_script) && apt install -y libfreetype-dev libharfbuzz-dev || exit \$?; $(docker_init_script)" && \
    fix_mounted_volumes_permission_docker ${name} && \
    set_url_docker && \
    new_version && \
    docker exec -it -u 1000 ${name} bash -c "
$(export_shared_env '' /src)
WORKING_DIRECTORY=/io/opencv-lua-custom
[ -d \${WORKING_DIRECTORY} ] || mkdir \${WORKING_DIRECTORY} || exit \$?
[ -d \${projectDir}/out/prepublish/server-${name} ] || mkdir \${projectDir}/out/prepublish/server-${name} || exit \$?
[ -L \${WORKING_DIRECTORY}/server ] || ln -s \${projectDir}/out/prepublish/server-${name} \${WORKING_DIRECTORY}/server || exit \$?
$(build_custom_linux_script)" && \
    new_version_rollback
}

function install_build_essentials_fedora_script() {
    local script='
$cpm update -y && \
$cpm install -y git \
        libjpeg-devel libpng-devel readline-devel make patch tbb-devel \
        pkg-config python3.12-pip qt5-qtbase-devel unzip wget zip || \
exit $?
$cpm install -y python3.12-pip || $cpm install -y python3.11-pip || exit $?
command -v curl &>/dev/null || $cpm install -y curl || exit $?

ALMALINUX_VERSION=$(sed -rn "s/ALMALINUX_MANTISBT_PROJECT=\"AlmaLinux-([0-9])\"/\1/p" /etc/os-release)
if [ ${#ALMALINUX_VERSION} -ne 0 ]; then
    $cpm install -y "dnf-command(config-manager)" || exit $?

    if [ ${ALMALINUX_VERSION} -eq 8 ]; then
        $cpm config-manager --set-enabled powertools || exit $?
    else
        $cpm config-manager --set-enabled crb || exit $?
    fi

    $cpm install -y epel-release && \
    $cpm install -y https://mirrors.rpmfusion.org/free/el/rpmfusion-free-release-${ALMALINUX_VERSION}.noarch.rpm && \
    $cpm install -y https://mirrors.rpmfusion.org/nonfree/el/rpmfusion-nonfree-release-${ALMALINUX_VERSION}.noarch.rpm && \
    $cpm update -y || exit $?

    if [ ${ALMALINUX_VERSION} -eq 8 ]; then
        $cpm install -y gcc-toolset-12-gcc gcc-toolset-12-gcc-c++ ffmpeg-devel || exit $?
    else
        $cpm install -y gcc gcc-c++ libavcodec-free-devel libavformat-free-devel libdc1394-devel libswscale-free-devel || exit $?
    fi
else
    $cpm install -y gcc gcc-c++ libavcodec-free-devel libavformat-free-devel libdc1394-devel libswscale-free-devel || exit $?
fi'

    echo "$(get_current_package_manager); ${script}"
}

function build_custom_fedora() {
    local name=$1; shift
    local image=$1; shift

    docker_run_bash ${name} ${image} && \
    docker exec -it -u 0 "${name}" bash -c "$(install_build_essentials_fedora_script) && \$cpm install -y freetype-devel harfbuzz-devel || exit \$?; $(docker_init_script)" && \
    fix_mounted_volumes_permission_docker ${name} && \
    set_url_docker && \
    new_version && \
    docker exec -it -u 1000 ${name} bash -c "
$(export_shared_env '' /src)
WORKING_DIRECTORY=/io/opencv-lua-custom
[ -d \${WORKING_DIRECTORY} ] || mkdir \${WORKING_DIRECTORY} || exit \$?
[ -d \${projectDir}/out/prepublish/server-${name} ] || mkdir \${projectDir}/out/prepublish/server-${name} || exit \$?
[ -L \${WORKING_DIRECTORY}/server ] || ln -s \${projectDir}/out/prepublish/server-${name} \${WORKING_DIRECTORY}/server || exit \$?
$(build_custom_linux_script)" && \
    new_version_rollback
}

function build_custom_wsl() {
    local name=wsl
    set_url_wsl && \
    new_version && \
    wsl -c "
$(export_shared_env /mnt); source scripts/wsl_init.sh || exit \$?
WORKING_DIRECTORY=\${sources}/../opencv-lua-custom
[ -d \${WORKING_DIRECTORY} ] || mkdir \${WORKING_DIRECTORY} || exit \$?
[ -d \${projectDir}/out/prepublish/server-${name} ] || mkdir \${projectDir}/out/prepublish/server-${name} || exit \$?
[ -L \${WORKING_DIRECTORY}/server ] || ln -s \${projectDir}/out/prepublish/server-${name} \${WORKING_DIRECTORY}/server || exit \$?
$(build_custom_linux_script)" && \
    new_version_rollback
}

function build_windows_debug() {
    time ./build.bat -d "-DLua_VERSION=luajit-2.1" --install --target luajit && \
    time ./build.bat -d "-DLua_VERSION=luajit-2.1" --install \
        "-DBUILD_contrib:BOOL=ON" \
        "-DWITH_FREETYPE:BOOL=ON" \
        "-DFREETYPE_DIR:PATH=C:/vcpkg/installed/x64-windows" \
        "-DHARFBUZZ_DIR:PATH=C:/vcpkg/installed/x64-windows" \
        "-DENABLE_EXPERIMENTAL_WIDE_CHAR:BOOL=ON" \
        "-DWITH_CUDA:BOOL=ON" \
        "-DWITH_CUDNN:BOOL=ON" \
        "-DOPENCV_DNN_CUDA:BOOL=ON" \
        "-DCUDA_ARCH_BIN=$(nvidia-smi --query-gpu=compute_cap --format=csv | sed /compute_cap/d)"
}

function build_wsl_debug() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
time ./build.sh -d "-DLua_VERSION=luajit-2.1" --install --target luajit && \
time ./build.sh -d "-DLua_VERSION=luajit-2.1" --install \
    "-DBUILD_contrib:BOOL=ON" \
    "-DWITH_FREETYPE:BOOL=ON"
'
}

function build_full() {
    build_windows && \
    build_manylinux && \
    build_custom_windows && \
    build_custom_debian build-custom-ubuntu-22.04 ubuntu:22.04 && \
    build_windows_debug && \
    build_wsl_debug
}

function test_rock_script() {
    local script='
if command -v cygpath &>/dev/null; then
unset -f ln
function ln() {
    local opt="$1"; shift
    local dest="$1"; shift
    local source="$1"; shift
    cmd.exe //c mklink //j "$(cygpath -w "$source")" "$(cygpath -w "$dest")"
}
fi

# ================================
# Prepare directories
# ================================
mkdir -p ${projectDir}/out/test || exit $?
test -d ${projectDir}/out/test/opencv/.git || git clone --depth 1 --branch ${OPENCV_VERSION} https://github.com/opencv/opencv.git ${projectDir}/out/test/opencv || exit $?

# ================================
# Disk and Network usage reduction:
# Get opencv sources from a single place
# ================================
if [ "${projectDir}" != "${PWD}" -a ! -L out/test/opencv ]; then
    mkdir -p out/test || exit $?
    test-e out/test/opencv && rm -f out/test/opencv
    ln -s ${projectDir}/out/test/opencv out/test/opencv || exit $?
fi

CWD="$PWD"
source "${projectDir}/scripts/tasks.sh" || exit $?

# ================================
# Disk and Network usage reduction:
# Install node_modules in a separate folder and link to it
# ================================
if [ "${projectDir}" != "${PWD}" ]; then
    open_git_project "file://${projectDir}" "${CWD}/out/test/build.node_modules" || exit $?
    [ -d node_modules ] || npm ci || exit $?
else
    [ -d node_modules ] || npm ci || exit $?
    [ -d "${CWD}/out/test/build.node_modules" ] || mkdir "${CWD}/out/test/build.node_modules"
    if [ ! -L "${CWD}/out/test/build.node_modules/node_modules" ]; then
        rm -rf "${CWD}/out/test/build.node_modules/node_modules"
        ln -s "${projectDir}/node_modules" "${CWD}/out/test/build.node_modules/node_modules" || exit $?
    fi
fi

if [ ${IS_WINDOWS} -eq 1 ]; then
    ARCH=x64
    LUAROCKS_BINDIR=
else
    ARCH=Linux-GCC
    LUAROCKS_BINDIR=-build/bin
fi

# ================================
# Install lua and luarocks
# ================================
[ "${version:0:6}" == luajit ] && target=luajit || target=lua
open_git_project "file://${projectDir}" "${CWD}/out/test/build.luaonly/${version}/src" || exit $?

export PATH="${PWD}/out/install/${ARCH}-Release/bin${PATH:+:${PATH}}"
export PATH="${PWD}/out/build.luaonly/${ARCH}-Release/luarocks/luarocks-prefix/src/luarocks${LUAROCKS_BINDIR}${PATH:+:${PATH}}"

command -v ${target}${EXE_SUFFIX} &>/dev/null || ./build${SCRIPT_SUFFIX} "-DLua_VERSION=${version}" --target ${target} --install || exit $?
command -v luarocks${EXE_SUFFIX} &>/dev/null || ./build${SCRIPT_SUFFIX} "-DLua_VERSION=${version}" --target luarocks || exit $?

command -v ${target}${EXE_SUFFIX} &>/dev/null || die "Failed to find ${target} executable" $?
command -v luarocks${EXE_SUFFIX} &>/dev/null || die "Failed to find luarocks executable" $?

# ================================
# Initialize the lua project
# ================================
open_git_project "file://${projectDir}" "${CWD}/out/test/${rock_type}/${version}/src${suffix}" || exit $?
[ "${version:0:6}" == luajit ] && lua_abi=5.1 || lua_abi=${version}

if [ ${IS_WINDOWS} -eq 1 ]; then
    cd luarocks && \
    luarocks.exe --lua-version "${lua_abi}" --lua-dir "$(cygpath -w "$(dirname "$(dirname "$(command -v ${target}.exe)")")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
    cd .. || exit $?
else
    cd luarocks && \
    luarocks --lua-version "${lua_abi}" --lua-dir "$(dirname "$(dirname "$(command -v ${target})")")" init --lua-versions "5.1,5.2,5.3,5.4" && \
    luarocks config --scope project cmake_generator Ninja && \
    luarocks config --scope project cmake_build_args -- -j$(nproc) && \
    cd ..
fi

# ================================
# Install the rock
# ================================
# Should we use an exact version?
if [ "${rock_type}" == "source" ]; then
    opencv_lua_version=${OPENCV_VERSION}
elif [ "${version:0:6}" == luajit ]; then
    opencv_lua_version=${OPENCV_VERSION}luajit2.1
else
    opencv_lua_version=
fi

./luarocks/luarocks${LUAROCKS_SUFFIX} install --deps-only samples/samples-scm-1.rockspec || exit $?
opencv_lua_installed="$(./luarocks/luarocks${LUAROCKS_SUFFIX} list --porcelain opencv_lua${suffix})"

install_rock=1
remove_rock=0

if [ ${#opencv_lua_installed} -ne 0 ]; then
    if [ ${upgrade_rock} -eq 1 ]; then
        remove_rock=1
    else
        install_rock=0
    fi
fi

[ ${remove_rock} -eq 0 ] || ./luarocks/luarocks${LUAROCKS_SUFFIX} remove opencv_lua${suffix} || exit $?
[ ${install_rock} -eq 0 ] || ./luarocks/luarocks${LUAROCKS_SUFFIX} install "--server=${projectDir}/out/prepublish/server" opencv_lua${suffix} ${opencv_lua_version} --force || exit $?

# ================================
# Run the tests
# ================================
if [ -d "${projectDir}/node_modules" -a ! -L node_modules ]; then
    rm -rf node_modules
    ln -s "${CWD}/out/test/build.node_modules/node_modules" node_modules || exit $?
fi

PYTHON_VENV_PATH="${CWD}/out/test/.venv" MODELS_PATH="${projectDir}/out/test/.models" node scripts/test.js --Release'

    local exclude=$1; shift
    local rock_type=$1; shift

    local upgrade_rock=0
    if [ "$1" == "--upgrade" ]; then
        upgrade_rock=1
        shift
    fi

    for arg in "$@"; do
        script="$script '$arg'"
    done

    if [ $exclude -eq 1 ]; then
        # excluded due to camera device missing
        script="$script $WSL_EXCLUDED_TESTS"
    fi

    echo "rock_type=${rock_type}; upgrade_rock=${upgrade_rock}; $script"
}

function test_prepublished_windows() {
    local exclude=$1; shift
    local rock_type=$1; shift
    local script="$(test_rock_script $exclude $rock_type "$@")"
    local versions

    if [ "$rock_type" == "source" ]; then
        versions='luajit-2.1'
    else
        versions="${LUA_VERSIONS}"
    fi

    bash -c "
$(export_shared_env)
source scripts/vcvars_restore_start.sh

for version in ${versions}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script} || exit \$?
        popd; PATH=\$_PATH; unset _PATH
    done
done
"
}

function test_prepublished_binary_windows() {
    test_prepublished_windows 0 binary "$@"
}

function test_prepublished_source_windows() {
    test_prepublished_windows 0 source "$@"
}

function test_prepublished_wsl() {
    local exclude=$1; shift
    local rock_type=$1; shift
    local script="$(test_rock_script $exclude $rock_type "$@")"
    local versions

    if [ "$rock_type" == "source" ]; then
        versions='luajit-2.1'
    else
        versions="${LUA_VERSIONS}"
    fi

    wsl -c "
$(export_shared_env /mnt)
source \${projectDir}/scripts/wsl_init.sh || exit \$?

for version in ${versions}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script} || exit \$?
        popd; PATH=\$_PATH; unset _PATH
    done
done
"
}

function test_prepublished_binary_wsl() {
    test_prepublished_wsl 1 binary "$@"
}

function test_prepublished_source_wsl() {
    test_prepublished_wsl 1 source "$@"
}

function test_prepublished_docker() {
    local name=$1; shift
    local exclude=$1; shift
    local rock_type=$1; shift
    local script="$(test_rock_script $exclude $rock_type "$@")"
    local versions

    if [ "$rock_type" == "source" ]; then
        versions='luajit-2.1'
    else
        versions="${LUA_VERSIONS}"
    fi

    docker exec -u 1001 -it ${name} bash -c "
if [ -f /opt/rh/gcc-toolset-12/enable ]; then
    source /opt/rh/gcc-toolset-12/enable || exit \$?
fi

git config --global --add safe.directory /src/.git && \
source /src/scripts/tasks.sh && \
make_available_nvs && nvs add 16 && nvs use 16 && \
cd /io || exit \$?
$(export_shared_env '' /src)

for version in ${versions}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script} || exit \$?
        popd; PATH=\$_PATH; unset _PATH
    done
done
"
}

function install_test_essentials_debian() {
    local name=$1; shift
    local image=$1; shift

    docker_run_bash ${name} ${image} && \
    docker exec -it -u 0 ${name} bash -c "
apt update && \
apt install -y curl g++ gcc git libgl1 libglib2.0-0 libreadline-dev libsm6 libxext6 make python3-pip python3-venv unzip wget \
|| exit \$?

$(docker_init_script)"
}

function test_prepublished_binary_debian() {
    local name=$1; shift
    local image=$1; shift

    install_test_essentials_debian ${name} ${image} && \
    test_prepublished_docker ${name} 1 binary "$@"
}

function install_build_essentials_debian() {
    local name=$1; shift
    local image=$1; shift

    docker_run_bash ${name} ${image} && \
    docker exec -it -u 0 "${name}" bash -c "$(install_build_essentials_debian_script) || exit \$?; $(docker_init_script)"
}

function test_prepublished_source_debian() {
    local name=$1; shift
    local image=$1; shift

    install_build_essentials_debian ${name} ${image} && \
    test_prepublished_docker ${name} 1 source "$@"
}

function get_current_package_manager() {
    echo '
if command -v apt 1>/dev/null 2>&1; then
    cpm=apt
elif command -v dnf 1>/dev/null 2>&1; then
    cpm=dnf
elif command -v yum 1>/dev/null 2>&1; then
    cpm=yum
fi'
}

function install_test_essentials_docker_fedora() {
    local name=$1; shift
    local image=$1; shift

    docker_run_bash ${name} ${image} && \
    docker exec -it -u 0 ${name} bash -c "
$(get_current_package_manager)
\$cpm update -y && \
\$cpm install -y gcc gcc-c++ git glib2 readline-devel libglvnd-glx libSM libXext make patch unzip wget || exit \$?
\$cpm install -y python3.12-pip || \$cpm install -y python3.11-pip || exit \$?
command -v curl &>/dev/null || \$cpm install -y curl || exit \$?

$(docker_init_script)"
}

function test_prepublished_binary_fedora() {
    local name=$1; shift
    local image=$1; shift

    install_test_essentials_docker_fedora ${name} ${image} && \
    test_prepublished_docker ${name} 1 binary "$@"
}

function install_build_essentials_fedora() {
    local name=$1; shift
    local image=$1; shift

    docker_run_bash ${name} ${image} && \
    docker exec -it -u 0 "${name}" bash -c "$(install_build_essentials_fedora_script) || exit \$?; $(docker_init_script)"
}

function test_prepublished_source_fedora() {
    local name=$1; shift
    local image=$1; shift

    install_build_essentials_fedora ${name} ${image} && \
    test_prepublished_docker ${name} 1 source "$@"
}

function publish() {
    for suffix in '' '-contrib'; do
        ./luarocks/luarocks.bat upload out/prepublish/server/opencv_lua${suffix}-${OPENCV_VERSION}-${DIST_VERSION}.rockspec --api-key=${LUA_ROCKS_API_KEY} || return $?
    done
    echo "Upload the content of $(cygpath -w "$PWD/out/prepublish/server") to github"
}

function build_clean_windows() {
    find out/prepublish/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
}

function build_clean_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
find out/prepublish/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
'
}

function test_debug_windows() {
    PYTHON_VENV_PATH="${PWD}/out/test/.venv" MODELS_PATH="${PWD}/out/test/.models" node scripts/test.js --Debug "$@"
}

function test_debug_wsl() {
    local script='
source scripts/wsl_init.sh || exit $?
PYTHON_VENV_PATH="${PWD}/out/test/.venv" MODELS_PATH="${projectDir}/out/test/.models" node scripts/test.js --Debug'

    for arg in "$@"; do
        script="$script '$arg'"
    done

    # excluded due to camera device missing
    script="$script $WSL_EXCLUDED_TESTS"

    wsl -c "$script"
}

function build_debug_clean_windows() {
    find out/build/x64-Debug/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
}

function build_debug_clean_wsl() {
    wsl -c '
source scripts/wsl_init.sh || exit $?
find out/build/Linux-GCC-Debug/ -type f \( -name CMakeCache.txt -o -name pyopencv_generated_include.h \) -delete
'
}

function compile_debug_windows() {
    local file="$1"
    compile_debug_strict_windows "opencv_lua/CMakeFiles/opencv_lua.dir/__/${file}"
}

function compile_debug_wsl() {
    local file="$1"
    compile_debug_strict_wsl "opencv_lua/CMakeFiles/opencv_lua.dir/__/${file}"
}

function compile_debug_strict_windows() {
    local file="$1"
    bash -c "cd out/build/x64-Debug/ && ninja ${file}.obj"
}

function compile_debug_strict_wsl() {
    local file="$1"
    wsl -c "source scripts/wsl_init.sh && cd out/build/Linux-GCC-Debug/ && ninja ${file}.o"
}

function install_build_essentials_wsl_debian() {
    "$WSL" -d "$WSL_DISTNAME" -u root -e bash -c "$(install_build_essentials_debian_script) || exit \$?; $(install_build_essentials_from_source)"
}

function test_build_script() {
    local exclude=$1; shift
    local script='
cd out/prepublish/${version}/opencv_lua${suffix} && \
./luarocks/luarocks${LUAROCKS_SUFFIX} install --deps-only samples/samples-scm-1.rockspec && \
node scripts/test.js --Release'

    for arg in "$@"; do
        script="$script '$arg'"
    done

    if [ $exclude -eq 1 ]; then
        # excluded due to camera device missing
        script="$script $WSL_EXCLUDED_TESTS"
    fi

    echo "$script"
}

function test_prepublished_build_windows() {
    local script="$(test_build_script 0 "$@")"

    bash -c "
source scripts/vcvars_restore_start.sh
$(export_shared_env)

for version in ${LUA_VERSIONS}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script} || exit \$?
        popd; PATH=\$_PATH; unset _PATH
    done
done
"
}

function test_prepublished_build_wsl() {
    local script="$(test_build_script 1 "$@")"

    wsl -c "
$(export_shared_env /mnt)
source \${projectDir}/scripts/wsl_init.sh || exit \$?

for version in ${LUA_VERSIONS}; do
    for suffix in '' '-contrib'; do
        _PATH=\$PATH; pushd \$PWD
        ${script} || exit \$?
        popd; PATH=\$_PATH; unset _PATH
    done
done
"
}
