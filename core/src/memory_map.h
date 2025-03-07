#pragma once
#include "../resources/dmg_boot.h"

#include <cstdint>
#include <array>
#include <vector>
#include <filesystem>
#include <fstream>

// Memory sizes
#define ROM_BANK_SIZE (0x4000)    // 16 KB per bank
#define RAM_BANK_SIZE (0x2000)    // 8 KB per bank
#define VRAM_SIZE     (0x2000)    // 8 KB         (0x8000-0x9FFF)
#define WRAM_SIZE     (0x2000)    // 8 KB         (0xC000-0xDFFF)
#define OAM_SIZE      (0xA0)      // 160 bytes    (0xFE00-0xFE9F)
#define IO_SIZE       (0x80)      // 128 bytes    (0xFF00-0xFF7F)
#define HRAM_SIZE     (0x7F)      // 127 bytes    (0xFF80-0xFFFE)

// Memory regions
#define ROM_BANK0_START 0x0000    // Fixed bank
#define ROM_BANK0_END   0x3FFF
#define ROM_BANKN_START 0x4000    // Switchable bank
#define ROM_BANKN_END   0x7FFF
#define VRAM_START      0x8000
#define VRAM_END        0x9FFF
#define ERAM_START      0xA000    // External RAM (cartridge)
#define ERAM_END        0xBFFF
#define WRAM_START      0xC000
#define WRAM_END        0xDFFF
#define ECHO_START      0xE000
#define ECHO_END        0xFDFF
#define OAM_START       0xFE00
#define OAM_END         0xFE9F
#define IO_START        0xFF00
#define IO_END          0xFF7F
#define HRAM_START      0xFF80
#define HRAM_END        0xFFFE
#define IE_REG          0xFFFF

// special registers
#define BOOT_ROM_DISABLE_REGISTER 0xFF50

namespace gb
{
    class memory_map;
}

class gb::memory_map
{
public:
    memory_map()
        : rom_bank0(std::array<uint8_t, ROM_BANK_SIZE>{}),
          vram(std::array<uint8_t, VRAM_SIZE>{}),
          wram(std::array<uint8_t, WRAM_SIZE>{}),
          oam(std::array<uint8_t, OAM_SIZE>{}),
          io(std::array<uint8_t, IO_SIZE>{}),
          hram(std::array<uint8_t, HRAM_SIZE>{}),
          ie_register(0),
          current_rom_bank(1), // Bank 0 is fixed, so we start with bank 1
          current_ram_bank(0),
          ram_enabled(false),
          boot_rom_enabled(true)
    {
        rom_bank0.fill(0);
        vram.fill(0);
        wram.fill(0);
        oam.fill(0);
        io.fill(0xFF);
        hram.fill(0);
    }

    [[nodiscard]] uint8_t read(uint16_t address) const
    {
        // Boot ROM handling (first 256 bytes)
        if (boot_rom_enabled && address < 0x100)
        {
            return boot_rom[address];
        }

        if (address <= ROM_BANK0_END)
        {
            return rom_bank0[address];
        }
        else if (address <= ROM_BANKN_END)
        {
            return rom_banks[current_rom_bank][address - ROM_BANKN_START];
        }
        else if (address >= VRAM_START && address <= VRAM_END)
        {
            return vram[address - VRAM_START];
        }
        else if (address >= ERAM_START && address <= ERAM_END)
        {
            if (ram_enabled && !ram_banks.empty())
            {
                return ram_banks[current_ram_bank][address - ERAM_START];
            }
            return 0xFF;
        }
        else if (address >= WRAM_START && address <= WRAM_END)
        {
            return wram[address - WRAM_START];
        }
        else if (address >= ECHO_START && address <= ECHO_END)
        {
            return wram[address - ECHO_START];
        }
        else if (address >= OAM_START && address <= OAM_END)
        {
            return oam[address - OAM_START];
        }
        else if (address >= IO_START && address <= IO_END)
        {
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
        if (address <= ROM_BANKN_END)
        {
            handle_banking(address, value);
        }
        else if (address >= VRAM_START && address <= VRAM_END)
        {
            vram[address - VRAM_START] = value;
        }
        else if (address >= ERAM_START && address <= ERAM_END)
        {
            if (ram_enabled && !ram_banks.empty())
            {
                ram_banks[current_ram_bank][address - ERAM_START] = value;
            }
        }
        else if (address >= WRAM_START && address <= WRAM_END)
        {
            wram[address - WRAM_START] = value;
        }
        else if (address >= ECHO_START && address <= ECHO_END)
        {
            wram[address - ECHO_START] = value;
        }
        else if (address >= OAM_START && address <= OAM_END)
        {
            oam[address - OAM_START] = value;
        }
        else if (address >= IO_START && address <= IO_END)
        {
            if (address == 0xFF50) // Boot ROM disable register
            {
                if (value == 0x01)
                {
                    boot_rom_enabled = false;
                }
                return;
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

        // Get file size
        rom_file.seekg(0, std::ios::end);
        const size_t rom_size = rom_file.tellg();
        rom_file.seekg(0);

        // Read first bank (16KB)
        rom_file.read(reinterpret_cast<char*>(rom_bank0.data()), ROM_BANK_SIZE);

        // Calculate number of additional banks needed
        const size_t remaining_size = rom_size - ROM_BANK_SIZE;
        const size_t num_banks = (remaining_size + ROM_BANK_SIZE - 1) / ROM_BANK_SIZE;
        rom_banks.resize(num_banks, std::array<uint8_t, ROM_BANK_SIZE>{});

        // Read remaining banks
        for (size_t i = 0; i < num_banks && rom_file; ++i)
        {
            rom_file.read(reinterpret_cast<char*>(rom_banks[i].data()), ROM_BANK_SIZE);
        }

        // Setup RAM banks based on cartridge type
        setup_ram_banks();
    }

    // optionally for debugging/testing
    void skip_boot_rom()
    {
        boot_rom_enabled = false;
    }

private:
    // ROM banks
    std::array<uint8_t, ROM_BANK_SIZE> rom_bank0; // Fixed bank 0
    std::vector<std::array<uint8_t, ROM_BANK_SIZE>> rom_banks; // Switchable banks

    // RAM regions
    std::vector<std::array<uint8_t, RAM_BANK_SIZE>> ram_banks; // External RAM banks
    std::array<uint8_t, VRAM_SIZE> vram;
    std::array<uint8_t, WRAM_SIZE> wram;
    std::array<uint8_t, OAM_SIZE> oam;
    std::array<uint8_t, IO_SIZE> io;
    std::array<uint8_t, HRAM_SIZE> hram;
    uint8_t ie_register;

    // Banking control
    uint8_t current_rom_bank;
    uint8_t current_ram_bank;
    bool ram_enabled;
    bool boot_rom_enabled;

    // Boot ROM (typically 256 bytes)
    static constexpr std::array<uint8_t, 0x100> boot_rom = dmg_boot;

    void handle_banking(uint16_t address, uint8_t value)
    {
        // Basic MBC1 implementation
        if (address <= 0x1FFF)
        {
            // RAM Enable
            ram_enabled = ((value & 0x0F) == 0x0A);
        }
        else if (address <= 0x3FFF)
        {
            // ROM Bank Number (low 5 bits)
            value &= 0x1F;
            if (value == 0)
                value = 1; // Bank 0 is not allowed here
            current_rom_bank = (current_rom_bank & 0x60) | value;
        }
        else if (address <= 0x5FFF)
        {
            // RAM Bank Number or Upper Bits of ROM Bank Number
            current_ram_bank = value & 0x03;
        }
    }

    void setup_ram_banks()
    {
        // Read cartridge type and RAM size from ROM header
        uint8_t ram_size = rom_bank0[0x149];

        size_t num_ram_banks = 0;
        switch (ram_size)
        {
            case 0x02:
                num_ram_banks = 1;
                break; // 8KB
            case 0x03:
                num_ram_banks = 4;
                break; // 32KB
            case 0x04:
                num_ram_banks = 16;
                break; // 128KB
            case 0x05:
                num_ram_banks = 8;
                break; // 64KB
        }

        if (num_ram_banks > 0)
        {
            ram_banks.resize(num_ram_banks, std::array<uint8_t, RAM_BANK_SIZE>{});
            for (auto& bank : ram_banks)
            {
                bank.fill(0);
            }
        }
    }
};
