#include <cstring>
#include <iostream>
#include <filesystem>

#include "cpu.h"
#include "fb_renderer.h"
#include "ppu.h"
#include "window.h"

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_MULTIPLIER 3
#define WINDOW_WIDTH (SCREEN_WIDTH * SCREEN_MULTIPLIER)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * SCREEN_MULTIPLIER)

int main(int argc, char* argv[])
{
    window win{SCREEN_WIDTH * SCREEN_MULTIPLIER, SCREEN_HEIGHT * SCREEN_MULTIPLIER, "gbemu"};
    gb::memory_map mem{};
    //mem.skip_boot_rom();
    gb::cpu cpu{};
    gb::ppu ppu{};

    fb_renderer renderer{};

    bool skip_rom_execution = false;

    if (argc == 2 && std::filesystem::exists(argv[1]))
    {
        mem.load_rom(std::filesystem::absolute(argv[1]));
    }
    else if (argc == 2 && !std::filesystem::exists(argv[1]))
    {
        std::cout << "Usage: app.exe <rom absolute path>" << std::endl;
        return -1;
    }
    else
    {
        skip_rom_execution = true;
        std::cout << "Skipping rom loading" << std::endl;
    }

    while (!win.should_close())
    {
        if (!skip_rom_execution)
        {
            try
            {
                const uint32_t cycles = cpu.execute(mem);
                ppu.tick(cycles, mem);
            }
            catch (const std::exception& e)
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
