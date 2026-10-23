#!/bin/sh
# Build the DXF-only fuzzer against a sanitized static libredwg.
# No autotools/cmake needed on the host beyond cmake-via-pip:
#   pip install --user --break-system-packages cmake ninja
set -e
SRC=${1:-.}            # path to a libredwg checkout
cmake -S "$SRC" -B "$SRC/bld" -G Ninja \
  -DBUILD_SHARED_LIBS=OFF -DLIBREDWG_LIBONLY=ON \
  -DDISABLE_WERROR=ON -DLIBREDWG_DISABLE_JSON=ON \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_C_FLAGS="-g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined"
cmake --build "$SRC/bld" -j"$(nproc)"
A=$(find "$SRC/bld" -name 'libredwg*.a' | head -1)
clang -g -O1 -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer \
  -I "$SRC/include" -I "$SRC/src" -I "$SRC/bld" -I "$SRC/bld/src" \
  "$(dirname "$0")/dxffuzz.c" "$A" -lm -o fuzz_dxf
echo "built ./fuzz_dxf"
