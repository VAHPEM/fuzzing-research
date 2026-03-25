#include <cstdint>
#include <cstddef>
#include <string>
#include <toml++/toml.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    try
    {
        std::string input(reinterpret_cast<const char*>(data), size);
        auto tbl = toml::parse(input);
        (void)tbl;
    }
    catch (...)
    {
    }

    return 0;
}
