#include "gb.h"

#include <iostream>
#include <filesystem>

uint32_t gb::cpu::execute(memory_map& mem)
{
    const uint8_t opcode = mem.read(PC++);
    uint32_t cycles = 1; // reading the opcode takes a cycle
    cycles += execute_opcode(opcode, mem);
    return cycles;
}

uint32_t gb::cpu::execute_opcode(uint8_t opcode, memory_map& mem)
{
    uint32_t cycles = 0;
    // consider adding instruction table instead of switch statement
    switch (opcode)
    {
        case NOP:
            break;
        case LD_BC_NN:
        {
            BC.low = mem.read(PC++);
            cycles++;
            BC.high = mem.read(PC++);
            cycles++;
        } break;

        default:
            std::cerr << "Unknown opcode: " << std::hex << (opcode) << std::endl;
    }

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
