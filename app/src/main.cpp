#include <iostream>
#include <cstring>

#include "cpu.h"
#include "fb_renderer.h"
#include "window.h"
#include "ppu.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_MULTIPLIER 3

int main(int argc, char* argv[])
{
    window win { SCREEN_WIDTH, SCREEN_HEIGHT, "gbemu" };
    gb::memory_map mem {};
    mem.skip_boot_rom();
    gb::cpu cpu{};
    gb::ppu ppu{};

    fb_renderer renderer { SCREEN_WIDTH, SCREEN_HEIGHT };

    bool skip_rom_execution = false;

    if (argc == 2 && std::filesystem::exists(argv[1]))
    {
        mem.load_rom(std::filesystem::absolute(argv[1]));
    }
    else if (argc == 2 && strcmp(argv[1], "--skip-rom") == 0)
    {
        skip_rom_execution = true;
    }
    else
    {
        std::cerr << "Usage: app.exe <rom absolute path>" << std::endl;
        return -1;
    }

    uint64_t total_cycles = 0;

    while (!win.should_close())
    {
        if (!skip_rom_execution)
        {
            try
            {
                const uint32_t cycles = cpu.execute(mem);
                ppu.tick(cycles, mem);
                ppu.debug_print_framebuffer();
                total_cycles += cycles;
            }
            catch(const std::exception& e)
            {
                std::cerr << e.what() << '\n';
            }
        }

        renderer.render(ppu.get_framebuffer(), SCREEN_WIDTH, SCREEN_HEIGHT);

        win.swap_buffers();
        win.poll_events();
    }

    return 0;
}
