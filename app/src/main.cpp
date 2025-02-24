#include <iostream>

#include "core.h"

int main()
{
    core::ram mem;
    core::cpu cpu;
    cpu.execute(mem);
    return 0;
}
