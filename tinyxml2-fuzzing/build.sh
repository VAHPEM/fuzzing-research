#!/usr/bin/env bash
set -e

clang++ -std=c++17 -O2 -g -fsanitize=fuzzer,address -I./target harness.cpp target/tinyxml2.cpp -o fuzz_tinyxml2
clang++ -std=c++17 -O2 -g -fsanitize=fuzzer,address -I./target harness_dom.cpp target/tinyxml2.cpp -o fuzz_dom
clang++ -std=c++17 -O2 -g -fsanitize=fuzzer,address -I./target harness_loadfile.cpp target/tinyxml2.cpp -o fuzz_loadfile
