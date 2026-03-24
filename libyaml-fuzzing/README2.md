# libyaml Fuzzing

This project contains coverage-guided fuzzing experiments on **libyaml**, a C library for parsing and emitting YAML.

The goal of this project is to explore the internal architecture of libyaml and apply modern fuzzing techniques to exercise parser and emitter components using **libFuzzer** and sanitizers.

The work is part of a personal fuzzing research repository.

---

# Targets

The following libyaml APIs are fuzzed:

yaml_parser_scan()  
Fuzzes the scanner/tokenizer layer.

yaml_parser_parse()  
Fuzzes the parser event pipeline.

yaml_parser_load()  
Fuzzes the higher-level document composition logic.

yaml_emitter_emit()  
Fuzzes YAML emission logic using structured event sequences.

yaml_emitter_emit() (nested harness)  
Generates nested YAML structures to explore deeper emitter paths.

parse → emit round-trip  
Parses fuzz input into events and re-emits them, significantly improving emitter coverage.

---

# Harnesses

The project contains the following fuzzing harnesses:

harness_scan.cpp  
Targets yaml_parser_scan()

harness_parse.cpp  
Targets yaml_parser_parse()

harness_load.cpp  
Targets yaml_parser_load()

harness_emit.cpp  
Basic harness for yaml_emitter_emit()

harness_emit_nested.cpp  
Generates nested YAML structures for deeper emitter exploration.

harness_roundtrip_parse_emit.cpp  
Round-trip harness that parses YAML input and emits the parsed events.

---

# Coverage

Coverage was measured using llvm-cov after fuzzing with libFuzzer + AddressSanitizer + UBSan.

Parser-side coverage:

parser.c  
≈ 92.9% line coverage

reader.c  
≈ 94.8% line coverage

scanner.c  
≈ 94.0% line coverage

Emitter-side coverage:

Basic emitter harness  
≈ 59% line coverage of emitter.c

Nested emitter harness  
≈ 68% line coverage of emitter.c

Round-trip harness  
≈ 85% line coverage of emitter.c

Overall round-trip binary coverage:

Line coverage  
≈ 79.6%

Branch coverage  
≈ 71.5%

---

# Example Edge Case

During early experimentation, a round-trip harness using a fixed-size emitter output buffer triggered a **double-free candidate during cleanup**.

After switching the emitter output to a **sink callback**, the issue no longer reproduced.  
This suggests the problem was caused by incorrect harness memory handling rather than a confirmed libyaml vulnerability.

The input that triggered the original issue: [[[[[?[[?[[?]]:[[?[[?[?]]]]:[[[[[[[[[?p]]]:[[?[?]]:[[?]]]]:[[?[[[[[?[?]]:[[?[[?

---

# Build Example

Example compilation using libFuzzer:

clang++ harness_parse.cpp \
  target/src/.libs/libyaml.a \
  -I target/include \
  -fsanitize=fuzzer,address,undefined \
  -g -O1 \
  -o fuzz_yaml_parse

Run fuzzing:

./fuzz_yaml_parse corpus

---

# Notes

This project focuses on **fuzzing methodology and coverage exploration**, not vulnerability claims.

The harnesses may serve as a starting point for further fuzzing experiments or potential integration with continuous fuzzing systems such as **OSS-Fuzz**.
