#include "tinyxml2.h"
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    if (size == 0 || size > 2048) return 0;

    char filename[] = "/tmp/tinyxml2_fuzz_XXXXXX";
    int fd = mkstemp(filename);
    if (fd == -1) return 0;

    ssize_t written = write(fd, data, size);
    close(fd);

    if (written < 0 || static_cast<size_t>(written) != size) {
        unlink(filename);
        return 0;
    }

    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename);

    unlink(filename);
    return 0;
}
