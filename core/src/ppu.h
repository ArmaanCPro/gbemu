#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <array>
#include "memory_map.h"

#include <iostream>

namespace gb
{
    class ppu;
}

enum class ppu_mode
{
    HBlank,     // H-Blank period
    VBlank,     // V-Blank period
    OAM,        // Search OAM for sprites on current scanline
    Drawing     // Pixel transfer
};

class gb::ppu
{
public:
    ppu();
    ~ppu() = default;

    void tick(uint32_t cycles, memory_map& mem);
    [[nodiscard]] const uint32_t* get_framebuffer() const { return framebuffer_; }

    void debug_print_framebuffer() const {
        int non_zero_pixels = 0;
        uint32_t first_non_zero = 0;

        for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
            if (framebuffer_[i] != 0) {
                non_zero_pixels++;
                if (!first_non_zero) first_non_zero = framebuffer_[i];
            }
        }

        std::cout << "Non-zero pixels: " << non_zero_pixels << "\n";
        if (first_non_zero) {
            std::cout << "First non-zero pixel value: 0x" << std::hex << first_non_zero << std::dec << "\n";
        }
    }


private:
    static constexpr int SCREEN_WIDTH = 160;
    static constexpr int SCREEN_HEIGHT = 144;
    static constexpr uint32_t CYCLES_OAM = 80;
    static constexpr uint32_t CYCLES_DRAWING = 172;
    static constexpr uint32_t CYCLES_HBLANK = 204;
    static constexpr uint32_t CYCLES_LINE = 456;
    static constexpr uint8_t TOTAL_LINES = 154;

    // LCD registers addresses
    static constexpr uint16_t LCDC_ADDR = 0xFF40;
    static constexpr uint16_t STAT_ADDR = 0xFF41;
    static constexpr uint16_t SCY_ADDR = 0xFF42;
    static constexpr uint16_t SCX_ADDR = 0xFF43;
    static constexpr uint16_t LY_ADDR = 0xFF44;
    static constexpr uint16_t LYC_ADDR = 0xFF45;
    static constexpr uint16_t BGP_ADDR = 0xFF47;
    static constexpr uint16_t OBP0_ADDR = 0xFF48;
    static constexpr uint16_t OBP1_ADDR = 0xFF49;
    static constexpr uint16_t WY_ADDR = 0xFF4A;
    static constexpr uint16_t WX_ADDR = 0xFF4B;

    uint32_t cyclecounter_ {0};
    uint8_t currentline_ {0};
    ppu_mode mode_ { ppu_mode::OAM };
    uint32_t framebuffer_[SCREEN_WIDTH * SCREEN_HEIGHT] {};

    void render_scanline(memory_map& mem);
    void render_background(memory_map& mem, int scanline);
    void render_window(memory_map& mem, int scanline);
    void render_sprites(memory_map& mem, int scanline);
    void update_mode(memory_map& mem);

    [[nodiscard]] uint32_t get_color(uint8_t color_id, uint8_t palette) const;
    [[nodiscard]] bool is_lcd_enabled(uint8_t lcdc) const { return lcdc & 0x80; }
    [[nodiscard]] bool is_window_enabled(uint8_t lcdc) const { return lcdc & 0x20; }
    [[nodiscard]] bool is_sprites_enabled(uint8_t lcdc) const { return lcdc & 0x02; }
    [[nodiscard]] bool is_bg_enabled(uint8_t lcdc) const { return lcdc & 0x01; }
};
