#!/bin/bash

die() {
    echo "ERROR: $1" 1>&2
    [ $2 ] && exit $2 || exit 1
}

#Absolute path to this script
SCRIPT=$(readlink -f "$0")
#Absolute path this script is in
SCRIPTPATH=$(dirname "$SCRIPT")

skip_build=0
skip_config=0
has_install=0
is_dry_run=0
has_test=0
GENERATOR=Ninja
PLATFORM=
TARGET=all

print_help() {
    echo '
Usage: ./build.sh [options]

Options:
    --help|h|help               Display the help message.
    --dry-run                   Display commands that will be executed without running them.
    --g                         Generate cmake config only.
    --d                         Use Debug build type.
    --no-config                 Do not generate cmake config.
    --build                     Build only.
    --no-build                  Do not build.
    --install                   Install.
    --test                      Execute test.
    -G <generator-name>         Specify a build system generator. ('${GENERATOR}')
    -A <platform-name>          Specify platform name if supported by generator. ('${PLATFORM}')
    --prefx <directory>         Install directory. ('$PREFIX')
    --target <target-name>      Target to build. ('$TARGET')
'
}

ac_opt=0
ac_prev=
for ac_option in "$@"; do
    # If the previous option needs an argument, assign it.
    if test ${#ac_prev} -ne 0; then
        eval "$ac_prev='$ac_option'"
        ac_prev=
        continue
    fi

    case "$ac_option" in
        --dry-run)
            is_dry_run=1
            continue
            ;;
        -d)
            CMAKE_BUILD_TYPE=Debug
            continue
            ;;
        -g)
            skip_build=1
            skip_config=0
            continue
            ;;
        --no-config)
            skip_config=1
            continue
            ;;
        --build)
            skip_build=0
            skip_config=1
            continue
            ;;
        --no-build)
            skip_build=1
            continue
            ;;
        --install)
            has_install=1
            continue
            ;;
        --test)
            has_test=1
            continue
            ;;
    esac

    case "$ac_option" in
        *=*)
            ac_opt=0
            key="${ac_option%%=*}"
            value="${ac_option#*=}"
            ;;
        *)
            ac_opt=1
            key="${ac_option}"
            value=
            ;;
    esac

    case "$key" in
        -C)
            key=--binary-dir
            ;;
        -G)
            key=--generator
            ;;
        -A)
            key=--platform
            ;;
        --config)
            key=--config-name
            ;;
        --binary-dir|--generator|--platform|--config-name )
            echo "Unknown option $key" 1>&2
            exit 1
            ;;
    esac

    case "$key" in
        --binary-dir|--generator|--platform|--prefix|--target|--config-name )
            key="${key:2}"
            key="${key//-/_}"

            if test $ac_opt -eq 1; then
                ac_prev="${key^^}"
            else
                eval "${key^^}='${value}'"
            fi
            ;;
        --help|-h|-help )
            print_help
            exit 0
            ;;
        * )
            echo "Unknown option $key" 1>&2
            exit 1
            ;;
    esac
done

try_run=
test $is_dry_run -eq 0 || try_run="echo "

CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"
CONFIG_NAME=${CONFIG_NAME:-Linux-GCC-$CMAKE_BUILD_TYPE}
BUILD_FOLDER="${BUILD_FOLDER:-$PWD/out/build/$CONFIG_NAME}"
CMAKE_INSTALL_PREFIX="${PREFIX:-$PWD/out/install/$CONFIG_NAME}"
if [[ "$TARGET" == 'lua' || "$TARGET" == 'luarocks' ]]; then
    export LUA_ONLY=ON
fi

${try_run}mkdir -p "$BUILD_FOLDER" && ${try_run}cd "$BUILD_FOLDER" || die "Cannot access build directory $BUILD_FOLDER" $?

test ${#PLATFORM} -eq 0 || GENERATOR="$GENERATOR -A $PLATFORM"

test $skip_config -eq 1 || ${try_run}cmake -G $GENERATOR -DCMAKE_BUILD_TYPE:STRING=$CMAKE_BUILD_TYPE "-DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}" "$SCRIPTPATH" || exit $?
test $skip_build -eq 1 || ${try_run}cmake --build . --target $TARGET -j4 || exit $?
test $has_install -eq 0 || ${try_run}cmake --install . --prefix "$CMAKE_INSTALL_PREFIX" || exit $?

if test $has_test -eq 1; then
    LUA_CPATH="$("$BUILD_FOLDER/bin/luajit" -e 'print(package.cpath)');$BUILD_FOLDER/luajit/lib/?.so" \
    ctest -C $CMAKE_BUILD_TYPE -R test_build

    if test -x "$CMAKE_INSTALL_PREFIX/bin/luajit"; then
        LUA_CPATH="$("$CMAKE_INSTALL_PREFIX/bin/luajit" -e 'print(package.cpath)');$CMAKE_INSTALL_PREFIX/lib/lua/?.so" \
        ctest -C $CMAKE_BUILD_TYPE -R test_install
    fi
fi
