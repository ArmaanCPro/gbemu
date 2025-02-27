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
