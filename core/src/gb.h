#pragma once

#include <memory>
#include <cstdint>
#include <array>
#include <fstream>

#include "../resources/dmg_boot.h"
#include "memory_map.h"

namespace gb
{
    struct cpu;
}


struct gb::cpu
{
    explicit cpu(memory_map& wram)
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

    Register16 AF; // Accumulator and flags. bit 7 = z, 6 = n, 5 = h, 4 = c
    Register16 BC;
    Register16 DE;
    Register16 HL;

    // these are accessed usually only as a full 16-bit register
    uint16_t SP;
    uint16_t PC;

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

    // returns the # of cycles
    uint32_t execute(memory_map& mem);

    uint32_t execute_opcode(uint8_t opcode, memory_map& mem);

    void power_up_sequence(memory_map& wram);
};
