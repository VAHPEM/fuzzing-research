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

        auto p1 = tbl.at_path("a");
        auto p2 = tbl.at_path("a.b");
        auto p3 = tbl.at_path("server.port");
        auto p4 = tbl.at_path("nums[0]");
        auto p5 = tbl.at_path("x.y.z");

        (void)p1;
        (void)p2;
        (void)p3;
        (void)p4;
        (void)p5;
    }
    catch (...)
    {
    }

    return 0;
}
