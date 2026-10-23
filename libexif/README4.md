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

---

## Structure-aware MakerNote harness (v1)

The direct/loader harnesses above feed whole Exif/TIFF blobs to
`exif_data_new_from_data()`. That left the vendor MakerNote parsers
(`exif-mnote-data-*.c`) at ~0% coverage and 0 crashes — not because they are
robust, but because they were **reachable in code yet unreached by inputs**.

**Root cause.** A MakerNote (tag `0x927C`) is only interpreted when it sits
inside the EXIF sub-IFD, reached from IFD0 via the ExifIFDPointer tag `0x8769`.
Blind mutation of whole JPEG/Exif blobs essentially never synthesises that
nested skeleton *and* a recognised per-vendor signature, so `interpret_maker_note()`
and every `exif_mnote_data_<vendor>_load()` were never entered.

**Fix.** `libexif-mnote-harness.c` builds the `IFD0 → ExifIFD → MakerNote`
skeleton itself and splices the fuzzer bytes straight into the MakerNote value.
`byte[0]` selects the vendor; the per-vendor magic bytes are verified against
libexif's own `*_identify()` functions (Nikon/Sanyo/Epson are dispatched by the
olympus module, Casio by the pentax module).

**Validation (local clang/gcc + gcov build).** A single untuned run over a
garbage payload already entered the vendor parsers:

| file | lines executed |
|---|---|
| exif-mnote-data.c (dispatcher) | ~68% |
| olympus (incl. Nikon/Sanyo/Epson) | ~47% |
| pentax (incl. Casio) | ~41% |
| fuji | ~34% |
| canon | ~32% |
| apple | 0% (identify matches but not yet selected — TODO) |

**Files.** `libexif-mnote-harness.c` (libFuzzer entry), `gen_mnote_seeds.py`
(extracts real MakerNote bytes from vendor JPEGs into `[vendor_byte][note]`
seeds), `mnote.dict` (vendor magic + TIFF tokens).

**Next.** libFuzzer + ASan + UBSan campaign seeded from real vendor photos.
Priority sinks, all now reachable: each vendor's `*_get_value` (OOB on value
formatting), sub-IFD offset/count fields trusted from the note, and integer
handling in the `exif_data_save_data()` re-encode path. Any finding must pass a
novelty check (issue tracker + closed PRs + CVE + OSV) before disclosure.


### v2 update

- **Apple removed.** libexif's `interpret_maker_note()` has the Apple branch
  commented out ("disabled until apple makernote can also be saved") — it is
  dead code, so Apple is dropped from the harness vendor table (live vendors:
  Canon, Olympus V1/V2 incl. Nikon/Sanyo/Epson, Fuji, Pentax/Casio).
- **Real-seed generator.** `gen_mnote_seeds.py` now follows
  `IFD0 -> ExifIFDPointer(0x8769) -> EXIF sub-IFD -> MakerNote(0x927C)`,
  honours TIFF byte order, and emits exact `[vendor_byte][note-after-signature]`
  seeds. From the existing `corpus_min` it yields ~1378 real-note seeds spread
  across all live vendors, which start the campaign deep inside the per-vendor
  entry parsers instead of cold.


### Campaign result (2026-10-23)

Ran the v2 harness with libFuzzer + AddressSanitizer + UndefinedBehaviorSanitizer,
8 parallel jobs, seeded with ~1378 real MakerNote bodies extracted by
`gen_mnote_seeds.py` from `corpus_min`. Replaying the seeds alone already covered
**76-90%** of the Canon/Olympus/Fuji/Pentax loaders and their entry parsers
(versus ~0% for the whole-blob harnesses).

Over ~2 hours and ~30M executions edge coverage **plateaued at ~5000 edges with
0 crashes / OOMs / timeouts** — consistent with libexif being under continuous
OSS-Fuzz. The reusable contribution here is the harness + generator that make the
per-vendor MakerNote parsers *reachable at all*; squeezing new findings out of
this surface needs a different lever rather than more wall-clock:

- `-use_value_profile=1` to get past the magic/offset comparisons in the parsers;
- Canon-specific seeds for the still-cold `mnote-canon-entry.c` (~7.5%);
- or pivoting to a higher-ceiling, under-fuzzed target.

No memory-safety vulnerability is claimed.
