#!/usr/bin/env bash
# Build the quirc libFuzzer harness with ASan + UBSan.
# Usage: QUIRC_SRC=/path/to/quirc ./build.sh
set -euo pipefail
QUIRC_SRC="${QUIRC_SRC:-./quirc-src}"   # a checkout of https://github.com/dlbeer/quirc
CC="${CC:-clang}"
$CC -O1 -g -fno-omit-frame-pointer \
    -fsanitize=fuzzer,address,undefined -fsanitize-address-use-after-scope \
    -I"$QUIRC_SRC/lib" \
    quirc_harness.c \
    "$QUIRC_SRC/lib/quirc.c" "$QUIRC_SRC/lib/identify.c" \
    "$QUIRC_SRC/lib/decode.c" "$QUIRC_SRC/lib/version_db.c" \
    -o fuzz_quirc
echo "built: ./fuzz_quirc"
