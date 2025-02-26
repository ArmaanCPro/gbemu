#include <iostream>

#include "gb.h"

int main()
{
    gb::work_ram mem;
    gb::cpu cpu {mem};

    std::cout << "PC: " << std::hex << cpu.PC << std::endl;

    mem.data[cpu.PC] = gb::cpu::LD_BC_NN;
    mem.data[cpu.PC + 1] = 0x51;
    mem.data[cpu.PC + 2] = 0x21;

    cpu.execute(mem);
    cpu.execute(mem);

    std::cout << "BC: " << std::hex << cpu.BC.full << std::endl;
    return 0;
}
