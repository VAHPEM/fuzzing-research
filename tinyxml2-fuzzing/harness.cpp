#include "target/tinyxml2.h"
#include <cstdint>
#include <cstddef>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size < 1 || size > 2048) return 0;

    tinyxml2::XMLDocument doc;
    doc.Parse(reinterpret_cast<const char*>(data + 1), size - 1);

    return 0;
}
