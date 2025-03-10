#include <cpu.h>
#include <cstdint>
#include <dmg_opcodes.h>
#include <memory_map.h>
#include <gtest/gtest.h>

class CpuTests1 : public ::testing::Test
{
public:
    gb::memory_map mem {};
    gb::cpu cpu {};

    void SetUp() override
    {
        // put PC at a location in WRAM so that we can actually access it
        cpu.PC.full = 0xD000;
        cpu.AF.low = 0x0;
    }
};

TEST_F(CpuTests1, NopOperationWorks)
{
    // given:
    mem.write(cpu.PC.full, 0x00);

    // when:
    const uint32_t cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 1);
}

TEST_F(CpuTests1, LD_SP_NN_OperationWorks)
{
    // given:
    mem.write(cpu.PC.full, LD_SP_NN);
    mem.write(cpu.PC.full + 1, 0x21);
    mem.write(cpu.PC.full + 2, 0x58);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.SP.full, 0x5821);
}

TEST_F(CpuTests1, LD_BC_NN_OperationWorks)
{
    // given:
    mem.write(cpu.PC.full, LD_BC_NN);
    mem.write(cpu.PC.full + 1, 0x21);
    mem.write(cpu.PC.full + 2, 0x58);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.BC.full, 0x5821);
}

TEST_F(CpuTests1, LD_HL_NN_OperationWorks)
{
    // given:
    mem.write(cpu.PC.full, LD_HL_NN);
    mem.write(cpu.PC.full + 1, 0x21);
    mem.write(cpu.PC.full + 2, 0x58);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.HL.full, 0x5821);
}

TEST_F(CpuTests1, LD_HLD_A_OperationWorks)
{
    // given:
    mem.write(cpu.PC.full, LD_HLD_A);
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
    mem.write(cpu.PC.full, LD_A_N);
    mem.write(cpu.PC.full + 1, 0x58);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 2);
    EXPECT_EQ(cpu.AF.high, 0x58);
}

TEST_F(CpuTests1, JP_NN_OperationWorks)
{
    // given:
    mem.write(cpu.PC.full, JP_NN);
    mem.write(cpu.PC.full + 1, 0x58); // Low byte of jump address
    mem.write(cpu.PC.full + 2, 0x21); // High byte of jump address

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.PC.full, 0x2158);
}

TEST_F(CpuTests1, JR_NZ_N_OperationWorks)
{
    // Test 1: When Z flag is 0 (should jump forward)
    {
        // given:
        cpu.PC.full = 0xD000;
        cpu.AF.low = 0x00; // Z flag is 0
        mem.write(cpu.PC.full, JR_NZ_N);
        mem.write(cpu.PC.full + 1, 0x04); // Jump forward 4 bytes

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 3);
        EXPECT_EQ(cpu.PC.full, 0xD006); // 0xD000 + 2 + 4
    }

    // Test 2: When Z flag is 1 (should not jump)
    {
        // given:
        cpu.PC.full = 0xD000;
        cpu.AF.low = 0x40; // Z flag is 1
        mem.write(cpu.PC.full, JR_NZ_N);
        mem.write(cpu.PC.full + 1, 0x04);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 2);
        EXPECT_EQ(cpu.PC.full, 0xD002); // Only advance by instruction length
    }

    // Test 3: Test negative offset (jump backward)
    {
        // given:
        cpu.PC.full = 0xD000;
        cpu.AF.low = 0x00; // Z flag is 0
        mem.write(cpu.PC.full, JR_NZ_N);
        mem.write(cpu.PC.full + 1, (uint8_t)-4); // Jump backward 4 bytes

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 3);
        EXPECT_EQ(cpu.PC.full, 0xCFFE); // 0xD000 + 2 - 4
    }
}

TEST_F(CpuTests1, CALL_NN_OperationWorks)
{
    // Test 1: Basic call operation
    {
        // given:
        cpu.PC.full = 0xD000;
        cpu.SP.full = 0xCF00;
        mem.write(cpu.PC.full, CALL_NN);
        mem.write(cpu.PC.full + 1, 0x58); // Low byte of target address
        mem.write(cpu.PC.full + 2, 0x21); // High byte of target address

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 6);
        // Check if PC jumped to correct address
        EXPECT_EQ(cpu.PC.full, 0x2158);
        // Check if SP was decremented properly
        EXPECT_EQ(cpu.SP.full, 0xCEFE);
        // Check if return address was stored correctly on stack
        EXPECT_EQ(mem.read(cpu.SP.full), 0x03); // Low byte of return address
        EXPECT_EQ(mem.read(cpu.SP.full + 1), 0xD0); // High byte of return address
    }

    // Test 2: Call to a different address
    {
        // given:
        cpu.PC.full = 0xC800;
        cpu.SP.full = 0xDFF0;
        mem.write(cpu.PC.full, CALL_NN);
        mem.write(cpu.PC.full + 1, 0x34); // Low byte of target address
        mem.write(cpu.PC.full + 2, 0x12); // High byte of target address

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 6);
        // Check if PC jumped to correct address
        EXPECT_EQ(cpu.PC.full, 0x1234);
        // Check if SP was decremented properly
        EXPECT_EQ(cpu.SP.full, 0xDFEE);
        // Check if return address was stored correctly on stack
        EXPECT_EQ(mem.read(cpu.SP.full), 0x03); // Low byte of return address
        EXPECT_EQ(mem.read(cpu.SP.full + 1), 0xC8); // High byte of return address
    }
}

TEST_F(CpuTests1, RET_OperationWorks)
{
    // given:
    cpu.PC.full = 0xC800;
    cpu.SP.full = 0xDFF0;
    mem.write(cpu.PC.full, CALL_NN);
    mem.write(cpu.PC.full + 1, 0x10); // Low byte of target address
    mem.write(cpu.PC.full + 2, 0xC0); // High byte of target address
    cpu.execute(mem); // Execute CALL instruction
    const uint16_t oldSP = cpu.SP.full;
    mem.write(cpu.PC.full, RET);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(cpu.PC.full, 0xC800 + 3);
    EXPECT_EQ(cpu.SP.full, oldSP + 2);
}

TEST_F(CpuTests1, DEC_R16_AND_SP_OperationWorks)
{
    // test 1 - BC
    {
        cpu.BC.full = 0x0520;
        mem.write(cpu.PC.full, DEC_BC);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 2);
        EXPECT_EQ(cpu.BC.full, 0x0520 - 1);
    }

    // test 2 - SP
    {
        // given:
        cpu.SP.full = 0xCF00;
        mem.write(cpu.PC.full, DEC_SP);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 2);
        EXPECT_EQ(cpu.SP.full, 0xCF00 - 1);
    }
}

TEST_F(CpuTests1, PUSH_BC_OperationWorks)
{
    // given:
    cpu.PC.full = 0xD000;
    cpu.SP.full = 0xCF00;
    cpu.BC.full = 0x1234;
    mem.write(cpu.PC.full, PUSH_BC);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 4);
    EXPECT_EQ(cpu.SP.full, 0xCEFE);
    EXPECT_EQ(mem.read(cpu.SP.full), 0x34);
    EXPECT_EQ(mem.read(cpu.SP.full + 1), 0x12);
}

TEST_F(CpuTests1, POP_BC_OperationWorks)
{
    // given:
    cpu.PC.full = 0xD000;
    cpu.SP.full = 0xCF00;
    cpu.BC.full = 0x1234;
    mem.write(cpu.PC.full, PUSH_BC);
    cpu.execute(mem);
    mem.write(cpu.PC.full, POP_BC);

    // when:
    const auto cycles = cpu.execute(mem);

    // then:
    EXPECT_EQ(cycles, 3);
    EXPECT_EQ(cpu.SP.full, 0xCF00);
    EXPECT_EQ(cpu.BC.full, 0x1234);
}

TEST_F(CpuTests1, A_Register_Arithmetic_OperationsWork)
{
    // test 1: inc
    {
        // given:
        cpu.AF.high = 0x05;
        mem.write(cpu.PC.full, INC_A);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.high, 0x06);
        EXPECT_EQ(cpu.AF.low, 0x0);
    }
    // test 2: dec
    {
        // given:
        cpu.AF.high = 0x01;
        mem.write(cpu.PC.full, DEC_A);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.high, 0x0);
        EXPECT_EQ(cpu.AF.low, gb::FLAG_Z | gb::FLAG_H | gb::FLAG_N);
    }
    // test 3: and w/ zero flag
    {
        // given:
        cpu.AF.high = 0x00;
        mem.write(cpu.PC.full, AND_A);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.low, 0x80 | 0x20);
    }
    // test 4: or w/o zero flag
    {
        // given:
        cpu.AF.high = 0x11;
        mem.write(cpu.PC.full, OR_A);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.high, 0x11);
        EXPECT_EQ(cpu.AF.low, 0x0);
    }
}

TEST_F(CpuTests1, ADC_A_R8_Template_OperationWorks)
{
    // test 1 - ADC_A_A
    {
        // given:
        cpu.AF.high = 0x01;
        cpu.AF.low = 0x00;
        mem.write(cpu.PC.full, ADC_A_A);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.high, 0x02);
        EXPECT_EQ(cpu.AF.low, 0x00);
    }

    // test 2 - ADC_A_B
    {
        // given:
        cpu.AF.high = 0x01;
        cpu.AF.low = 0x00;
        cpu.BC.high = 0x02;
        mem.write(cpu.PC.full, ADC_A_B);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.high, 0x03);
        EXPECT_EQ(cpu.AF.low, 0x00);
    }
}

TEST_F(CpuTests1, CCF_OperationWorks)
{
    // test 1 - true to false
    {
        // given:
        cpu.AF.low = 0x00;
        cpu.set_flag(gb::FLAG_C, true);
        mem.write(cpu.PC.full, CCF);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.low, 0x00);
        EXPECT_EQ(cpu.get_flag(gb::FLAG_C), false);
    }
    // test 2 - false to true
    {
        // given:
        cpu.AF.low = 0x00;
        cpu.set_flag(gb::FLAG_C, false);
        mem.write(cpu.PC.full, CCF);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.low, 0x00 | gb::FLAG_C);
        EXPECT_EQ(cpu.get_flag(gb::FLAG_C), true);
    }
}

TEST_F(CpuTests1, CP_A_R8_TemplateOperationWorks)
{
    // test 1 - (A - A)
    {
        // given:
        cpu.AF.high = 0x02;
        cpu.AF.low = 0x00;
        mem.write(cpu.PC.full, CP_A);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.low, gb::FLAG_Z | gb::FLAG_N);
    }
    // test 2 - (A - B) when B > A
    {
        // given:
        cpu.AF.high = 0x01;
        cpu.AF.low = 0x00;
        cpu.BC.high = 0x02;
        mem.write(cpu.PC.full, CP_B);

        // when:
        const auto cycles = cpu.execute(mem);

        // then:
        EXPECT_EQ(cycles, 1);
        EXPECT_EQ(cpu.AF.low, gb::FLAG_C | gb::FLAG_N | gb::FLAG_H);
    }
}
