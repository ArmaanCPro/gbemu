#include "ppu.h"

// this is a class similar to a renderer in a game engine. it doesn't actually manage the "os window"
gb::ppu::ppu()
{

}

void gb::ppu::tick(uint32_t cycles, memory_map& mem)
{
    if (!is_lcd_enabled(mem.read(LCDC_ADDR)))
        return;

    cyclecounter_ += cycles;

    if (cyclecounter_ >= CYCLES_LINE)
    {
        cyclecounter_ -= CYCLES_LINE;
        currentline_++;

        if (currentline_ >= TOTAL_LINES)
            currentline_ = 0;

        mem.write(LY_ADDR, currentline_);

        if (currentline_ < 144)
        {
            render_scanline(mem);
        }
        else if (currentline_ == 144)
        {
            mode_ = ppu_mode::VBlank;
            //mem.request_interrupt(0x01); // Request VBlank interrupt
        }
    }

    update_mode(mem);
}

void gb::ppu::render_scanline(memory_map& mem)
{
    const uint8_t lcdc = mem.read(LCDC_ADDR);

    if (is_bg_enabled(lcdc))
        render_background(mem, currentline_);
    if (is_window_enabled(lcdc))
        render_window(mem, currentline_);
    if (is_sprites_enabled(lcdc))
        render_sprites(mem, currentline_);
}

void gb::ppu::render_background(memory_map& mem, int scanline)
{
    const uint8_t lcdc = mem.read(LCDC_ADDR);
    const uint8_t scx = mem.read(SCX_ADDR);
    const uint8_t scy = mem.read(SCY_ADDR);
    const uint16_t tile_map = (lcdc & 0x08) ? 0x9C00 : 0x9800;
    const uint16_t tile_data = (lcdc & 0x10) ? 0x8000 : 0x8800;
    const bool signed_addressing = !(lcdc & 0x10);
    const uint8_t bg_palette = mem.read(BGP_ADDR);

    const uint8_t y = (scanline + scy) & 255;
    const uint8_t tile_row = y / 8;

    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        const uint8_t mapped_x = (x + scx) & 255;
        const uint8_t tile_col = mapped_x / 8;
        const uint16_t tile_addr = tile_map + tile_row * 32 + tile_col;

        uint8_t tile_id = mem.read(tile_addr);
        if (signed_addressing)
            tile_id = static_cast<int8_t>(tile_id) + 128;

        const uint16_t tile_location = tile_data + tile_id * 16;
        const uint8_t line = (y % 8) * 2;
        const uint8_t byte1 = mem.read(tile_location + line);
        const uint8_t byte2 = mem.read(tile_location + line + 1);

        const int bit_num = 7 - (mapped_x % 8);
        const uint8_t color_id = ((byte1 >> bit_num) & 1) | (((byte2 >> bit_num) & 1) << 1);

        framebuffer_[scanline * SCREEN_WIDTH + x] = get_color(color_id, bg_palette);
    }
}

void gb::ppu::render_window(memory_map& mem, int scanline)
{
    const uint8_t wy = mem.read(WY_ADDR);
    if (scanline < wy)
        return;

    const uint8_t wx = mem.read(WX_ADDR);
    const uint8_t lcdc = mem.read(LCDC_ADDR);
    const uint16_t tile_map = (lcdc & 0x40) ? 0x9C00 : 0x9800;
    const uint16_t tile_data = (lcdc & 0x10) ? 0x8000 : 0x8800;
    const bool signed_addressing = !(lcdc & 0x10);
    const uint8_t bg_palette = mem.read(BGP_ADDR);

    const uint8_t window_line = scanline - wy;
    const uint8_t tile_row = window_line / 8;

    for (int x = 0; x < SCREEN_WIDTH - wx + 7; x++)
    {
        if (x + wx < 7) continue;

        const uint8_t tile_col = x / 8;
        const uint16_t tile_addr = tile_map + tile_row * 32 + tile_col;

        uint8_t tile_id = mem.read(tile_addr);
        if (signed_addressing)
            tile_id = static_cast<int8_t>(tile_id) + 128;

        const uint16_t tile_location = tile_data + tile_id * 16;
        const uint8_t line = (window_line % 8) * 2;
        const uint8_t byte1 = mem.read(tile_location + line);
        const uint8_t byte2 = mem.read(tile_location + line + 1);

        const int bit_num = 7 - (x % 8);
        const uint8_t color_id = ((byte1 >> bit_num) & 1) | (((byte2 >> bit_num) & 1) << 1);

        const int screen_x = x + wx - 7;
        if (screen_x >= 0 && screen_x < SCREEN_WIDTH)
            framebuffer_[scanline * SCREEN_WIDTH + screen_x] = get_color(color_id, bg_palette);
    }
}

void gb::ppu::render_sprites(memory_map& mem, int scanline)
{
    const uint8_t lcdc = mem.read(LCDC_ADDR);
    const bool tall_sprites = lcdc & 0x04;
    const int sprite_height = tall_sprites ? 16 : 8;

    struct sprite {
        uint8_t y;
        uint8_t x;
        uint8_t tile;
        uint8_t attributes;
    };

    std::array<sprite, 40> sprites;
    for (int i = 0; i < 40; i++)
    {
        const uint16_t sprite_addr = 0xFE00 + i * 4;
        sprites[i].y = mem.read(sprite_addr) - 16;
        sprites[i].x = mem.read(sprite_addr + 1) - 8;
        sprites[i].tile = mem.read(sprite_addr + 2);
        sprites[i].attributes = mem.read(sprite_addr + 3);
    }

    int sprites_on_line = 0;
    for (const auto& sprite : sprites)
    {
        if (sprites_on_line >= 10)
            break;

        if (scanline < sprite.y || scanline >= sprite.y + sprite_height)
            continue;

        sprites_on_line++;

        const bool flip_y = sprite.attributes & 0x40;
        const bool flip_x = sprite.attributes & 0x20;
        const uint8_t palette = sprite.attributes & 0x10 ? mem.read(OBP1_ADDR) : mem.read(OBP0_ADDR);

        uint8_t line = scanline - sprite.y;
        if (flip_y)
            line = sprite_height - 1 - line;

        const uint16_t tile_addr = 0x8000 + sprite.tile * 16 + (line * 2);
        const uint8_t byte1 = mem.read(tile_addr);
        const uint8_t byte2 = mem.read(tile_addr + 1);

        for (int x = 0; x < 8; x++)
        {
            if (sprite.x + x < 0 || sprite.x + x >= SCREEN_WIDTH)
                continue;

            const int bit = flip_x ? x : 7 - x;
            const uint8_t color_id = ((byte1 >> bit) & 1) | (((byte2 >> bit) & 1) << 1);

            if (color_id == 0)  // Transparent pixel
                continue;

            framebuffer_[scanline * SCREEN_WIDTH + sprite.x + x] = get_color(color_id, palette);
        }
    }
}

void gb::ppu::update_mode(memory_map& mem)
{
    uint8_t stat = mem.read(STAT_ADDR) & 0xFC;  // Clear mode bits

    switch (mode_)
    {
        case ppu_mode::OAM:
            stat |= 0x02;
        if (cyclecounter_ >= CYCLES_OAM)
        {
            mode_ = ppu_mode::Drawing;
            stat |= 0x03;
        }
        break;

        case ppu_mode::Drawing:
            stat |= 0x03;
        if (cyclecounter_ >= CYCLES_OAM + CYCLES_DRAWING)
        {
            mode_ = ppu_mode::HBlank;
            stat |= 0x00;
        }
        break;

        case ppu_mode::HBlank:
            stat |= 0x00;
        break;

        case ppu_mode::VBlank:
            stat |= 0x01;
        if (currentline_ >= TOTAL_LINES)
        {
            mode_ = ppu_mode::OAM;
            stat |= 0x02;
            currentline_ = 0;
        }
        break;
    }

    // LYC comparison
    if (currentline_ == mem.read(LYC_ADDR))
    {
        stat |= 0x04;
        //if (stat & 0x40)
            //mem.request_interrupt(0x02);  // STAT interrupt
    }

    mem.write(STAT_ADDR, stat);
}

uint32_t gb::ppu::get_color(uint8_t color_id, uint8_t palette) const
{
    const uint8_t shade = (palette >> (color_id * 2)) & 0x03;

    switch (shade)
    {
        case 0: return 0xFFFFFFFF;  // White
        case 1: return 0xFFAAAAAA;  // Light gray
        case 2: return 0xFF555555;  // Dark gray
        case 3: return 0xFF000000;  // Black
        default: return 0xFFFFFFFF;
    }
}
