#pragma once

#include <memory>
#include <cstdint>

#include "memory_map.h"

namespace gb
{
    struct cpu;

    enum flag_types : uint8_t
    {
        FLAG_Z = 0x80, // zero
        FLAG_N = 0x40, // subtract
        FLAG_H = 0x20, // half-carry
        FLAG_C = 0x10 // carry
    };

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
}

struct gb::cpu
{
    explicit cpu()
        :
        AF(),
        BC(),
        DE(),
        HL(),
        SP(),
        PC(),
        instruction_table{}
    {
        init_instruction_table();
        power_up_sequence();
    }

    // an enum allowing opcodes to choose a register as a parameter/template
    enum class r8
    {
        A, B, C, D, E, H, L
    };

    enum class r16
    {
        BC, DE, HL, SP, PC
    };

    uint8_t& get_r8(r8 reg)
    {
        switch (reg)
        {
            case r8::A:
                return AF.high;
            case r8::B:
                return BC.high;
            case r8::C:
                return BC.low;
            case r8::D:
                return DE.high;
            case r8::E:
                return DE.low;
            case r8::H:
                return HL.high;
            case r8::L:
                return HL.low;
            default:
                throw std::runtime_error("Invalid register r8");
        }
    }

    Register16& get_r16(r16 reg)
    {
        switch (reg)
        {
            case r16::BC:
                return BC;
            case r16::DE:
                return DE;
            case r16::HL:
                return HL;
            case r16::SP:
                return SP;
            case r16::PC:
                return PC;
            default:
                throw std::runtime_error("Invalid register r16");
        }
    }

    Register16 AF; // Accumulator and flags. bit 7 (0x80) = z, 6 (0x40) = n, 5 (0x20) = h, 4 (0x10) = c
    Register16 BC;
    Register16 DE;
    Register16 HL;

    // these are accessed usually only as a full 16-bit register
    Register16 SP;
    Register16 PC;

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

    [[nodiscard]] bool get_flag(flag_types flag) const
    {
        return AF.full & flag;
    }

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
    template <r8 reg>
    uint32_t adc_a_r8(memory_map&);
    uint32_t adc_a_hl_mem(memory_map& mem);
    uint32_t adc_a_n(memory_map& mem);
    template <r8 reg>
    uint32_t add_a_r8(memory_map&);
    uint32_t add_a_hl_mem(memory_map& mem);
    uint32_t add_a_n(memory_map& mem);
    template <r16 reg>
    uint32_t add_hl_r16(memory_map&);
    // e because it is signed
    uint32_t add_sp_e(memory_map& mem);
    template<r8 reg>
    uint32_t and_a_r8(memory_map&);
    uint32_t and_a_hl_mem(memory_map& mem);
    uint32_t and_a_n(memory_map& mem);
    template<r8 reg>
    uint32_t dec_r8(memory_map&);
    template<r16 reg>
    uint32_t dec_r16(memory_map&);
    template <r16 reg>
    uint32_t ld_r16_nn(memory_map& mem);
    template <r8 reg>
    uint32_t ld_r8_nn(memory_map& mem);
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
    template<r8 reg>
    uint32_t inc_r8(memory_map&);
    uint32_t inc_hl_mem(memory_map& mem);
    template<r16 reg>
    uint32_t inc_r16(memory_map&);
    template<r8 reg>
    uint32_t or_a_r8(memory_map&);
    template<r8 reg>
    uint32_t xor_a_r8(memory_map&);
};
