#pragma once

#include <memory>
#include <cstdint>

#include "memory_map.h"
#include "../resources/dmg_opcodes.h"

#include <functional>

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

    // instruction table shenanigans
    using instruction_fn = std::function<void(memory_map& mem, uint32_t& cycles)>;
    static instruction_fn instructionTable[256];

    // returns the # of cycles
    uint32_t execute(memory_map& mem);

    uint32_t execute_opcode(uint8_t opcode, memory_map& mem);

    void power_up_sequence(memory_map& wram);

    void init_instruction_table();
};
