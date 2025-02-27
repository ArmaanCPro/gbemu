#include "cpu.h"

#include <iostream>
#include <filesystem>
#include "../resources/dmg_boot.h"
#include <sstream>

uint32_t gb::cpu::execute(memory_map& mem)
{
    const uint8_t opcode = mem.read(PC++);
    uint32_t cycles = 0; // instruction functions handle all the cycle info, no work needs to be done here

    if (instruction_table[opcode] != nullptr)
        cycles += (this->*instruction_table[opcode])(mem);
    else
        std::cerr << "Unknown opcode: 0x" << std::hex << (opcode) << std::endl;
    return cycles;
}

void gb::cpu::power_up_sequence(memory_map& mem)
{
    // init cpu registers
    SP = 0xFFFE;
    PC = 0x0000;
    AF.full = 0x01B0;
    BC.full = 0x0013;
    DE.full = 0x00D8;
    HL.full = 0x014D;

    // write onto memory the boot rom. boot rom actually exists in ../resources/dmg_boot.h
    for (int i = 0; i < DMG_BOOT_ROM_SIZE; i++)
    {
        mem.write(i, dmg_boot[i]);
    }

    {
        uint32_t cycles = DMG_BOOT_ROM_SIZE;
        while (PC <= 0x50)
            cycles -= execute(mem);
    }

    PC = 0x0000;
}

void gb::cpu::init_instruction_table()
{
    std::fill_n(instruction_table, 256, &cpu::invalid_opcode);
    instruction_table[NOP] = &cpu::nop;

    instruction_table[LD_BC_NN] = &cpu::ld_bc_nn;
    instruction_table[LD_HL_NN] = &cpu::ld_hl_nn;
    instruction_table[LD_HLD_A] = &cpu::ld_hld_a;
    instruction_table[LD_A_N] = &cpu::ld_a_n;
    instruction_table[JP_NN] = &cpu::jp_nn;
    instruction_table[JR_NZ_N] = &cpu::jr_nz_n;
}

uint32_t gb::cpu::invalid_opcode(memory_map& mem)
{
    // [address, opcode]
    std::stringstream ss;
    ss << "Invalid opcode: ( 0x" << std::hex << (PC - 1) << ", 0x" << std::hex << mem.read(PC - 1) << " )";
    throw std::runtime_error(ss.str());
}

uint32_t gb::cpu::ld_bc_nn(memory_map& mem)
{
    BC.low = mem.read(PC++);
    BC.high = mem.read(PC++);
    return 3;
}

uint32_t gb::cpu::ld_hl_nn(memory_map& mem)
{
    HL.low = mem.read(PC++);
    HL.high = mem.read(PC++);
    return 3;
}

uint32_t gb::cpu::ld_hld_a(memory_map& mem)
{
    mem.write(HL.full, AF.high);
    HL.full--;
    return 2;
}

uint32_t gb::cpu::ld_a_n(memory_map& mem)
{
    AF.high = mem.read(PC++);
    return 2;
}

uint32_t gb::cpu::jp_nn(memory_map& mem)
{
    PC = mem.read(PC) | mem.read(PC + 1) << 8; // low | high << 8
    return 3;
}

uint32_t gb::cpu::jr_nz_n(memory_map& mem)
{
    const int8_t offset = static_cast<int8_t>(mem.read(PC++));
    if (!(AF.low & 0x40))  // Check Zero flag (bit 6)
    {
        PC += offset;
        return 3;
    }
    return 2;
}
