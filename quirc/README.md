# quirc Fuzzing

Coverage-guided fuzzing of [quirc](https://github.com/dlbeer/quirc), a small C
QR-code recognition library. QR decoding is an attacker-reachable surface on
mobile (a malicious QR code presented to a camera), which motivated the target.

## Setup
- Engines: libFuzzer (clang) and an AFL++ persistent-mode harness
- Sanitizers: AddressSanitizer (ASan) + UndefinedBehaviorSanitizer (UBSan)
- Target commit: 927d680 (master)

## Harness
quirc consumes a raw grayscale image buffer, not a file format, so the fuzzer
input is framed as:

    [2-byte big-endian width][2-byte big-endian height][ w*h grayscale bytes ]

Width/height are clamped to [1,1024] and w*h is capped at 2^20. The harness
drives the full pipeline — `quirc_end` (identify) -> `quirc_count` ->
`quirc_extract` -> `quirc_decode` — plus the `quirc_flip` mirror path
(ISO 18004:2015), where the trickier logic lives.

Files:
- `quirc_harness.c` — libFuzzer entry point
- `quirc_afl.c` — AFL++ persistent-mode harness
- `repro_flip.c` — standalone, public-API-only reproducer
- `gen_seeds.py` — seeds from real QR codes (qrcode + PIL) plus degenerate /
  finder-pattern seeds
- `build.sh` — libFuzzer build helper
- `poc/poc_flip_oob.bin` — minimized crashing input

## Build & run (libFuzzer)

    QUIRC_SRC=/path/to/quirc ./build.sh
    python3 gen_seeds.py seeds && mkdir -p corpus && cp seeds/* corpus/
    ./fuzz_quirc -fork=$(nproc) -ignore_crashes=1 -max_len=40000 -timeout=25 corpus

## Finding (already-known — kept as a methodology case study)

Fuzzing reproduced a **stack-buffer-overflow** in `quirc_flip`: an out-of-bounds
read in `grid_bit` (`decode.c:412`) and an out-of-bounds write into the local
`flipped` struct, on oversized / falsely-detected grids. Root cause:
`quirc_extract` can return a `code` with `code.size > QUIRC_MAX_GRID_SIZE` (177),
and `quirc_flip` iterates up to `code.size` **without** the size guard that the
sibling paths already have (`quirc_decode` #119, `read_data`/extract #87,
`fitness_all`).

**This is not a new bug.** It was reported in 2020 as issue
[#91](https://github.com/dlbeer/quirc/issues/91) ("bug_memory_copy：quirc_flip"),
closed by the reporter the same day, and never fixed upstream. It is documented
here as a methodology example — and as a reminder that novelty-checking the
**issue tracker** (not just the commit log) must come before any disclosure.

A one-line guard fixes it (verified against the PoC):

    if (code->size > QUIRC_MAX_GRID_SIZE)
        return;   /* at the top of quirc_flip() */

## Takeaways
- Mature, OSS-Fuzz-covered parsers (quirc included) are largely dry to pure
  mutation fuzzing; the crashes that remain are often already reported.
- Always check the issue tracker for duplicates before claiming a finding.
- A reproducible, minimized, root-caused PoC + patch is what separates a real
  report from automated noise.
