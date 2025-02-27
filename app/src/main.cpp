#include <iostream>

#include "cpu.h"

int main(int argc, char* argv[])
{
    gb::memory_map mem {};
    gb::cpu cpu { mem };

    if (!std::filesystem::exists(argv[1]))
        std::cerr << "File does not exist: " << std::filesystem::absolute(argv[1]) << std::endl;

    else if (argc == 2)
        mem.load_rom(std::filesystem::absolute(argv[1]));

    else
    {
        std::cerr << "Usage: app.exe <rom absolute path>" << std::endl;
        //return -1;
    }

    while (true)
    {
        try
        {
            cpu.execute(mem);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    return 0;
}
