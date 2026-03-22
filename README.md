This project documents my fuzzing experiments on [TinyXML2](https://github.com/leethomason/tinyxml2), a lightweight C++ XML parser.

## Goal

The goal of this project was to practice coverage-guided fuzzing on a real-world parser and document the workflow for portfolio and learning purposes.

## Target

- Project: TinyXML2
- Language: C++
- Main parser API: `tinyxml2::XMLDocument::Parse()`

## Harnesses

This project includes multiple fuzzing harnesses:

- `harness.cpp`  
  Fuzzes `XMLDocument::Parse()` directly.

- `harness_dom.cpp`  
  Parses XML and then accesses DOM-related APIs such as root element, attributes, and child nodes.

- `harness_loadfile.cpp`  
  Writes the fuzz input to a temporary file and fuzzes `XMLDocument::LoadFile()`.

## Build

```bash
./build.sh
