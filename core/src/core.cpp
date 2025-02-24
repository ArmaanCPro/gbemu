#include "core.h"

#include <iostream>

uint32_t core::cpu::execute(ram& mem)
{
    uint32_t cycles = 0;
    uint8_t opcode = read_byte(mem, cycles);
    switch (opcode)
    {
    case NOP:
        break;
    default:
        std::cerr << "Unknown opcode: " << std::hex << (opcode) << std::endl;
    }
    return cycles;
}
