#!/bin/bash
# This script will use cmake to manage the project

function build()
{
    cmake -B build -G "Unix Makefiles" -DBOARD_NAME=$board_name -DCMAKE_TOOLCHAIN_FILE=${board_name}_toolchain.cmake -DCMAKE_BUILD_TYPE=$build_type
}

function compile()
{
    cmake --build build
}

function build_compile()
{
    build
    compile
}

function clear_build()
{
    echo "clear build"
    rm build -rf
}

function help_info()
{
    echo "This shell script help user to build the easy_bootloader project easily"
    echo "Usage:"
    echo "./build.sh [options]"
    echo ""
    echo "options"
    echo "  -[B|b] --name=<board name> --[debug|release]    = build the project by board name"
    echo "  -[C|c]                                          = compile the project"
    echo "  -[R|r]                                          = clear the target and the project directory"
}

if [ $# -eq 0 ]; then
    echo "Please input the board name that you want to compile"
    exit 0
fi

build=0
compile=0
clear=0
build_type=Debug

while [ $# -gt 0 ]; do
    if [ $( expr $1 : '-[B|b]' ) -gt 0 ]; then
        build=1
    elif [ $( expr $1 : '-[C|c]' ) -gt 0 ]; then
        compile=1
    elif [ $( expr $1 : '-[R|r]' ) -gt 0 ]; then
        clear=1
    elif [ $( expr $1 : '--name=[a-z|A-Z|0-9]\{1,\}' ) -gt 0 ]; then
        board_name=$( expr $1 : '--name=\([a-z|A-Z|0-9]\{1,\}\)' )
    elif [ $( expr $1 : '--release' ) -gt 0 ]; then
        build_type=Release
    elif [ $( expr $1 : '--debug' ) -gt 0 ]; then
        build_type=Debug
    elif [ $( expr $1 : '-[-help|h]' ) -gt 0 ]; then
        help_info
    fi
    shift
done

if [ $build -gt 0 ]; then
    if [ -n "$board_name" ]; then
        build
    else
        echo "Please input the board name that you want to compile"
        exit 0
    fi
fi

if [ $compile -gt 0 ]; then
    compile
fi

if [ $clear -gt 0 ]; then
    clear_build
fi

exit 0