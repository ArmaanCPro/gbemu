#include "gb.h"

#include <iostream>
#include <filesystem>

uint32_t gb::cpu::execute(work_ram& mem)
{
    uint32_t cycles = 0;

    // Execute instructions from Game ROM
    uint8_t opcode = mem.data[PC++];
    cycles += execute_opcode(opcode, mem);

    return cycles;
}

uint32_t gb::cpu::execute_opcode(uint8_t opcode, work_ram &mem)
{
    uint32_t cycles = 0;
    // consider adding instruction table instead of switch statement
    switch (opcode)
    {
        case NOP:
            break;
        case LD_BC_NN:
        {
            BC.low = read_byte(mem, cycles);
            BC.high = read_byte(mem, cycles);
        } break;

        default:
            std::cerr << "Unknown opcode: " << std::hex << (opcode) << std::endl;
    }

    // bullshit for the compiler
    mem.data[PC++] = 0x00;
    return cycles;
}

void gb::cpu::load_rom(const std::string &rom_path, work_ram &wram)
{
    const auto path = std::filesystem::absolute(rom_path);
    std::ifstream rom_file(path, std::ios::binary);
    std::cout << "Loading ROM: " << rom_path << std::endl;

    if (!rom_file)
        throw std::runtime_error("Error loading ROM!");

    // Load ROM into memory (0x0000-0x7FFF)
    rom_file.read(reinterpret_cast<char*>(wram.data.data()), 0x8000);
}

void gb::cpu::power_up_sequence(work_ram &wram)
{
    // init cpu registers
    SP = 0xFFFE;
    PC = 0x0000; // Start with Boot ROM
    AF.full = 0x01B0;
    BC.full = 0x0013;
    DE.full = 0x00D8;
    HL.full = 0x014D;

    // init memory
    wram.data.fill(0);
    high_ram.fill(0);

    for (int i = 0; i < DMG_BOOT_ROM_SIZE; i++)
    {
        wram.data[i] = dmg_boot[i];
    }

    {
        uint32_t cycles = DMG_BOOT_ROM_SIZE;
        while (PC <= 0x50)
            cycles -= execute(wram);

        wram.data.fill(0); // remove boot sequence from memory
    }

    PC = 0x0100;
}
