# libexif Fuzzing

This project contains coverage-guided fuzzing experiments on libexif, a C library for parsing, editing, and saving EXIF metadata embedded in image files (typically JPEG).

The goal of this project is to explore the internal architecture of libexif and apply modern fuzzing techniques to exercise different parsing pathways using AFL++ and sanitizers.

The work is part of a personal fuzzing research repository.

---

## Targets

The following libexif APIs are fuzzed:

exif_data_new_from_data()  
Fuzzes the direct EXIF parsing logic from raw memory buffers.

ExifLoader (exif_loader_write + exif_loader_get_data)  
Fuzzes the incremental parsing pipeline used in real-world ingestion scenarios.

parse → traverse → serialize  
After parsing, the harness traverses EXIF structures, processes maker notes, and serializes data using exif_data_save_data().

byte-order mutation → serialize  
Changes EXIF byte order and re-serializes data to explore additional transformation paths.

---

## Harnesses

The project contains the following fuzzing harnesses:

afl-libexif-harness.c  
Targets exif_data_new_from_data() with additional traversal, maker note processing, and serialization logic.

afl-libexif-loader-harness.c  
Targets the ExifLoader API to simulate incremental parsing and explore alternative internal code paths.

---

## Coverage

Coverage was approximated using AFL++ instrumentation metrics (edges discovered, corpus growth, and path exploration).

Observed results:

Execution speed  
≈ 25,000 exec/sec (persistent mode)

Corpus size  
≈ 2,500+ inputs

New edges discovered  
≈ 500+

Stability  
100%

Crashes  
0

Hangs  
0

These results indicate that the fuzzer successfully explored a significant portion of reachable parsing logic, although deeper paths (e.g., complex maker note handling) may require more structured inputs.

---

## Seed Corpus

Initial seeds were obtained from the libexif test suite.

The corpus was later enhanced with:

- Real-world JPEG images
- Multiple camera vendors (Canon, Nikon, etc.)
- Images containing GPS metadata, orientation tags, and maker notes

A minimized corpus was generated using afl-cmin to retain only coverage-relevant inputs.

---

## Dictionary

A custom EXIF/JPEG dictionary was used to improve mutation efficiency.

Examples:

"Exif\x00\x00"  
"II"  
"MM"  
"\xFF\xD8"  
"\xFF\xE1"  
"Canon"  
"NIKON"  
"GPS"  

This helps AFL++ preserve structural elements during mutation and improves deeper path exploration.

---

## Example Edge Case

No crashes or hangs were observed during fuzzing.

This suggests that:

- The current libexif version is relatively robust against malformed inputs
- Input validation prevents malformed data from reaching vulnerable states
- Certain deep parsing paths may require more structured or targeted inputs to trigger edge-case behavior

---

## Build Example

Example compilation using AFL++:

afl-clang-lto test/afl-libexif-harness.c \
libexif/.libs/libexif.a \
-I . -I libexif \
-O1 -fno-omit-frame-pointer \
-o afl-libexif-harness

Run fuzzing:

afl-fuzz -i seeds -o out -- ./afl-libexif-harness

---

## Notes

This project focuses on fuzzing methodology and coverage exploration, not vulnerability claims.

The harnesses demonstrate how multiple API entry points can be exercised to improve coverage in complex binary parsers.

This work may serve as a foundation for:

- Structure-aware EXIF fuzzing
- JPEG-aware preprocessing harnesses
- Integration with continuous fuzzing systems such as OSS-Fuzz
