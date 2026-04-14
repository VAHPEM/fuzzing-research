# Fuzzing Research

This repository contains my experiments with **coverage-guided fuzzing on real-world software libraries**.

The goal of this project is to explore how complex input parsers behave under fuzzing, practice building effective fuzzing harnesses, and study how modern fuzzing tools can be used to discover robustness issues in widely used libraries.

The projects use a combination of **libFuzzer**, **AFL++**, and **AI-assisted analysis**.

---

# Fuzzing Environment

This repository uses multiple fuzzing toolchains depending on the target.

## Fuzzing Engines

- libFuzzer
- AFL++

## Sanitizers

- AddressSanitizer (ASAN)
- UndefinedBehaviorSanitizer (UBSAN)

## Compiler

- Clang / LLVM

## AI-Assisted Analysis

Some projects integrate **AI-assisted code analysis** to improve target selection and fuzzing efficiency.

Tools used:

- Qwen (Alibaba Cloud LLM)
- OpenHands (agent-based code exploration framework)

AI is used to:
- analyze repository structure
- identify input entry points
- trace data flow through parsing logic
- highlight potential vulnerability patterns
- suggest promising fuzzing targets

---

# Fuzzing Workflow

Typical workflow used in these projects:

1. Select a real-world target library
2. Perform static exploration (manual + AI-assisted analysis)
3. Identify high-risk parsing or input-processing APIs
4. Design fuzzing harnesses for those APIs
5. Create seed corpus and dictionaries
6. Compile with sanitizers enabled
7. Run coverage-guided fuzzing (libFuzzer or AFL++)
8. Improve harness design to increase coverage
9. Minimize corpus and analyze results

---

# Projects

## TinyXML2 Fuzzing

Folder:

tinyxml2-fuzzing/

Target library:

https://github.com/leethomason/tinyxml2

TinyXML2 is a lightweight C++ XML parser that builds a DOM tree from XML input.

The fuzzing experiments in this project explore different parsing paths and DOM-processing behavior.

Experiments include:

- fuzzing `XMLDocument::Parse()`
- fuzzing `XMLDocument::LoadFile()`
- traversing the DOM after parsing
- experimenting with whitespace parsing modes
- designing an XML seed corpus and dictionary
- corpus minimization

Harnesses implemented:

- `harness.cpp` — fuzzes `Parse()`
- `harness_dom.cpp` — parses XML and traverses the DOM
- `harness_loadfile.cpp` — fuzzes file-based parsing

---

## libyaml Fuzzing

Folder:

libyaml-fuzzing/

Target library:

https://github.com/yaml/libyaml

libyaml is a C library for parsing and emitting YAML documents.

This fuzzing project explores several parts of the libyaml parsing and emitting pipeline.

Fuzzing targets include:

- `yaml_parser_scan()` — scanner/tokenizer layer
- `yaml_parser_parse()` — parser event pipeline
- `yaml_parser_load()` — document composition layer
- `yaml_emitter_emit()` — YAML emission logic
- nested emitter harness for structured YAML generation
- parse → emit round-trip harness

The round-trip harness parses YAML input and re-emits the parsed events, which helps exercise both parsing and emitting code paths.

---

## toml++ Fuzzing

Folder:

tomlplusplus-fuzzing/

Target library:

https://github.com/marzer/tomlplusplus

toml++ is a modern C++ TOML parser and formatter.

The fuzzing work in this project targets multiple parts of the parsing and formatting pipeline.

Implemented harnesses include:

- fuzzing the main TOML parser
- parsing and formatting back to TOML
- parsing and converting to JSON
- parsing and converting to YAML
- exercising the `at_path()` lookup API

Seed corpora were created for each harness to guide the fuzzer toward meaningful parsing paths.

During fuzzing, sanitizer findings revealed **two independent bug families related to undefined behavior in the parser and Unicode handling logic**.

These issues were discovered through fuzzing and reported upstream.

---

## pugixml Fuzzing

Folder:

pugixml/

Target library:

https://github.com/zeux/pugixml

pugixml is a high-performance C++ XML parsing library.

This project combines **AI-assisted analysis** and **coverage-guided fuzzing (AFL++)** to explore its parsing logic.

### AI-assisted target selection

Qwen + OpenHands were used to:
- identify XML input entry points
- trace parsing logic (`load_buffer → parse_tree`)
- locate complex subsystems (DOCTYPE, entities, escapes)
- suggest fuzzing targets based on code structure

### Fuzzing targets

- DOCTYPE parsing (recursive structures)
- Entity / numeric reference parsing
- Attribute and PCDATA handling

### Harnesses

- `fuzz_doctype.cpp` — targets DOCTYPE parsing
- `fuzz_entities.cpp` — targets entity and numeric reference parsing
- `fuzz_attr_pcdata.cpp` — targets attribute and text processing

### Fuzzing setup

- AFL++
- AddressSanitizer
- custom dictionaries
- minimized seed corpus

### Result

- No crashes discovered during fuzzing campaigns
- Stable execution with consistent behavior
- Good path exploration achieved

This project demonstrates:
- integration of AI into fuzzing workflows
- targeted harness design
- evaluation of parser robustness

---

## libexif Fuzzing

Folder:

libexif/

Target library:

https://github.com/libexif/libexif

libexif is a C library for parsing, editing, and saving EXIF metadata embedded in image files.

This project focuses on fuzzing EXIF parsing logic using AFL++ and persistent-mode harnesses.

### Fuzzing targets

- `exif_data_new_from_data()` — direct EXIF parsing from raw buffers
- `ExifLoader` — incremental parsing pipeline (`exif_loader_write`, `exif_loader_get_data`)
- parse → traverse → serialize workflow
- byte-order mutation followed by serialization

### Harnesses

- `afl-libexif-harness.c` — direct parser harness with traversal, maker note processing, and serialization
- `afl-libexif-loader-harness.c` — loader-based harness simulating real-world ingestion paths

### Fuzzing setup

- AFL++
- AddressSanitizer
- persistent mode (`__AFL_LOOP`)
- custom EXIF/JPEG dictionary
- minimized corpus using `afl-cmin`

### Result

- No crashes or hangs observed
- Stable execution (100% stability)
- ~25k exec/sec throughput
- ~2500+ corpus inputs generated
- 500+ new edges discovered

This project demonstrates:
- fuzzing of binary metadata formats
- multi-target harness design
- importance of seed diversity for structured inputs
- limitations of mutation-based fuzzing on complex formats like EXIF

---

# Repository Contents

Each project folder typically includes:

- fuzzing harness source code
- seed corpus used for fuzzing (minimized)
- dictionaries (if applicable)
- build instructions and documentation

Large fuzzing artifacts such as generated corpora, crash files, and coverage outputs are intentionally excluded.

---

# Future Targets

Planned fuzzing experiments include:

- more complex real-world software components
- deeper structure-aware fuzzing techniques
- grammar-based fuzzing for complex formats

---

# Purpose

This repository serves several purposes:

- learning and practicing fuzzing techniques
- exploring internal behavior of real-world parsers
- experimenting with harness design and coverage improvement
- integrating AI-assisted analysis into vulnerability research
- building a portfolio of fuzzing and security research projects

The focus is on **methodology, workflow, and coverage exploration**, while documenting interesting findings during fuzzing.
