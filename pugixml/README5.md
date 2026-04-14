# pugixml Fuzzing Research

This project contains coverage-guided fuzzing experiments on pugixml, a high-performance C++ XML parsing library.

The goal of this project is to analyze the internal parsing logic of pugixml, identify high-risk attack surfaces, and apply modern fuzzing techniques using AFL++ and sanitizers.

This work is part of a personal fuzzing research repository focused on real-world software.

---

## AI-Assisted Analysis

Before fuzzing, the repository was analyzed using the Qwen LLM (Alibaba Cloud Model Studio).

The AI was used to:
- Identify untrusted input entry points
- Trace XML data flow through parsing functions
- Highlight memory allocation patterns
- Detect potential vulnerability classes (integer overflow, recursion depth, buffer assumptions)
- Suggest promising fuzzing targets

This significantly reduces manual reverse engineering time and improves target selection quality.

---

## Workflow

The fuzzing workflow used in this project:

1. Static exploration (manual + AI-assisted)
2. Identification of attack surfaces (entry points & parsing layers)
3. Selection of high-risk targets
4. Custom harness development
5. Seed corpus creation and minimization
6. AFL++ fuzzing with ASan
7. Coverage and behavior observation

Pipeline:

AI Analysis → Target Selection → Harness → Seeds → AFL++ → Evaluation

---

## Targets

The following parsing components were selected based on AI-assisted analysis:

### 1. DOCTYPE Parsing
Handles nested structures such as:
- <!DOCTYPE>
- <!ENTITY>
- <![IGNORE[ ... ]]>

Risk:
- Deep recursion
- Stack exhaustion
- Complex state transitions

---

### 2. Entity / Character Reference Parsing
Processes inputs like:
- &#123;
- &#x41;
- malformed or oversized numeric references

Risk:
- Integer overflow
- Unicode edge cases
- Encoding conversion issues

---

### 3. Attribute & PCDATA Processing
Handles:
- XML attributes
- text nodes (PCDATA)
- escape sequences (&amp;, etc.)

Risk:
- String processing errors
- buffer miscalculations
- unexpected input combinations

---

## Harnesses

The project contains the following fuzzing harnesses:

### fuzz_doctype.cpp
Targets:
- DOCTYPE parsing logic via xml_document::load_buffer

Focus:
- deeply nested XML structures
- malformed declarations

---

### fuzz_entities.cpp
Targets:
- numeric and symbolic entity parsing

Focus:
- extreme values (e.g., &#4294967295;)
- invalid encodings

---

### fuzz_attr_pcdata.cpp
Targets:
- attribute parsing and PCDATA handling

Focus:
- malformed attributes
- escape handling
- mixed content

---

## Seed Corpus

Seed inputs were:
- manually crafted based on XML structure knowledge
- guided by AI-identified edge cases
- minimized using AFL++ corpus minimization tools

Examples include:
- nested DOCTYPE declarations
- extreme numeric entities
- malformed attributes and text nodes

---

## Build Example

Example compilation using AFL++ + AddressSanitizer:

AFL_USE_ASAN=1 afl-clang-lto++ -O1 -g -fsanitize=address \
-I./src fuzz_doctype.cpp src/pugixml.cpp -o fuzz_doctype

---

## Run Fuzzing

afl-fuzz -i seeds -o findings -- ./fuzz_doctype @@

Other targets:

afl-fuzz -i seeds_entities -o findings_entities -- ./fuzz_entities @@  
afl-fuzz -i seeds_attr -o findings_attr -- ./fuzz_attr_pcdata @@

---

## Results

- No crashes discovered during fuzzing campaigns
- Stable execution with consistent behavior
- Good path exploration and input diversity observed

Example metrics:
- ~2M–4M executions per run
- stable coverage growth
- no hangs or memory errors detected

---

## Potential Issues (from analysis)

- Integer overflow risk in size calculations: (length + 1) * sizeof(char_t)
- Deep recursion in DOCTYPE parsing
- Assumptions about mutable input buffers
- Limited bounds checking in string operations

---

## Notes

This project focuses on fuzzing methodology, target selection, and coverage exploration.

The use of AI-assisted analysis demonstrates an approach to scaling vulnerability research by combining static reasoning with dynamic fuzzing.

No confirmed vulnerabilities are claimed in this project.

The harnesses and workflow may serve as a foundation for further fuzzing research or integration into automated fuzzing systems.
