#include <iostream>

#include "cpu.h"

int main(int argc, char* argv[])
{
    gb::memory_map mem {};
    mem.skip_boot_rom();
    gb::cpu cpu {};

    if (!std::filesystem::exists(argv[1]))
    {
        std::cerr << "File does not exist: " << std::filesystem::absolute(argv[1]) << std::endl;
        return -1;
    }
    else if (argc == 2)
        mem.load_rom(std::filesystem::absolute(argv[1]));
    else
    {
        std::cerr << "Usage: app.exe <rom absolute path>" << std::endl;
        return -1;
    }

    uint32_t cycles = 0;

    while (true)
    {
        try
        {
            cycles += cpu.execute(mem);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    return 0;
}
