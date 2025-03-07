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

    instruction_table[ADC_A_HL] = &cpu::adc_a_hl_mem;
    instruction_table[ADC_A_N] = &cpu::adc_a_n;

    instruction_table[ADD_A_A] = &cpu::add_a_r8<r8::A>;
    instruction_table[ADD_A_B] = &cpu::add_a_r8<r8::B>;
    instruction_table[ADD_A_C] = &cpu::add_a_r8<r8::C>;
    instruction_table[ADD_A_D] = &cpu::add_a_r8<r8::D>;
    instruction_table[ADD_A_E] = &cpu::add_a_r8<r8::E>;
    instruction_table[ADD_A_H] = &cpu::add_a_r8<r8::H>;
    instruction_table[ADD_A_L] = &cpu::add_a_r8<r8::L>;

    instruction_table[ADD_A_HL] = &cpu::add_a_hl_mem;
    instruction_table[ADD_A_N] = &cpu::add_a_n;

    instruction_table[ADD_HL_BC] = &cpu::add_hl_r16<r16::BC>;
    instruction_table[ADD_HL_DE] = &cpu::add_hl_r16<r16::DE>;
    instruction_table[ADD_HL_HL] = &cpu::add_hl_r16<r16::HL>;
    instruction_table[ADD_HL_SP] = &cpu::add_hl_r16<r16::SP>;
    instruction_table[ADD_SP_N] = &cpu::add_sp_e;

    instruction_table[AND_HL] = &cpu::and_a_hl_mem;
    instruction_table[AND_N] = &cpu::and_a_n;

    instruction_table[LD_SP_NN] = &cpu::ld_r16_nn<r16::SP>;
    instruction_table[LD_BC_NN] = &cpu::ld_r16_nn<r16::BC>;
    instruction_table[LD_HL_NN] = &cpu::ld_r16_nn<r16::HL>;
    instruction_table[LD_NN_A] = &cpu::ld_nn_a;
    instruction_table[LD_HLD_A] = &cpu::ld_hld_a;
    instruction_table[LD_A_N] = &cpu::ld_a_n;
    instruction_table[LD_A_NN] = &cpu::ld_a_nn;

    instruction_table[LD_A_N] = &cpu::ld_r8_nn<r8::A>;
    instruction_table[LD_B_N] = &cpu::ld_r8_nn<r8::B>;
    instruction_table[LD_C_N] = &cpu::ld_r8_nn<r8::C>;
    instruction_table[LD_D_N] = &cpu::ld_r8_nn<r8::D>;
    instruction_table[LD_E_N] = &cpu::ld_r8_nn<r8::E>;
    instruction_table[LD_H_N] = &cpu::ld_r8_nn<r8::H>;
    instruction_table[LD_L_N] = &cpu::ld_r8_nn<r8::L>;

    instruction_table[JP_NN] = &cpu::jp_nn;
    instruction_table[JR_NZ_N] = &cpu::jr_nz_n;
    instruction_table[CALL_NN] = &cpu::call_nn;
    instruction_table[RET] = &cpu::ret;
    instruction_table[PUSH_BC] = &cpu::push_bc;
    instruction_table[POP_BC] = &cpu::pop_bc;

    instruction_table[INC_A] = &cpu::inc_r8<r8::A>;
    instruction_table[INC_B] = &cpu::inc_r8<r8::B>;
    instruction_table[INC_C] = &cpu::inc_r8<r8::C>;
    instruction_table[INC_D] = &cpu::inc_r8<r8::D>;
    instruction_table[INC_E] = &cpu::inc_r8<r8::E>;
    instruction_table[INC_H] = &cpu::inc_r8<r8::H>;
    instruction_table[INC_L] = &cpu::inc_r8<r8::L>;
    instruction_table[INC_BC] = &cpu::inc_r16<r16::BC>;
    instruction_table[INC_DE] = &cpu::inc_r16<r16::DE>;
    instruction_table[INC_HL] = &cpu::inc_r16<r16::HL>;
    instruction_table[INC_SP] = &cpu::inc_r16<r16::SP>;
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

    instruction_table[AND_A] = &cpu::and_a_r8<r8::A>;
    instruction_table[AND_B] = &cpu::and_a_r8<r8::B>;
    instruction_table[AND_C] = &cpu::and_a_r8<r8::C>;
    instruction_table[AND_D] = &cpu::and_a_r8<r8::D>;
    instruction_table[AND_E] = &cpu::and_a_r8<r8::E>;
    instruction_table[AND_H] = &cpu::and_a_r8<r8::H>;
    instruction_table[AND_L] = &cpu::and_a_r8<r8::L>;

    instruction_table[OR_A] = &cpu::or_a_r8<r8::A>;
    instruction_table[OR_B] = &cpu::or_a_r8<r8::B>;
    instruction_table[OR_C] = &cpu::or_a_r8<r8::C>;
    instruction_table[OR_D] = &cpu::or_a_r8<r8::D>;
    instruction_table[OR_E] = &cpu::or_a_r8<r8::E>;
    instruction_table[OR_H] = &cpu::or_a_r8<r8::H>;
    instruction_table[OR_L] = &cpu::or_a_r8<r8::L>;

    instruction_table[XOR_A] = &cpu::xor_a_r8<r8::A>;
    instruction_table[XOR_B] = &cpu::xor_a_r8<r8::B>;
    instruction_table[XOR_C] = &cpu::xor_a_r8<r8::C>;
    instruction_table[XOR_D] = &cpu::xor_a_r8<r8::D>;
    instruction_table[XOR_E] = &cpu::xor_a_r8<r8::E>;
    instruction_table[XOR_H] = &cpu::xor_a_r8<r8::H>;
    instruction_table[XOR_L] = &cpu::xor_a_r8<r8::L>;
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

uint32_t gb::cpu::adc_a_hl_mem(memory_map& mem)
{
    const uint16_t result = AF.high + mem.read(HL.full) + get_flag(FLAG_C);
    AF.high = (uint8_t)result;
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((AF.high & 0x0F) + (mem.read(HL.full) & 0x0F) + get_flag(FLAG_C)) > 0x0F);
    set_flag(FLAG_C, result > 0xFF);
    return 2;
}

uint32_t gb::cpu::adc_a_n(memory_map& mem)
{
    const uint16_t result = AF.high + mem.read(PC.full++) + get_flag(FLAG_C);
    AF.high = (uint8_t)result;
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((AF.high & 0x0F) + (mem.read(PC.full++) & 0x0F) + get_flag(FLAG_C)) > 0x0F);
    set_flag(FLAG_C, result > 0xFF);
    return 2;
}

template <gb::cpu::r8 reg>
uint32_t gb::cpu::add_a_r8(memory_map&)
{
    const uint8_t orig_value = AF.high;
    const uint8_t to_add = get_r8(reg);
    const uint16_t result = orig_value + to_add;
    AF.high = (uint8_t)result;
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((orig_value & 0x0F) + (to_add & 0x0F)) > 0x0F);
    set_flag(FLAG_C, result > 0xFF);
    return 1;
}

uint32_t gb::cpu::add_a_hl_mem(memory_map& mem)
{
    const uint16_t result = AF.high + mem.read(HL.full);
    AF.high = (uint8_t)result;
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((AF.high & 0x0F) + (mem.read(HL.full) & 0x0F)) > 0x0F);
    set_flag(FLAG_C, result > 0xFF);
    return 2;
}

uint32_t gb::cpu::add_a_n(memory_map& mem)
{
    const uint16_t result = AF.high + mem.read(PC.full++);
    AF.high = (uint8_t)result;
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((AF.high & 0x0F) + (mem.read(PC.full++) & 0x0F)) > 0x0F);
    set_flag(FLAG_C, result > 0xFF);
    return 2;
}

template <gb::cpu::r16 reg>
uint32_t gb::cpu::add_hl_r16(memory_map&)
{
    Register16& r = get_r16(reg);
    const uint32_t result = HL.full + r.full;
    HL.full = (uint16_t)result;
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((HL.full & 0x0FFF) + (r.full & 0x0FFF)) > 0x0FFF);
    set_flag(FLAG_C, result > 0xFFFF);
    return 2;
}

uint32_t gb::cpu::add_sp_e(memory_map& mem)
{
    const int8_t e = (int8_t)mem.read(PC.full++);
    SP.full += e;
    set_flag(FLAG_Z, false);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, ((SP.full & 0x0FFF) + (e & 0x0FFF)) > 0x0FFF);
    set_flag(FLAG_C, (uint32_t(SP.full) + uint32_t(e)) > 0xFFFF);
    return 4;
}

template <gb::cpu::r8 reg>
uint32_t gb::cpu::and_a_r8(memory_map&)
{
    AF.high &= get_r8(reg);
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, true);
    set_flag(FLAG_C, false);
    return 1;
}

uint32_t gb::cpu::and_a_hl_mem(memory_map& mem)
{
    const uint8_t result = AF.high & mem.read(HL.full);
    AF.high = result;
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, true);
    set_flag(FLAG_C, false);
    return 2;
}

uint32_t gb::cpu::and_a_n(memory_map& mem)
{
    AF.high = AF.high & mem.read(PC.full++);
    AF.low = 0x0;
    set_flag(FLAG_Z, AF.high == 0);
    set_flag(FLAG_N, false);
    set_flag(FLAG_H, true);
    set_flag(FLAG_C, false);
    return 2;
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

template <gb::cpu::r8 reg>
uint32_t gb::cpu::ld_r8_nn(memory_map& mem)
{
    get_r8(reg) = mem.read(PC.full++);
    return 2;
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

template <gb::cpu::r8 reg>
uint32_t gb::cpu::inc_r8(memory_map&)
{
    uint8_t& r = get_r8(reg);
    bool half_carry = (r & 0x0F) == 0x0F;
    ++r;
    AF.low &= FLAG_C; // preserve carry flag and clear
    set_flag(FLAG_Z, r == 0);
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

template <gb::cpu::r16 reg>
uint32_t gb::cpu::inc_r16(memory_map&)
{
    ++get_r16(reg).full;
    return 2;
}

template <gb::cpu::r8 reg>
uint32_t gb::cpu::or_a_r8(memory_map&)
{
    AF.high |= get_r8(reg);
    AF.low = 0x0; // reset all flags
    set_flag(FLAG_Z, AF.high == 0);
    return 1;
}

template <gb::cpu::r8 reg>
uint32_t gb::cpu::xor_a_r8(memory_map&)
{
    AF.high ^= get_r8(reg);
    AF.low = 0x0; // reset all flags
    set_flag(FLAG_Z, AF.high == 0);
    return 1;
}
