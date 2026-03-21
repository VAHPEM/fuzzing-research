# fuzzing-research
This repository documents my experiments with coverage-guided fuzzing on open-source projects.

Each project includes:

- fuzz harness
- build instructions
- fuzzing setup
- crash analysis
- technical write-up

## Projects

| Project | Target | Status |
|-------|------|------|
| TinyXML2 | XML parser | In progress |
| libyaml | YAML parser | Planned |
| libexif | EXIF parser | Planned |

## Tools

- libFuzzer
- AFL++
- AddressSanitizer
- UBSan
- GDB / LLDB
