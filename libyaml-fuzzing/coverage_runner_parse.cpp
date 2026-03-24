#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <stdint.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        std::ifstream file(argv[i], std::ios::binary);
        if (!file) continue;

        std::vector<uint8_t> data(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>());

        LLVMFuzzerTestOneInput(data.data(), data.size());
    }
    return 0;
}
