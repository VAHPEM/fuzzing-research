#include <cstdint>
#include <cstddef>
#include <string>
#include <sstream>
#include <toml++/toml.hpp>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    try
    {
        std::string input(reinterpret_cast<const char*>(data), size);
        auto tbl = toml::parse(input);

        std::ostringstream oss;
        oss << toml::json_formatter{ tbl };
        auto out = oss.str();
        (void)out;
    }
    catch (...)
    {
    }

    return 0;
}
