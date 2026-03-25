#!/usr/bin/env bash
set -euo pipefail

CXX=${CXX:-clang++}
COMMON_FLAGS=${COMMON_FLAGS:--O1 -g -std=c++17 -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer}
INCLUDES="-I$(pwd)/target/include"

$CXX $COMMON_FLAGS \
    harness_parse_toml.cpp \
    target/src/toml.cpp \
    $INCLUDES \
    -o fuzz_parse_toml

$CXX $COMMON_FLAGS \
    harness_parse_toml_format.cpp \
    target/src/toml.cpp \
    $INCLUDES \
    -o fuzz_parse_toml_format
