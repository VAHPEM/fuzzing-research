# TinyXML2 Fuzzing

This folder documents fuzzing experiments performed on **TinyXML2**, a lightweight C++ XML parser.

The goal of this project is to practice coverage-guided fuzzing on a real-world parser, design multiple fuzzing harnesses, and explore different parsing code paths.

TinyXML2 repository:  
https://github.com/leethomason/tinyxml2

---

# Target

- Library: TinyXML2  
- Language: C++  
- Parser type: XML DOM parser  

TinyXML2 parses XML documents and builds a **Document Object Model (DOM)** that can be traversed and manipulated.

---

# Fuzzing Targets

The following APIs were fuzzed:

- XMLDocument::Parse()
- XMLDocument::LoadFile()
- DOM traversal after parsing

---

# Harnesses

## Parse Harness

File: `harness.cpp`

Directly fuzzes:

`XMLDocument::Parse()`

During experimentation, different TinyXML2 whitespace modes were tested:

- PRESERVE_WHITESPACE
- COLLAPSE_WHITESPACE
- PEDANTIC_WHITESPACE

These modes influence how whitespace is interpreted during parsing.

---

## DOM Harness

File: `harness_dom.cpp`

After parsing XML input, this harness explores additional execution paths by:

- accessing the root element
- iterating over attributes
- traversing child nodes

This helps exercise additional DOM logic inside TinyXML2.

---

## LoadFile Harness

File: `harness_loadfile.cpp`

This harness writes fuzz input to a temporary file and calls:

`XMLDocument::LoadFile()`

This allows fuzzing of file-based parsing code paths.

---

# Corpus

The fuzzing corpus contains:

- hand-written XML seed files
- minimized corpus generated during fuzzing

Seed inputs include examples with:

- minimal XML documents
- nested elements
- attributes
- comments
- CDATA sections
- entity references
- XML declarations

---

# Dictionary

`xml.dict` contains common XML tokens used to improve mutation quality.

Examples include:

<
>
</
/>
<?xml
<!--
<![CDATA[
&amp;
&# 
&#x

---

# Build

Compile the fuzzers using:

./build.sh

The build uses:

- libFuzzer
- AddressSanitizer
- UndefinedBehaviorSanitizer

---

# Running the Fuzzer

Parse harness:

./fuzz_tinyxml2 corpus -artifact_prefix=crashes/

DOM harness:

./fuzz_dom corpus -artifact_prefix=crashes_dom/

LoadFile harness:

./fuzz_loadfile corpus -artifact_prefix=crashes_loadfile/

Optional dictionary usage:

./fuzz_tinyxml2 corpus -dict=xml.dict

---

# Results

The fuzzing campaign explored multiple TinyXML2 parsing paths including:

- direct XML parsing
- DOM traversal
- file-based parsing

Coverage increased through corpus mutation and dictionary guidance.

No confirmed memory-safety vulnerabilities were observed during this campaign.

---

# Learning Outcomes

This project provided hands-on experience with:

- fuzzing harness design
- coverage-guided fuzzing with libFuzzer
- corpus design and minimization
- dictionary-guided mutations
- fuzzing parser libraries
