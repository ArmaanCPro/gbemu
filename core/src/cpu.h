#pragma once

#include <memory>
#include <cstdint>

#include "memory_map.h"

namespace gb
{
    struct cpu;

    enum flag_types : uint8_t
    {
        FLAG_Z = 0x80,
        FLAG_N = 0x40,
        FLAG_H = 0x20,
        FLAG_C = 0x10
    };
}


struct gb::cpu
{
    explicit cpu()
        :
        AF(),
        BC(),
        DE(),
        HL(),
        SP(0),
        PC(0),
        instruction_table{}
    {
        init_instruction_table();
        power_up_sequence();
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

    Register16 AF; // Accumulator and flags. bit 7 (0x80) = z, 6 (0x40) = n, 5 (0x20) = h, 4 (0x10) = c
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

    void power_up_sequence();

    void init_instruction_table();

    bool set_flag(flag_types flag, bool value)
    {
        if (value)
        {
            AF.full |= flag;
        }
        else
        {
            AF.full &= ~flag;
        }
        return value;
    }


    uint32_t invalid_opcode(memory_map&);
    uint32_t nop(memory_map&);
    uint32_t dec_sp(memory_map&);
    uint32_t ld_sp_nn(memory_map& mem);
    uint32_t ld_bc_nn(memory_map& mem);
    uint32_t ld_hl_nn(memory_map& mem);
    uint32_t ld_nn_a(memory_map& mem);
    uint32_t ld_hld_a(memory_map& mem);
    uint32_t ld_a_n(memory_map& mem);
    uint32_t ld_a_nn(memory_map& mem);
    uint32_t jp_nn(memory_map& mem);
    uint32_t jr_nz_n(memory_map& mem);
    uint32_t call_nn(memory_map& mem);
    uint32_t ret(memory_map& mem);
    uint32_t push_bc(memory_map& mem);
    uint32_t pop_bc(memory_map& mem);
    uint32_t inc_a(memory_map&);
    uint32_t inc_hl_mem(memory_map& mem);
    uint32_t inc_hl(memory_map&);
    uint32_t dec_a(memory_map&);
    uint32_t and_a(memory_map&);
    uint32_t or_a(memory_map&);
    uint32_t xor_a(memory_map&);
};
