#include <iostream>

#include "gb.h"

int main()
{
    gb::work_ram mem;
    gb::cpu cpu {mem};

    uint32_t cycles = mem.data.size();
    while (cycles != 0)
    {
        cycles -= cpu.execute(mem);
    }
    return 0;
}
