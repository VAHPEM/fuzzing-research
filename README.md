# Fuzzing Research

This repository contains my experiments with **coverage-guided fuzzing on real-world software libraries**.

The goal of this project is to explore how complex input parsers behave under fuzzing, practice building effective fuzzing harnesses, and study how modern fuzzing tools can be used to discover memory safety issues in widely used libraries.

All experiments use **coverage-guided fuzzing with libFuzzer**, combined with runtime sanitizers such as AddressSanitizer and UndefinedBehaviorSanitizer.

---

# Fuzzing Environment

Most projects in this repository use the following toolchain:

- **libFuzzer**
- **AddressSanitizer (ASAN)**
- **UndefinedBehaviorSanitizer (UBSAN)**
- **Clang / LLVM**

Typical fuzzing workflow:

1. Select a real-world target library
2. Identify interesting parsing or input-processing APIs
3. Design fuzzing harnesses for those APIs
4. Create a seed corpus and optional dictionaries
5. Compile the target with sanitizers enabled
6. Run coverage-guided fuzzing
7. Improve harness design to increase code coverage
8. Minimize the corpus and document results

---

# Projects

## TinyXML2 Fuzzing

Location:

```
tinyxml2-fuzzing/
```

Target library:

https://github.com/leethomason/tinyxml2

TinyXML2 is a lightweight C++ XML parser that builds a DOM tree from XML input.

The fuzzing experiments in this project focus on exploring different parsing paths and DOM processing behavior.

Experiments include:

- fuzzing `XMLDocument::Parse()`
- fuzzing `XMLDocument::LoadFile()`
- traversing the DOM after parsing
- experimenting with TinyXML2 whitespace parsing modes
- designing an XML seed corpus and dictionary
- corpus minimization

Harnesses implemented:

- `harness.cpp` — fuzzes `Parse()`
- `harness_dom.cpp` — parses XML and traverses the DOM
- `harness_loadfile.cpp` — fuzzes file-based parsing

---

## libyaml Fuzzing

Location:

```
libyaml-fuzzing/
```

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

The round-trip harness parses YAML input and re-emits the parsed events, which significantly increases emitter-side code coverage.

---

# Repository Structure

```
fuzzing-research/
│
├ README.md
│
├ tinyxml2-fuzzing/
│   ├ corpus/
│   ├ target/
│   ├ harness.cpp
│   ├ harness_dom.cpp
│   ├ harness_loadfile.cpp
│   ├ xml.dict
│   └ build.sh
│
├ libyaml-fuzzing/
│   ├ harness_scan.cpp
│   ├ harness_parse.cpp
│   ├ harness_load.cpp
│   ├ harness_emit.cpp
│   ├ harness_emit_nested.cpp
│   ├ harness_roundtrip_parse_emit.cpp
│   └ README.md
│
└ (future fuzzing targets)
```

Each fuzzing project typically contains:

- fuzzing harness source code
- seed corpus
- optional dictionaries
- build scripts or compilation instructions
- documentation of the fuzzing setup

Large fuzzing artifacts such as generated corpora, crash artifacts, and coverage outputs are intentionally excluded from the repository.

---

# Future Targets

Planned fuzzing experiments include:

- Expat
- pugixml
- additional parsing libraries
- other input-processing components

---

# Purpose

This repository serves several purposes:

- learning and practicing fuzzing techniques
- exploring the internal behavior of real-world parsers
- experimenting with fuzz harness design
- building a portfolio of vulnerability research experiments

The focus is on **methodology, coverage exploration, and harness design**, rather than claiming vulnerabilities.
