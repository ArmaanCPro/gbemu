#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace gb
{
    class ppu;
}

class gb::ppu
{
public:
    ppu();

    void tick();
    void renderScanline();

private:

    uint8_t lcdc;
    uint8_t stat;
    uint8_t scy, scx;
    uint8_t ly;
    uint8_t lyc;
    uint8_t wy, wx;
    uint8_t bgp;
    uint8_t obp0, obp1;
};
