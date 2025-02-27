#pragma once

#include <cstdint>
#include <array>
#include <fstream>

// sizes
#define ROM_SIZE  (0x8000)      // 32 KB        (0x0000-0x7FFF)
#define WRAM_SIZE (8 * 1024)    // 8 KB         (0xC000-0xDFFF)
#define HRAM_SIZE (127)         // 127 bytes    (0xFF80-0xFFFE)
#define VRAM_SIZE (8 * 1024)     // 8 KB         (0x8000-0x9FFF)

// address locations
#define ROM_START   0x0000
#define ROM_END     0x7FFF
#define WRAM_START  0xC000
#define WRAM_END    0xDFFF
#define HRAM_START  0xFF80
#define HRAM_END    0xFFFE
#define VRAM_START  0x8000
#define VRAM_END    0x9FFF

namespace gb
{
    class memory_map;
}

class gb::memory_map
{

public:
    memory_map()
    : // shady initialization code. maybe not though
    rom(std::array<uint8_t, ROM_SIZE>{}),
    wram(std::array<uint8_t, WRAM_SIZE>{}),
    hram(std::array<uint8_t, HRAM_SIZE>{}),
    vram(std::array<uint8_t, VRAM_SIZE>{})
    {
        rom.fill(0);
        wram.fill(0);
        hram.fill(0);
        vram.fill(0);
    }


    [[nodiscard]] uint8_t read(uint16_t address) const
    {
        if (address <= ROM_END)
        {
            return rom[address];
        }
        else if (address >= WRAM_START && address <= WRAM_END)
        {
            return wram[address - WRAM_START];
        }
        else if (address >= HRAM_START && address <= HRAM_END)
        {
            return hram[address - HRAM_START];
        }
        return 0xFF;
    }

    void write(uint16_t address, uint8_t value)
    {
        // rom area is read-only, so no writes
        if (address >= WRAM_START && address <= WRAM_END)
        {
            wram[address - WRAM_START] = value;
        }
        else if (address >= HRAM_START && address <= HRAM_END)
        {
            hram[address - HRAM_START] = value;
        }
    }

    void load_rom(const std::string &rom_path)
    {
        std::ifstream rom_file(rom_path, std::ios::binary);
        if (!rom_file)
        {
            throw std::runtime_error("Error loading ROM!");
        }
        rom_file.read(reinterpret_cast<char*>(rom.data()), rom.size());
    }

private:
    std::array<uint8_t, ROM_SIZE> rom;
    std::array<uint8_t, WRAM_SIZE> wram;
    std::array<uint8_t, HRAM_SIZE> hram;
    std::array<uint8_t, VRAM_SIZE> vram;
};
