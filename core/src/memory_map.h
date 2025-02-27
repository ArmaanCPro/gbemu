#pragma once
#include "../resources/dmg_boot.h"

#include <cstdint>
#include <array>
#include <vector>
#include <filesystem>
#include <fstream>

// Memory sizes
#define ROM_SIZE    (0x8000)      // 32 KB        (0x0000-0x7FFF)
#define VRAM_SIZE   (0x2000)      // 8 KB         (0x8000-0x9FFF)
#define ERAM_SIZE   (0x2000)      // 8 KB         (0xA000-0xBFFF)
#define WRAM_SIZE   (0x2000)      // 8 KB         (0xC000-0xDFFF)
#define OAM_SIZE    (0xA0)        // 160 bytes    (0xFE00-0xFE9F)
#define IO_SIZE     (0x80)        // 128 bytes    (0xFF00-0xFF7F)
#define HRAM_SIZE   (0x7F)        // 127 bytes    (0xFF80-0xFFFE)

// Memory regions
#define ROM_START   0x0000
#define ROM_END     0x7FFF
#define VRAM_START  0x8000
#define VRAM_END    0x9FFF
#define ERAM_START  0xA000
#define ERAM_END    0xBFFF
#define WRAM_START  0xC000
#define WRAM_END    0xDFFF
#define ECHO_START  0xE000
#define ECHO_END    0xFDFF
#define OAM_START   0xFE00
#define OAM_END     0xFE9F
#define IO_START    0xFF00
#define IO_END      0xFF7F
#define HRAM_START  0xFF80
#define HRAM_END    0xFFFE
#define IE_REG      0xFFFF

// Special registers
#define BOOT_ROM_DISABLE_REG 0xFF50


namespace gb {
    class memory_map;
}

class gb::memory_map {
public:
    memory_map()
        : rom(std::array<uint8_t, ROM_SIZE>{}),
          vram(std::array<uint8_t, VRAM_SIZE>{}),
          wram(std::array<uint8_t, WRAM_SIZE>{}),
          oam(std::array<uint8_t, OAM_SIZE>{}),
          io(std::array<uint8_t, IO_SIZE>{}),
          hram(std::array<uint8_t, HRAM_SIZE>{}),
          ie_register(0),
          boot_rom_enabled(true)
    {
        rom.fill(0);
        vram.fill(0);
        wram.fill(0);
        oam.fill(0);
        io.fill(0xFF);  // IO registers typically initialize to 0xFF
        hram.fill(0);
    }

    [[nodiscard]] uint8_t read(uint16_t address) const
    {
        // Boot ROM handling (first 256 bytes)
        if (boot_rom_enabled && address <= DMG_BOOT_ROM_END)
        {
            return boot_rom[address];
        }

        if (address <= ROM_END)
        {
            return rom[address];
        }
        else if (address >= VRAM_START && address <= VRAM_END)
        {
            return vram[address - VRAM_START];
        }
        else if (address >= ERAM_START && address <= ERAM_END)
        {
            // External RAM handling would go here
            return 0xFF;
        }
        else if (address >= WRAM_START && address <= WRAM_END)
        {
            return wram[address - WRAM_START];
        }
        else if (address >= ECHO_START && address <= ECHO_END)
        {
            // Echo RAM mirrors WRAM
            return wram[address - ECHO_START];
        }
        else if (address >= OAM_START && address <= OAM_END)
        {
            return oam[address - OAM_START];
        }
        else if (address >= IO_START && address <= IO_END)
        {
            if (address == BOOT_ROM_DISABLE_REG)
            {
                return boot_rom_enabled ? 0x00 : 0x01;
            }
            return io[address - IO_START];
        }
        else if (address >= HRAM_START && address <= HRAM_END)
        {
            return hram[address - HRAM_START];
        }
        else if (address == IE_REG)
        {
            return ie_register;
        }
        return 0xFF;
    }

    void write(uint16_t address, uint8_t value)
    {
        if (address <= ROM_END)
        {
            // ROM is read-only, but might need MBC handling here
            return;
        }
        else if (address >= VRAM_START && address <= VRAM_END)
        {
            vram[address - VRAM_START] = value;
        }
        else if (address >= ERAM_START && address <= ERAM_END)
        {
            // External RAM handling would go here
        }
        else if (address >= WRAM_START && address <= WRAM_END)
        {
            wram[address - WRAM_START] = value;
        }
        else if (address >= ECHO_START && address <= ECHO_END)
        {
            // Echo RAM mirrors WRAM
            wram[address - ECHO_START] = value;
        }
        else if (address >= OAM_START && address <= OAM_END)
        {
            oam[address - OAM_START] = value;
        }
        else if (address >= IO_START && address <= IO_END)
        {
            // Special handling for some IO registers
            if (address == BOOT_ROM_DISABLE_REG && value == 0x01)
            {
                boot_rom_enabled = false;  // Disable boot ROM
            }
            io[address - IO_START] = value;
        }
        else if (address >= HRAM_START && address <= HRAM_END)
        {
            hram[address - HRAM_START] = value;
        }
        else if (address == IE_REG)
        {
            ie_register = value;
        }
    }

    void load_rom(const std::filesystem::path& rom_path)
    {
        std::ifstream rom_file(rom_path, std::ios::binary);
        if (!rom_file)
        {
            throw std::runtime_error("Error loading ROM!");
        }
        rom_file.read(reinterpret_cast<char*>(rom.data()), rom.size());
    }

    // for debugging/testing
    void skip_boot_rom()
    {
        boot_rom_enabled = false;
    }

private:
    std::array<uint8_t, ROM_SIZE> rom;
    std::array<uint8_t, VRAM_SIZE> vram;
    std::array<uint8_t, WRAM_SIZE> wram;
    std::array<uint8_t, OAM_SIZE> oam;
    std::array<uint8_t, IO_SIZE> io;
    std::array<uint8_t, HRAM_SIZE> hram;
    uint8_t ie_register;
    bool boot_rom_enabled;

    // Boot ROM (typically 256 bytes)
    static constexpr std::array<uint8_t, 0x100> boot_rom = dmg_boot;  // Would need to be filled with actual boot ROM data
};
