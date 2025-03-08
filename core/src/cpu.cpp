#include "cpu.h"

#include "../resources/dmg_opcodes.h"
#include <filesystem>
#include <iostream>

uint32_t gb::cpu::execute(memory_map& mem)
{
    const uint8_t opcode = mem.read(PC.full++);
    uint32_t cycles = 0; // instruction functions handle all the cycle info, no work needs to be done here

    if (opcode == CB_PREFIX)
    {
        // TODO CB opcode bullshits. maybe have another jump table for cb instructions
        //uint8_t cb_opcode = mem.read(PC.full++);
        // cb_instruction_table[cb_opcode]
    }
    else if (instruction_table[opcode] != nullptr)
        cycles += (this->*instruction_table[opcode])(mem);
    else
        std::cerr << "Unknown opcode: 0x" << std::hex << (opcode) << std::endl;

    return cycles;
}

void gb::cpu::power_up_sequence()
{
    // init cpu registers
    SP.full = 0xFFFE;
    PC.full = 0x0000;
    AF.full = 0x01B0;
    BC.full = 0x0013;
    DE.full = 0x00D8;
    HL.full = 0x014D;
}

void gb::cpu::init_instruction_table()
{
    std::fill_n(instruction_table, 256, &cpu::invalid_opcode);
    instruction_table[NOP] = &cpu::nop;

    instruction_table[ADC_A_A] = &cpu::adc_a_r8<r8::A>;
    instruction_table[ADC_A_B] = &cpu::adc_a_r8<r8::B>;
    instruction_table[ADC_A_C] = &cpu::adc_a_r8<r8::C>;
    instruction_table[ADC_A_D] = &cpu::adc_a_r8<r8::D>;
    instruction_table[ADC_A_E] = &cpu::adc_a_r8<r8::E>;
    instruction_table[ADC_A_H] = &cpu::adc_a_r8<r8::H>;
    instruction_table[ADC_A_L] = &cpu::adc_a_r8<r8::L>;
    instruction_table[LD_SP_NN] = &cpu::ld_r16_nn<r16::SP>;
    instruction_table[LD_BC_NN] = &cpu::ld_r16_nn<r16::BC>;
    instruction_table[LD_HL_NN] = &cpu::ld_r16_nn<r16::HL>;
    instruction_table[LD_NN_A] = &cpu::ld_nn_a;
    instruction_table[LD_HLD_A] = &cpu::ld_hld_a;
    instruction_table[LD_A_N] = &cpu::ld_a_n;
    instruction_table[LD_A_NN] = &cpu::ld_a_nn;
    instruction_table[JP_NN] = &cpu::jp_nn;
    instruction_table[JR_NZ_N] = &cpu::jr_nz_n;
    instruction_table[CALL_NN] = &cpu::call_nn;
    instruction_table[RET] = &cpu::ret;
    instruction_table[PUSH_BC] = &cpu::push_bc;
    instruction_table[POP_BC] = &cpu::pop_bc;
    instruction_table[INC_A] = &cpu::inc_a;
    instruction_table[INC_B] = &cpu::inc_b;
    instruction_table[INC_C] = &cpu::inc_c;
    instruction_table[INC_D] = &cpu::inc_d;
    instruction_table[INC_E] = &cpu::inc_e;
    instruction_table[INC_H] = &cpu::inc_h;
    instruction_table[INC_L] = &cpu::inc_l;
    instruction_table[INC_HL] = &cpu::inc_hl;
    instruction_table[INC_HL_MEM] = &cpu::inc_hl_mem;
    instruction_table[DEC_A] = &cpu::dec_r8<r8::A>;
    instruction_table[DEC_B] = &cpu::dec_r8<r8::B>;
    instruction_table[DEC_C] = &cpu::dec_r8<r8::C>;
    instruction_table[DEC_D] = &cpu::dec_r8<r8::D>;
    instruction_table[DEC_E] = &cpu::dec_r8<r8::E>;
    instruction_table[DEC_H] = &cpu::dec_r8<r8::H>;
    instruction_table[DEC_L] = &cpu::dec_r8<r8::L>;
    instruction_table[DEC_BC] = &cpu::dec_r16<r16::BC>;
    instruction_table[DEC_DE] = &cpu::dec_r16<r16::DE>;
    instruction_table[DEC_HL] = &cpu::dec_r16<r16::HL>;
    instruction_table[DEC_SP] = &cpu::dec_r16<r16::SP>;
    instruction_table[AND_A] = &cpu::and_a;
    instruction_table[OR_A] = &cpu::or_a;
    instruction_table[XOR_A] = &cpu::xor_a;
}

uint32_t gb::cpu::invalid_opcode(memory_map&)
{
    // [address, opcode]
    //std::cerr << "Invalid opcode: [ 0x" << std::hex << PC.full << ", 0x" << std::hex << mem.read(PC.full) << " ]" << std::endl;
    return 0;
}

uint32_t gb::cpu::nop(memory_map&)
{
    return 1;
}

template <gb::cpu::r8 reg>
uint32_t gb::cpu::adc_a_r8(memory_map&)
{
    const uint8_t orig_value = AF.high;
    const uint8_t to_add = get_r8(reg);
    const uint16_t result = orig_value + to_add + get_flag(FLAG_C);
    AF.high = (uint8_t)result;
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((orig_value & 0x0F) + (to_add & 0x0F) + get_flag(FLAG_C)) > 0x0F);
    set_flag(FLAG_C, result > 0xFF);
    return 1;
}

template <gb::cpu::r8 reg>
uint32_t gb::cpu::dec_r8(memory_map&)
{
    uint8_t& r = get_r8(reg);
    --r;
    set_flag(FLAG_Z, r == 0);
    set_flag(FLAG_N, true);
    set_flag(FLAG_H, (r & 0x0F) == 0x0);
    return 1;
}

template <gb::cpu::r16 reg>
uint32_t gb::cpu::dec_r16(memory_map&)
{
    --get_r16(reg).full;
    return 2;
}

template <gb::cpu::r16 reg>
uint32_t gb::cpu::ld_r16_nn(memory_map& mem)
{
    if constexpr (reg == r16::SP) // could also use template specialization
    {
        SP.full = mem.read(PC.full) | mem.read(PC.full + 1) << 8;
        PC.full += 2;
    }
    else
    {
        Register16& r = get_r16(reg);
        r.low = mem.read(PC.full++);
        r.high = mem.read(PC.full++);
    }
    return 3;
}

uint32_t gb::cpu::ld_nn_a(memory_map& mem)
{
    const uint16_t addr = mem.read(PC.full) | mem.read(PC.full + 1) << 8;
    PC.full += 2;
    mem.write(addr, AF.high);
    return 4;
}

uint32_t gb::cpu::ld_hld_a(memory_map& mem)
{
    mem.write(HL.full, AF.high);
    HL.full--;
    return 2;
}

uint32_t gb::cpu::ld_a_n(memory_map& mem)
{
    AF.high = mem.read(PC.full++);
    return 2;
}

uint32_t gb::cpu::ld_a_nn(memory_map& mem)
{
    const uint16_t addr = mem.read(PC.full) | mem.read(PC.full + 1) << 8;
    PC.full += 2;
    AF.high = mem.read(addr);
    return 4;
}

uint32_t gb::cpu::jp_nn(memory_map& mem)
{
    PC.full = mem.read(PC.full) | mem.read(PC.full + 1) << 8; // low | high << 8
    return 3;
}

uint32_t gb::cpu::jr_nz_n(memory_map& mem)
{
    const int8_t offset = static_cast<int8_t>(mem.read(PC.full++));
    if (!(AF.low & 0x40)) // Check Zero flag (bit 6)
    {
        PC.full += offset;
        return 3;
    }
    return 2;
}

uint32_t gb::cpu::call_nn(memory_map& mem)
{
    const uint16_t target_addr = mem.read(PC.full) | (mem.read(PC.full + 1) << 8);
    PC.full += 2;

    // Push current PC.full onto stack (SP.full decrements by 2)
    SP.full -= 2;
    mem.write(SP.full + 1, (PC.full >> 8) & 0xFF); // Write high byte
    mem.write(SP.full, PC.full & 0xFF); // Write low byte

    // Jump to target address
    PC.full = target_addr;

    return 6;
}

uint32_t gb::cpu::ret(memory_map& mem)
{
    // Pop PC.full from stack (SP.full increments by 2)
    PC.full = mem.read(SP.full) | (mem.read(SP.full + 1) << 8);
    SP.full += 2;
    return 4;
}

uint32_t gb::cpu::push_bc(memory_map& mem)
{
    SP.full--;
    mem.write(SP.full, BC.high);
    SP.full--;
    mem.write(SP.full, BC.low);
    return 4;
}

uint32_t gb::cpu::pop_bc(memory_map& mem)
{
    BC.low = mem.read(SP.full);
    SP.full++;
    BC.high = mem.read(SP.full);
    SP.full++;
    return 3;
}

uint32_t gb::cpu::inc_a(memory_map&)
{
    bool half_carry = (AF.high & 0x0F) == 0x0F;
    AF.high++;
    AF.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_H, half_carry);
    return 1;
}

uint32_t gb::cpu::inc_b(memory_map&)
{
    bool half_carry = (BC.high & 0x0F) == 0x0F;
    BC.high++;
    BC.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, BC.high == 0);
    set_flag(FLAG_H, half_carry);
    return 1;
}

uint32_t gb::cpu::inc_c(memory_map&)
{
    bool half_carry = (BC.low & 0x0F) == 0x0F;
    BC.low++;
    BC.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, BC.low == 0);
    set_flag(FLAG_H, half_carry);
    return 1;
}

uint32_t gb::cpu::inc_d(memory_map&)
{
    bool half_carry = (DE.high & 0x0F) == 0x0F;
    DE.high++;
    DE.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, DE.high == 0);
    set_flag(FLAG_H, half_carry);
    return 1;
}

uint32_t gb::cpu::inc_e(memory_map&)
{
    bool half_carry = (DE.low & 0x0F) == 0x0F;
    DE.low++;
    DE.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, DE.low == 0);
    set_flag(FLAG_H, half_carry);
    return 1;
}

uint32_t gb::cpu::inc_h(memory_map&)
{
    bool half_carry = (HL.high & 0x0F) == 0x0F;
    HL.high++;
    HL.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, HL.high == 0);
    set_flag(FLAG_H, half_carry);
    return 1;
}

uint32_t gb::cpu::inc_l(memory_map&)
{
    bool half_carry = (HL.low & 0x0F) == 0x0F;
    HL.low++;
    HL.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, HL.low == 0);
    set_flag(FLAG_H, half_carry);
    return 1;
}

uint32_t gb::cpu::inc_hl_mem(memory_map& mem)
{
    uint8_t value = mem.read(HL.full);
    bool half_carry = (value & 0x0F) == 0x0F;
    value++;
    mem.write(HL.full, value);
    set_flag(FLAG_Z, value == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, half_carry);
    return 4;
}

uint32_t gb::cpu::inc_hl(memory_map&)
{
    HL.full++;
    return 2;
}

uint32_t gb::cpu::and_a(memory_map&)
{
    AF.high &= AF.high;
    AF.low = 0x20; // Set half carry flag and no others
    if (AF.high == 0)
        AF.low |= 0x80; // Set zero flag

    return 1;
}

uint32_t gb::cpu::or_a(memory_map&)
{
    AF.high |= AF.high;
    AF.low = 0x0; // reset all flags
    if (AF.high == 0)
        AF.low |= 0x80; // set zero flag
    return 1;
}

uint32_t gb::cpu::xor_a(memory_map&)
{
    //AF.high ^= AF.high;
    AF.high = 0;
    AF.low = 0x0; // reset all flags
    //if (AF.high == 0)
    AF.low |= 0x80; // set zero flag
    return 1;
}
