#!/bin/bash

function parse_argument
{
    case "$1" in
        --debug)
            BUILD_TYPE=Debug
            BUILD_DIR=${PROJECT_DIR}/build/debug
            ;;
        --release)
            BUILD_TYPE=Release
            BUILD_DIR=${PROJECT_DIR}/build/release
            ;;
        --test)
            RUN_TEST="ON"
            ;;
        *)
            echo "Invalid argument."
            exit 1
            ;;
    esac
}

SCRIPT_PATH=$(readlink -f "$0")
TOOL_UNIX_DIR=$(dirname "$SCRIPT_PATH")
TOOL_DIR=$(dirname "$TOOL_UNIX_DIR")
PROJECT_DIR=$(dirname "$TOOL_DIR")

BUILD_TYPE=Release
BUILD_DIR=${PROJECT_DIR}/build/release
RUN_TEST="OFF"

for arg in $*
do
    parse_argument $arg
done

mkdir -p "${BUILD_DIR}"
cmake -S "${PROJECT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cmake --build "${BUILD_DIR}"

if [[ ${RUN_TEST} == "ON" ]]; then
    export LIBFTP_TEST_SERVER_PATH="${PROJECT_DIR}/test/server/server.py"
    ctest --test-dir "${BUILD_DIR}/test" --verbose
fi