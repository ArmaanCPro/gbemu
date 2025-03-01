#include <iostream>

#include "cpu.h"
#include "window.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_MULTIPLIER 5

int main(int argc, char* argv[])
{
    window win { SCREEN_WIDTH * SCREEN_MULTIPLIER, SCREEN_HEIGHT * SCREEN_MULTIPLIER, "gbemu" };
    gb::memory_map mem {};
    mem.skip_boot_rom();
    gb::cpu cpu {};

    if (argc == 2 && !std::filesystem::exists(argv[1]))
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

    while (!win.should_close())
    {
        try
        {
            cycles += cpu.execute(mem);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        win.swap_buffers();
        win.poll_events();
    }

    return 0;
}
