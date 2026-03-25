#!/usr/bin/env bash
set -euo pipefail

CXX=${CXX:-clang++}
CXXFLAGS=${CXXFLAGS:--O1 -g -DNDEBUG -std=c++17 -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer}
INCLUDES="-I$(pwd)/target/include"

$CXX $CXXFLAGS \
    harness_parse_yaml.cpp \
    target/src/toml.cpp \
    $INCLUDES \
    -o fuzz_parse_yaml_release
