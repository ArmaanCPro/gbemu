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

    Register16 AF; // Accumulator and flags. bit 7 (0x80) = z, 6 (0x40) = n, 5 (0x20) = h, 4 (0x10) = c
    Register16 BC;
    Register16 DE;
    Register16 HL;

    // these are accessed usually only as a full 16-bit register
    Register16 SP;
    Register16 PC;

    // enum specifying an 8 bit register, used for template access to registers
    enum class r8 : uint8_t
    {
        A = 0, B = 1, C = 2, D = 3, E = 4, H = 5, L = 6
    };

    // enum specifying a 16 bit register, used for template access to registers
    enum class r16 : uint8_t
    {
        BC = 0, DE = 1, HL = 2, SP = 3, PC = 4
    };

    // two arrays used for compile time/template access to registers
    const std::array<Register16*, 5> register16s = {&BC, &DE, &HL, &SP, &PC};
    const std::array<uint8_t*, 7> register8s = {&AF.high, &BC.high, &BC.low, &DE.high, &DE.low, &HL.high, &HL.low};

    // get an 8 bit register from a template type
    template <r8 reg>
    [[nodiscard]] uint8_t& get_r8() const
    {
        return *register8s[static_cast<uint8_t>(reg)];
    }

    // get a 16 bit register from a template type
    template <r16 reg>
    [[nodiscard]] Register16& get_r16() const
    {
        return *register16s[static_cast<uint8_t>(reg)];
    }

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
    template <r8 reg>
    uint32_t and_a_r8(memory_map&);
    uint32_t and_a_hl_mem(memory_map& mem);
    uint32_t and_a_n(memory_map& mem);
    // TODO CB bit operations here

    uint32_t call_nn(memory_map& mem);
    uint32_t call_nz_nn(memory_map& mem);
    uint32_t call_z_nn(memory_map& mem);
    uint32_t call_nc_nn(memory_map& mem);
    uint32_t call_c_nn(memory_map& mem);
    uint32_t ccf(memory_map&);
    template <r8 reg>
    uint32_t cp_a_r8(memory_map&);
    uint32_t cp_a_hl_mem(memory_map& mem);
    uint32_t cp_a_n(memory_map& mem);
    uint32_t cpl(memory_map&);
    uint32_t daa(memory_map&);
    template <r8 reg>
    uint32_t dec_r8(memory_map&);
    template <r16 reg>
    uint32_t dec_r16(memory_map&);
    uint32_t dec_hl_mem(memory_map& mem);
    template <r16 reg>
    uint32_t ld_r16_nn(memory_map& mem);
    template <r8 reg>
    uint32_t ld_r8_nn(memory_map& mem);
    template <r8 reg_1, r8 reg_2>
    uint32_t ld_r8_r8(memory_map&);
    uint32_t ld_nn_a(memory_map& mem);
    uint32_t ld_hld_a(memory_map& mem);
    template <r8 reg>
    uint32_t ld_r8_n(memory_map& mem);
    template <r8 reg>
    uint32_t ld_hl_mem_r8(memory_map& mem);
    uint32_t ld_hl_mem_n(memory_map& mem);
    template <r8 reg>
    uint32_t ld_r8_hl_mem(memory_map& mem);
    template <r16 reg>
    uint32_t ld_r16_mem_a(memory_map& mem);
    // corresponds to ldh a8 a
    uint32_t ldh_nn_a(memory_map& mem);
    uint32_t ldh_c_a(memory_map& mem);
    template <r16 reg>
    uint32_t ld_a_r16_mem(memory_map& mem);
    uint32_t ld_a_nn(memory_map& mem);
    uint32_t ldh_a_c(memory_map& mem);
    // same as ldi_hl_a
    uint32_t ld_hli_mem_a(memory_map& mem);
    // same as ldd_hli_a
    uint32_t ld_hld_mem_a(memory_map& mem);
    // same as ldd_a_hl
    uint32_t ld_a_hld_mem(memory_map& mem);
    // same as ldi_a_hl
    uint32_t ld_a_hli_mem(memory_map& mem);
    uint32_t ld_nn_sp(memory_map& mem);
    // LD HL, SP+e8
    uint32_t ld_hl_sp_e8(memory_map& mem);
    uint32_t ld_sp_hl(memory_map&);
    uint32_t jp_nn(memory_map& mem);
    uint32_t jp_nz_nn(memory_map& mem);
    uint32_t jp_z_nn(memory_map& mem);
    uint32_t jp_nc_nn(memory_map& mem);
    uint32_t jp_c_nn(memory_map& mem);
    uint32_t jp_hl(memory_map&);
    // some websites may refer to this as jr_n16 or jr_r8. effectively, this is a signed 8-bit relative jump
    uint32_t jr_e(memory_map& mem);
    uint32_t jr_nz_n(memory_map& mem);
    uint32_t jr_z_n(memory_map& mem);
    uint32_t jr_nc_n(memory_map& mem);
    uint32_t jr_c_n(memory_map& mem);
    uint32_t ret(memory_map& mem);
    uint32_t push_bc(memory_map& mem);
    uint32_t pop_bc(memory_map& mem);
    template <r8 reg>
    uint32_t inc_r8(memory_map&);
    uint32_t inc_hl_mem(memory_map& mem);
    template <r16 reg>
    uint32_t inc_r16(memory_map&);
    template <r8 reg>
    uint32_t or_a_r8(memory_map&);
    uint32_t or_a_hl_mem(memory_map& mem);
    uint32_t or_a_n(memory_map& mem);
    template <r8 reg>
    uint32_t xor_a_r8(memory_map&);
};
