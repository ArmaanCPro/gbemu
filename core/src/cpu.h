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
        init_instruction_table();
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

    /** function pointer returning uint32_t (# cycles) taking memory_map&.
     * @param memory_map ref to a memory_map
     * @returns # of machine cycles taken
     */
    typedef uint32_t (cpu::*instruction_fn)(memory_map&);
    instruction_fn instruction_table[256];

    // returns the # of machine cycles (1 mc = 4 clock cycles)
    uint32_t execute(memory_map& mem);

    void power_up_sequence(memory_map& wram);

    void init_instruction_table();


    uint32_t invalid_opcode(memory_map&);
    uint32_t nop(memory_map&) { return 1; }
    uint32_t ld_bc_nn(memory_map& mem);
    uint32_t ld_hl_nn(memory_map& mem);
    uint32_t ld_hld_a(memory_map& mem);
    uint32_t ld_a_n(memory_map& mem);
    uint32_t jp_nn(memory_map& mem);
    uint32_t jr_nz_n(memory_map& mem);

};
