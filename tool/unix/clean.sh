#!/bin/bash

SCRIPT_PATH=$(readlink -f "$0")
TOOL_UNIX_DIR=$(dirname "$SCRIPT_PATH")
TOOL_DIR=$(dirname "$TOOL_UNIX_DIR")
PROJECT_DIR=$(dirname "$TOOL_DIR")

rm -rf "${PROJECT_DIR}/build"