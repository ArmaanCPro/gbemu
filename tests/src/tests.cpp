#include <gtest/gtest.h>

#include <cpu.h>
#include <memory_map.h>

class CpuTests1 : public ::testing::Test
{
public:
    gb::memory_map mem;
    gb::cpu cpu { mem };

    void SetUp() override
    {
        // put PC at a location in WRAM so that we can actually access it
        cpu.PC = 0xD000;
    }
};

TEST_F(CpuTests1, NopOperationWorks)
{
    // given:
    mem.write(cpu.PC, 0x00);

    // when:
    const uint32_t cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 1);
}

TEST_F(CpuTests1, LD_BC_NN_OperationWorks)
{
    // given:
    mem.write(cpu.PC, LD_BC_NN);
    mem.write(cpu.PC + 1, 0x21);
    mem.write(cpu.PC + 2, 0x58);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.BC.full, 0x5821);
}

TEST_F(CpuTests1, LD_HL_NN_OperationWorks)
{
    // given:
    mem.write(cpu.PC, LD_HL_NN);
    mem.write(cpu.PC + 1, 0x21);
    mem.write(cpu.PC + 2, 0x58);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.HL.full, 0x5821);
}

TEST_F(CpuTests1, LD_HLD_A_OperationWorks)
{
    // given:
    mem.write(cpu.PC, LD_HLD_A);
    cpu.AF.high = 0x58;
    cpu.HL.full = 0xCEEE;

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(mem.read(cpu.HL.full + 1), cpu.AF.high);
    EXPECT_EQ(cpu.HL.full, 0xCEEE - 1);
}

TEST_F(CpuTests1, LD_A_N_OperationWorks)
{
    // given:
    mem.write(cpu.PC, LD_A_N);
    mem.write(cpu.PC + 1, 0x58);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(cpu.AF.high, 0x58);
}

TEST_F(CpuTests1, JP_NN_OperationWorks)
{
    // given:
    mem.write(cpu.PC, JP_NN);
    mem.write(cpu.PC + 1, 0x58);    // Low byte of jump address
    mem.write(cpu.PC + 2, 0x21);    // High byte of jump address

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.PC, 0x2158);
}

TEST_F(CpuTests1, JR_NZ_N_OperationWorks)
{
    // Test 1: When Z flag is 0 (should jump forward)
    {
        // given:
        cpu.PC = 0xD000;
        cpu.AF.low = 0x00;          // Z flag is 0
        mem.write(cpu.PC, JR_NZ_N);
        mem.write(cpu.PC + 1, 0x04); // Jump forward 4 bytes

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 3);
        EXPECT_EQ(cpu.PC, 0xD006);  // 0xD000 + 2 + 4
    }

    // Test 2: When Z flag is 1 (should not jump)
    {
        // given:
        cpu.PC = 0xD000;
        cpu.AF.low = 0x40;          // Z flag is 1
        mem.write(cpu.PC, JR_NZ_N);
        mem.write(cpu.PC + 1, 0x04);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 2);
        EXPECT_EQ(cpu.PC, 0xD002);  // Only advance by instruction length
    }

    // Test 3: Test negative offset (jump backward)
    {
        // given:
        cpu.PC = 0xD000;
        cpu.AF.low = 0x00;          // Z flag is 0
        mem.write(cpu.PC, JR_NZ_N);
        mem.write(cpu.PC + 1, -4);  // Jump backward 4 bytes

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 3);
        EXPECT_EQ(cpu.PC, 0xCFFE);  // 0xD000 + 2 - 4
    }
}

TEST_F(CpuTests1, CALL_NN_OperationWorks)
{
    // Test 1: Basic call operation
    {
        // given:
        cpu.PC = 0xD000;
        cpu.SP = 0xCF00;
        mem.write(cpu.PC, CALL_NN);
        mem.write(cpu.PC + 1, 0x58);    // Low byte of target address
        mem.write(cpu.PC + 2, 0x21);    // High byte of target address

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 6);
        // Check if PC jumped to correct address
        EXPECT_EQ(cpu.PC, 0x2158);
        // Check if SP was decremented properly
        EXPECT_EQ(cpu.SP, 0xCEFE);
        // Check if return address was stored correctly on stack
        EXPECT_EQ(mem.read(cpu.SP), 0x03);      // Low byte of return address
        EXPECT_EQ(mem.read(cpu.SP + 1), 0xD0);  // High byte of return address
    }

    // Test 2: Call to a different address
    {
        // given:
        cpu.PC = 0xC800;
        cpu.SP = 0xDFF0;
        mem.write(cpu.PC, CALL_NN);
        mem.write(cpu.PC + 1, 0x34);    // Low byte of target address
        mem.write(cpu.PC + 2, 0x12);    // High byte of target address

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 6);
        // Check if PC jumped to correct address
        EXPECT_EQ(cpu.PC, 0x1234);
        // Check if SP was decremented properly
        EXPECT_EQ(cpu.SP, 0xDFEE);
        // Check if return address was stored correctly on stack
        EXPECT_EQ(mem.read(cpu.SP), 0x03);      // Low byte of return address
        EXPECT_EQ(mem.read(cpu.SP + 1), 0xC8);  // High byte of return address
    }
}

TEST_F(CpuTests1, RET_OperationWorks)
{
    // given:
    cpu.PC = 0xC800;
    cpu.SP = 0xDFF0;
    mem.write(cpu.PC, CALL_NN);
    mem.write(cpu.PC + 1, 0x10);     // Low byte of target address
    mem.write(cpu.PC + 2, 0xC0);     // High byte of target address
    cpu.execute(mem);                         // Execute CALL instruction
    const uint16_t oldSP = cpu.SP;
    mem.write(cpu.PC, RET);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(cpu.PC, 0xC800 + 3);
    EXPECT_EQ(cpu.SP, oldSP + 2);
}

TEST_F(CpuTests1, PUSH_BC_OperationWorks)
{
    // given:
    cpu.PC = 0xD000;
    cpu.SP = 0xCF00;
    cpu.BC.full = 0x1234;
    mem.write(cpu.PC, PUSH_BC);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(cpu.SP, 0xCEFE);
    EXPECT_EQ(mem.read(cpu.SP), 0x34);
    EXPECT_EQ(mem.read(cpu.SP + 1), 0x12);
}

TEST_F(CpuTests1, POP_BC_OperationWorks)
{
    // given:
    cpu.PC = 0xD000;
    cpu.SP = 0xCF00;
    cpu.BC.full = 0x1234;
    mem.write(cpu.PC, PUSH_BC);
    cpu.execute(mem);
    mem.write(cpu.PC, POP_BC);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.SP, 0xCF00);
    EXPECT_EQ(cpu.BC.full, 0x1234);
}
