# Fuzzing Research

This repository documents my experiments with **coverage-guided fuzzing on real-world libraries**.

The goal of this project is to practice fuzzing techniques, understand how parsers process complex inputs, and design effective fuzzing harnesses using **libFuzzer and sanitizers**.

Each library is fuzzed in a separate folder with its own harness, corpus, and build scripts.

---

# Fuzzing Setup

Most projects in this repository use the following tools:

- libFuzzer
- AddressSanitizer (ASAN)
- UndefinedBehaviorSanitizer (UBSAN)
- Clang

Typical workflow:

1. Select a target library
2. Identify interesting parsing APIs
3. Design fuzzing harnesses
4. Create seed corpus and dictionary
5. Compile with sanitizers
6. Run coverage-guided fuzzing
7. Minimize corpus and document results

---

# Projects

## TinyXML2 Fuzzing

Location:

tinyxml2-fuzzing/

Target library:

https://github.com/leethomason/tinyxml2

TinyXML2 is a lightweight C++ XML parser that builds a DOM structure from XML input.

The fuzzing experiments in this project include:

- fuzzing XMLDocument::Parse()
- fuzzing XMLDocument::LoadFile()
- DOM traversal after parsing
- experimenting with different TinyXML2 whitespace modes
- designing seed corpus and XML dictionary
- corpus minimization

Harnesses implemented:

- harness.cpp – fuzzes Parse()
- harness_dom.cpp – parses XML and traverses DOM
- harness_loadfile.cpp – fuzzes file-based parsing

---

# Repository Structure

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
└ (future fuzzing targets)

Each fuzzing target contains:

- source code of the library (target/)
- fuzzing harness
- seed corpus
- optional dictionary
- build script

---

# Future Targets

Planned fuzzing experiments include:

- libyaml
- expat
- pugixml
- other parsing libraries

---

# Purpose

This repository serves as:

- a learning project for fuzzing techniques
- a portfolio of security research experiments
- a reference for designing fuzzing harnesses for parser libraries
