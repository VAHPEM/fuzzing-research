# Fuzzing Research

Coverage-guided fuzzing experiments on real-world C/C++ parser libraries —
harness design, seed/dictionary engineering, AI-assisted target selection, and
coverage analysis.

This top-level page is only an **overview** of the methodology and tooling.
**The detailed write-up for each target lives in that project's own folder
README** (linked in the table below).

## Tools & environment

- **Engines:** libFuzzer, AFL++
- **Sanitizers:** AddressSanitizer (ASan), UndefinedBehaviorSanitizer (UBSan)
- **Compiler / coverage:** Clang / LLVM, llvm-cov
- **AI-assisted analysis:** Qwen (Alibaba Cloud) + OpenHands — repository
  structure analysis, input entry-point identification, data-flow tracing
  through parsing logic, and target selection

## Workflow

1. Select a real-world target library
2. Static exploration (manual + AI-assisted)
3. Identify high-risk parsing / input-processing APIs
4. Design fuzzing harnesses for those APIs
5. Build a seed corpus and dictionaries
6. Compile with sanitizers enabled
7. Run coverage-guided fuzzing (libFuzzer / AFL++)
8. Improve harness design to grow coverage
9. Minimize the corpus, triage, and analyze results

## Projects

| Project | Format | Engine(s) | Summary | Details |
|---|---|---|---|---|
| TinyXML2 | XML (DOM) | libFuzzer | `Parse()` / `LoadFile()` / DOM-traversal harnesses; whitespace modes | [tinyxml2-fuzzing/](tinyxml2-fuzzing/README1.md) |
| libyaml | YAML | libFuzzer | scan / parse / load / emit + parse→emit round-trip; ~80–95% line coverage | [libyaml-fuzzing/](libyaml-fuzzing/README2.md) |
| toml++ | TOML | libFuzzer | parser / formatter / JSON / YAML / `at_path()`; **2 undefined-behavior bug families reported upstream** | [tomlplusplus-fuzzing/](tomlplusplus-fuzzing/README3.md) |
| pugixml | XML | AFL++ | AI-assisted target selection; DOCTYPE / entity / PCDATA paths | [pugixml/](pugixml/README5.md) |
| libexif | EXIF metadata | AFL++ | persistent mode; direct + loader harnesses; ~25k exec/s | [libexif/](libexif/README4.md) |
| quirc | QR decode | libFuzzer / AFL++ | reproduced a `quirc_flip` out-of-bounds on oversized grids — matched the already-known issue #91, kept as a methodology case study | [quirc/](quirc/README.md) |

## Repository layout

Each project folder contains its own harness source code, a minimized seed
corpus, dictionaries (where applicable), build instructions, and a README with
the full write-up. Large fuzzing artifacts (generated corpora, crash files,
coverage output) are intentionally excluded.

## Future directions

- Structure-aware and grammar-based fuzzing for complex formats
- Differential fuzzing across parsers of the same format
- Integrating under-fuzzed, widely-deployed native parsers into continuous
  fuzzing (OSS-Fuzz)

## Purpose

Learning and practicing modern fuzzing; exploring the internals of real-world
parsers; experimenting with harness design and coverage improvement;
integrating AI-assisted analysis into vulnerability research; and building a
portfolio of fuzzing and security-research work. The focus is on
**methodology, workflow, and coverage**, documenting findings as they arise.
