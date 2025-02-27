#include <iostream>

#include "gb.h"

int main(int argc, char* argv[])
{
    gb::work_ram mem;
    gb::cpu cpu { mem };


    if (argc == 2)
    {
        cpu.load_rom(argv[1], mem);
    }
    else
    {
        std::cerr << "Usage: app.exe <rom_path>" << std::endl;
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
