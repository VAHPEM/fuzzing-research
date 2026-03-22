#include "target/tinyxml2.h"
#include <cstddef>
#include <cstdint>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size == 0 || size > 2048) return 0;

    tinyxml2::XMLDocument doc;
    doc.Parse(reinterpret_cast<const char*>(data), size);

    tinyxml2::XMLElement* root = doc.RootElement();
    if (root) {
        root->Name();
        root->GetText();

        for (const tinyxml2::XMLAttribute* a = root->FirstAttribute(); a; a = a->Next()) {
            a->Name();
            a->Value();
        }

        for (tinyxml2::XMLNode* child = root->FirstChild(); child; child = child->NextSibling()) {
            child->Value();
        }
    }

    return 0;
}
