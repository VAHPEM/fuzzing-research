# libredwg Fuzzing (DXF importer)

Target analysis and a dedicated **DXF-import** harness for
[GNU LibreDWG](https://github.com/LibreDWG/libredwg), a C library that reads and
writes DWG and DXF (AutoCAD) files.

> **Status:** harness + build recipe ready, code-review done. A full fuzzing
> campaign has **not** been completed yet — no findings are claimed.

## Why libredwg, and the gap

libredwg is on OSS-Fuzz via a single "fat" harness (`examples/llvmfuzz.c`) that
auto-detects the input format from its first bytes: `AC…` → the DWG binary
decoder (`dwg_decode`), `{` → JSON (`dwg_read_json`), otherwise → the DXF
importer (`dwg_read_dxf`). The OSS-Fuzz corpus is DWG-dominated, so `dwg_decode`
is fuzzed deeply while the **DXF importer `in_dxf.c` (~15k lines of hand-written
text parsing) and the JSON importer stay shallow** — mutated DWG inputs keep the
`AC` prefix (and stay in the decoder) or turn into garbage the DXF reader
rejects at the top. Feeding valid DXF seeds to a DXF-only harness drives
`in_dxf.c` far past where the OSS-Fuzz run reaches.

Reporting route: libredwg uses **GitHub Security Advisories**, which can mint a
CVE automatically and credit the reporter (`SECURITY.md`).

## Code review first (`in_dxf.c`)

Before fuzzing, a manual pass over the hot sinks in `in_dxf.c` — the string
reader, the binary-value `memcpy`, the LTYPE `strings_area` accumulation, the
ACIS `encr_sat_data` `strcpy`/`strcat`, and the `dashes[]`/`lines[]`
index-by-count writes — found them **all explicitly bounds-checked**:

- the `sscanf("%s", buf)` overflow is dead code (`#else` branch, not compiled);
  the live path sizes strings with `malloc(strlen+1)` then `strcpy`;
- the binary readers verify `byte + size > dat->size` before copying, and the
  DXFB size is a single byte (0–255);
- `strings_area` / `encr_sat_data` check the running length before each copy;
- every `dashes[j]`/`lines[j]` write is guarded by `CHK_array`/`CHK_dashes`,
  which `return NULL` on `i < 0 || i >= o->num_*`.

No obvious unguarded overflow remained, so fuzzing is the right tool for the
subtler residue (reachable assertions, NULL-derefs, OOB in less-trodden object
decoders or the encode round-trip). **When triaging, prefer OOB-write /
use-after-free over reachable-assertion crashes** — libredwg's recent CVEs skew
to reachable assertions (CWE-617) and NULL-derefs, which are only DoS-class.

## Harness

`dxffuzz.c` forces the DXF path: it NUL-terminates a copy of the input and calls
`dwg_read_dxf` directly, then `dwg_free`, so every exec exercises `in_dxf.c`
regardless of the leading bytes.

## Build

No autotools/cmake were available on the test host, so cmake was installed via
pip. `build_dxf.sh` wraps the recipe:

    pip install --user --break-system-packages cmake ninja
    ./build_dxf.sh /path/to/libredwg-checkout

Key flags: `-DDISABLE_WERROR=ON` (a `_POSIX_C_SOURCE` redefinition is otherwise
promoted to an error), `-DLIBREDWG_DISABLE_JSON=ON` (drops `in_json.c`, whose
`jsmn` submodule is absent in a shallow clone and which the DXF harness does not
need), `-DLIBREDWG_LIBONLY=ON`, static lib, ASan + UBSan.

## Seeds & run

Seed from libredwg's own `test/` DXF files (~67 of them):

    mkdir -p corpus_dxf
    find /path/to/libredwg-checkout/test -iname '*.dxf' -exec cp -n {} corpus_dxf/ \;
    ASAN_OPTIONS=detect_leaks=0:abort_on_error=1 \
      ./fuzz_dxf -fork=$(nproc) -ignore_crashes=1 -max_len=131072 -timeout=25 \
      -dict=dxf.dict -artifact_prefix=art_dxf/ corpus_dxf

Always novelty-check any crash against the LibreDWG issue tracker, GitHub
Security Advisories, OSV and NVD before reporting — libredwg has a very dense
CVE history, so deduplication is essential.
