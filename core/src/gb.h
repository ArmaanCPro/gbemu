#pragma once

#include <memory>
#include <cstdint>
#include <array>
#include <fstream>

#include "../resources/dmg_boot.h"

#define WRAM_SIZE (8 * 1024)    // 8 KB
#define HRAM_SIZE (8 * 127)     // 80 bytes

namespace gb
{
    struct work_ram;
    struct cpu;
}

struct gb::work_ram
{
    std::array<uint8_t, WRAM_SIZE> data;
};


struct gb::cpu
{
    explicit cpu(work_ram& wram)
    {
        power_up_sequence(wram);
    }
    // allows viewing a register as both a 16 bit and 2 8 bit values
    union Register16
    {
        uint16_t full;
        struct
        {
            uint8_t low;
            uint8_t high;
        };
    };

    // -- registers --
    // these are commonly accessed as 1 16-bit or 2 8-bit registers
    Register16 AF; // Accumulator and flags. bit 7 = z, 6 = n, 5 = h, 4 = c
    Register16 BC;
    Register16 DE;
    Register16 HL;

    // these are accessed usually only as a full 16-bit register
    uint16_t SP;    // Stack Pointer
    uint16_t PC;    // Program Counter/Pointer
    // -------------

    // high ram
    std::array<uint8_t, HRAM_SIZE> high_ram;

    static constexpr uint8_t
        // NOP & Jump Instructions
        NOP         = 0x00,
        JP_NN       = 0xC3,
        JP_NZ_NN    = 0xC2,
        JP_Z_NN     = 0xCA,
        JR_N        = 0x18,
        // Load Instructions
        LD_BC_NN    = 0x01,
        LD_B_N      = 0x06,
        LD_D_N      = 0x16,
        LD_H_N      = 0x26,
        LD_HL_NN    = 0x21,
        LD_A_B      = 0x21,
        LD_A_C      = 0x79,
        LD_B_D      = 0x42,
        LD_A_HLP    = 0x2A,     // HL+
        LD_A_HLM    = 0x3A,     // HL-
        LD_NN_A     = 0xEA,
        // Arithmetic Instructions
        ADD_A_C     = 0x81,
        ADD_A_D8    = 0xC6,
        SUB_B       = 0x90,
        SUB_D8      = 0xD6,
        INC_B       = 0x04,
        INC_C       = 0x0C,
        DEC_DE      = 0x05,
        // Bitwise Instructions
        AND_B       = 0xA0,
        AND_D8      = 0xE6,
        XOR_C       = 0xA9,
        XOR_D8      = 0xEE,
        OR_B        = 0xB0,
        OR_D8       = 0xF6,
        // Stack Instructions
        PUSH_BC     = 0xC5,
        PUSH_DE     = 0xD5,
        PUSH_HL     = 0xE5,
        POP_BC      = 0xC1,
        POP_DE      = 0xD1,
        POP_HL      = 0xE1,
        CALL_NN     = 0xCD,
        CALL_NZ_NN  = 0xC4,
        CALL_Z_NN   = 0xCC,
        // Misc Instructions
        HALT        = 0x76,
        STOP        = 0x10, // second byte should generally be NOP 0x00
        CCF         = 0x3F,
        SCF         = 0x37,
        DI          = 0xF3,
        EI          = 0xFB
    ;

    [[nodiscard]] inline uint8_t read_hram(uint16_t address)
    {
        if (address >= 0xFF80 && address <= 0xFFFE)
        {
            return high_ram[address - 0xFF80];
        }
        throw std::out_of_range("HRAM access out of range");
    }

    inline void write_hram(uint16_t address, uint8_t value)
    {
        if (address >= 0xFF80 && address <= 0xFFFE)
        {
            high_ram[address - 0xFF80] = value;
            return;
        }
        throw std::out_of_range("HRAM access out of range");
    }

    [[nodiscard]] inline uint8_t read_byte(work_ram& mem, uint32_t& cycles)
    {
        cycles++;
        if (PC >= 0xFF80 && PC <= 0xFFFE)
        {
            // Handle HRAM access
            return read_hram(PC++);
        }
        else if (PC < mem.data.size())
        {
            // Handle WRAM access
            return mem.data[PC++];
        }
        throw std::out_of_range("Invalid memory read");
    }

    inline void write_byte(work_ram& mem, uint16_t address, uint8_t value, uint32_t& cycles)
    {
        cycles++;
        if (address >= 0xFF80 && address <= 0xFFFE)
        {
            // Handle HRAM access
            write_hram(address, value);
        }
        else if (address < mem.data.size())
        {
            // Handle WRAM access
            mem.data[address] = value;
        }
        else
        {
            throw std::out_of_range("Invalid memory write");
        }
    }


    // returns the # of cycles
    uint32_t execute(work_ram& mem);

    uint32_t execute_opcode(uint8_t opcode, work_ram& mem);

    void load_rom(const std::string& rom_path, work_ram& mem);


    void power_up_sequence(work_ram& wram);
};
