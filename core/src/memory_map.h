#pragma once

#include <cstdint>
#include <array>
#include <fstream>

#define ROM_SIZE  (0x8000)      // 32 KB        (0x0000-0x7FFF)
#define WRAM_SIZE (8 * 1024)    // 8 KB         (0xC000-0xDFFF)
#define HRAM_SIZE (127)         // 127 bytes    (0xFF80-0xFFFE)

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
    hram(std::array<uint8_t, HRAM_SIZE>{})
    {
        rom.fill(0);
        wram.fill(0);
        hram.fill(0);
    }


    [[nodiscard]] uint8_t read(uint16_t address) const
    {
        if (address <= 0x7FFF)
        {
            return rom[address];
        }
        else if (address >= 0xC000 && address <= 0xDFFF)
        {
            return wram[address - 0xC000];
        }
        else if (address >= 0xFF80 && address <= 0xFFFE)
        {
            return hram[address - 0xFF80];
        }
        return 0xFF;
    }

    void write(uint16_t address, uint8_t value)
    {
        if (address >= 0xC000 && address <= 0xDFFF)
        {
            wram[address - 0xC000] = value;
        }
        else if (address >= 0xFF80 && address <= 0xFFFE)
        {
            hram[address - 0xFF80] = value;
        }
        // rom area is read-only, so no writes
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
};
