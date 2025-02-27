#pragma once
#include <cstdint>

static constexpr uint8_t
    // 8-bit Load Instructions
    LD_B_B       = 0x40,
    LD_B_C       = 0x41,
    LD_B_D       = 0x42,
    LD_B_E       = 0x43,
    LD_B_H       = 0x44,
    LD_B_L       = 0x45,
    LD_B_HL      = 0x46,
    LD_B_A       = 0x47,
    LD_C_B       = 0x48,
    LD_C_C       = 0x49,
    LD_C_D       = 0x4A,
    LD_C_E       = 0x4B,
    LD_C_H       = 0x4C,
    LD_C_L       = 0x4D,
    LD_C_HL      = 0x4E,
    LD_C_A       = 0x4F,
    LD_D_B       = 0x50,
    LD_D_C       = 0x51,
    LD_D_D       = 0x52,
    LD_D_E       = 0x53,
    LD_D_H       = 0x54,
    LD_D_L       = 0x55,
    LD_D_HL      = 0x56,
    LD_D_A       = 0x57,
    LD_E_B       = 0x58,
    LD_E_C       = 0x59,
    LD_E_D       = 0x5A,
    LD_E_E       = 0x5B,
    LD_E_H       = 0x5C,
    LD_E_L       = 0x5D,
    LD_E_HL      = 0x5E,
    LD_E_A       = 0x5F,
    LD_H_B       = 0x60,
    LD_H_C       = 0x61,
    LD_H_D       = 0x62,
    LD_H_E       = 0x63,
    LD_H_H       = 0x64,
    LD_H_L       = 0x65,
    LD_H_HL      = 0x66,
    LD_H_A       = 0x67,
    LD_L_B       = 0x68,
    LD_L_C       = 0x69,
    LD_L_D       = 0x6A,
    LD_L_E       = 0x6B,
    LD_L_H       = 0x6C,
    LD_L_L       = 0x6D,
    LD_L_HL      = 0x6E,
    LD_L_A       = 0x6F,
    LD_HL_B      = 0x70,
    LD_HL_C      = 0x71,
    LD_HL_D      = 0x72,
    LD_HL_E      = 0x73,
    LD_HL_H      = 0x74,
    LD_HL_L      = 0x75,
    LD_HL_A      = 0x77,
    LD_A_B       = 0x78,
    LD_A_C       = 0x79,
    LD_A_D       = 0x7A,
    LD_A_E       = 0x7B,
    LD_A_H       = 0x7C,
    LD_A_L       = 0x7D,
    LD_A_HL      = 0x7E,
    LD_A_A       = 0x7F,

    // 16-bit Load Instructions
    LD_BC_NN     = 0x01,
    LD_DE_NN     = 0x11,
    LD_HL_NN     = 0x21,
    LD_SP_NN     = 0x31,
    LD_SP_HL     = 0xF9,
    LD_HL_SPR    = 0xF8,    // LD HL, SP+r8
    LD_MM_SP     = 0x08,    // LD (a16), SP

    // Load Instructions with Immediate Data
    LD_B_N       = 0x06,
    LD_C_N       = 0x0E,
    LD_D_N       = 0x16,
    LD_E_N       = 0x1E,
    LD_H_N       = 0x26,
    LD_L_N       = 0x2E,
    LD_HL_N      = 0x36,
    LD_A_N       = 0x3E,

    // Load Instructions Using Memory
    LD_BC_A      = 0x02,    // LD (BC), A
    LD_DE_A      = 0x12,    // LD (DE), A
    LD_HLI_A     = 0x22,    // LD (HL+), A
    LD_HLD_A     = 0x32,    // LD (HL-), A
    LD_A_BC      = 0x0A,    // LD A, (BC)
    LD_A_DE      = 0x1A,    // LD A, (DE)
    LD_A_HLI     = 0x2A,    // LD A, (HL+)
    LD_A_HLD     = 0x3A,    // LD A, (HL-)
    LD_MM_A      = 0xEA,    // LD (a16), A
    LD_A_MM      = 0xFA,    // LD A, (a16)
    LDH_N_A      = 0xE0,    // LDH (a8), A
    LDH_A_N      = 0xF0,    // LDH A, (a8)
    LDH_C_A      = 0xE2,    // LDH (C), A
    LDH_A_C      = 0xF2,    // LDH A, (C)

    // Stack Operations
    PUSH_AF      = 0xF5,
    PUSH_BC      = 0xC5,
    PUSH_DE      = 0xD5,
    PUSH_HL      = 0xE5,
    POP_AF       = 0xF1,
    POP_BC       = 0xC1,
    POP_DE       = 0xD1,
    POP_HL       = 0xE1,

    // 8-bit Arithmetic/Logic
    ADD_A_A      = 0x87,
    ADD_A_B      = 0x80,
    ADD_A_C      = 0x81,
    ADD_A_D      = 0x82,
    ADD_A_E      = 0x83,
    ADD_A_H      = 0x84,
    ADD_A_L      = 0x85,
    ADD_A_HL     = 0x86,
    ADD_A_N      = 0xC6,    // ADD A, d8

    ADC_A_A      = 0x8F,
    ADC_A_B      = 0x88,
    ADC_A_C      = 0x89,
    ADC_A_D      = 0x8A,
    ADC_A_E      = 0x8B,
    ADC_A_H      = 0x8C,
    ADC_A_L      = 0x8D,
    ADC_A_HL     = 0x8E,
    ADC_A_N      = 0xCE,    // ADC A, d8

    SUB_A        = 0x97,
    SUB_B        = 0x90,
    SUB_C        = 0x91,
    SUB_D        = 0x92,
    SUB_E        = 0x93,
    SUB_H        = 0x94,
    SUB_L        = 0x95,
    SUB_HL       = 0x96,
    SUB_N        = 0xD6,    // SUB d8

    SBC_A_A      = 0x9F,
    SBC_A_B      = 0x98,
    SBC_A_C      = 0x99,
    SBC_A_D      = 0x9A,
    SBC_A_E      = 0x9B,
    SBC_A_H      = 0x9C,
    SBC_A_L      = 0x9D,
    SBC_A_HL     = 0x9E,
    SBC_A_N      = 0xDE,    // SBC A, d8

    // 16-bit Arithmetic
    ADD_HL_BC    = 0x09,
    ADD_HL_DE    = 0x19,
    ADD_HL_HL    = 0x29,
    ADD_HL_SP    = 0x39,
    ADD_SP_N     = 0xE8,    // ADD SP, r8

    // Increment/Decrement
    INC_A        = 0x3C,
    INC_B        = 0x04,
    INC_C        = 0x0C,
    INC_D        = 0x14,
    INC_E        = 0x1C,
    INC_H        = 0x24,
    INC_L        = 0x2C,
    INC_HL_MEM   = 0x34,    // INC (HL)
    INC_BC       = 0x03,
    INC_DE       = 0x13,
    INC_HL       = 0x23,
    INC_SP       = 0x33,

    DEC_A        = 0x3D,
    DEC_B        = 0x05,
    DEC_C        = 0x0D,
    DEC_D        = 0x15,
    DEC_E        = 0x1D,
    DEC_H        = 0x25,
    DEC_L        = 0x2D,
    DEC_HL_MEM   = 0x35,    // DEC (HL)
    DEC_BC       = 0x0B,
    DEC_DE       = 0x1B,
    DEC_HL       = 0x2B,
    DEC_SP       = 0x3B,

    // Bit Operations
    AND_A        = 0xA7,
    AND_B        = 0xA0,
    AND_C        = 0xA1,
    AND_D        = 0xA2,
    AND_E        = 0xA3,
    AND_H        = 0xA4,
    AND_L        = 0xA5,
    AND_HL       = 0xA6,
    AND_N        = 0xE6,    // AND d8

    OR_A         = 0xB7,
    OR_B         = 0xB0,
    OR_C         = 0xB1,
    OR_D         = 0xB2,
    OR_E         = 0xB3,
    OR_H         = 0xB4,
    OR_L         = 0xB5,
    OR_HL        = 0xB6,
    OR_N         = 0xF6,    // OR d8

    XOR_A        = 0xAF,
    XOR_B        = 0xA8,
    XOR_C        = 0xA9,
    XOR_D        = 0xAA,
    XOR_E        = 0xAB,
    XOR_H        = 0xAC,
    XOR_L        = 0xAD,
    XOR_HL       = 0xAE,
    XOR_N        = 0xEE,    // XOR d8

    CP_A         = 0xBF,
    CP_B         = 0xB8,
    CP_C         = 0xB9,
    CP_D         = 0xBA,
    CP_E         = 0xBB,
    CP_H         = 0xBC,
    CP_L         = 0xBD,
    CP_HL        = 0xBE,
    CP_N         = 0xFE,    // CP d8

    // Rotates and Shifts (non-CB)
    RLCA         = 0x07,
    RLA          = 0x17,
    RRCA         = 0x0F,
    RRA          = 0x1F,

    // Control Flow
    JP_NN        = 0xC3,
    JP_NZ_NN     = 0xC2,
    JP_Z_NN      = 0xCA,
    JP_NC_NN     = 0xD2,
    JP_C_NN      = 0xDA,
    JP_HL        = 0xE9,

    JR_N         = 0x18,
    JR_NZ_N      = 0x20,
    JR_Z_N       = 0x28,
    JR_NC_N      = 0x30,
    JR_C_N       = 0x38,

    // Subroutine Instructions
    CALL_NN      = 0xCD,
    CALL_NZ_NN   = 0xC4,
    CALL_Z_NN    = 0xCC,
    CALL_NC_NN   = 0xD4,
    CALL_C_NN    = 0xDC,

    RST_00       = 0xC7,
    RST_08       = 0xCF,
    RST_10       = 0xD7,
    RST_18       = 0xDF,
    RST_20       = 0xE7,
    RST_28       = 0xEF,
    RST_30       = 0xF7,
    RST_38       = 0xFF,

    RET          = 0xC9,
    RET_NZ       = 0xC0,
    RET_Z        = 0xC8,
    RET_NC       = 0xD0,
    RET_C        = 0xD8,
    RETI         = 0xD9,

    // CPU Control
    NOP          = 0x00,
    HALT         = 0x76,
    STOP         = 0x10,
    DI           = 0xF3,
    EI           = 0xFB,

    // Flag Operations
    CCF          = 0x3F,
    SCF          = 0x37,
    CPL          = 0x2F,
    DAA          = 0x27,

    // CB Prefix (for extended instructions)
    CB_PREFIX    = 0xCB
;

// CB-prefixed instructions
static constexpr uint8_t
    // Rotates and Shifts
    CB_RLC_B     = 0x00,
    CB_RLC_C     = 0x01,
    CB_RLC_D     = 0x02,
    CB_RLC_E     = 0x03,
    CB_RLC_H     = 0x04,
    CB_RLC_L     = 0x05,
    CB_RLC_HL    = 0x06,
    CB_RLC_A     = 0x07,

    CB_RRC_B     = 0x08,
    CB_RRC_C     = 0x09,
    CB_RRC_D     = 0x0A,
    CB_RRC_E     = 0x0B,
    CB_RRC_H     = 0x0C,
    CB_RRC_L     = 0x0D,
    CB_RRC_HL    = 0x0E,
    CB_RRC_A     = 0x0F,

    CB_RL_B      = 0x10,
    CB_RL_C      = 0x11,
    CB_RL_D      = 0x12,
    CB_RL_E      = 0x13,
    CB_RL_H      = 0x14,
    CB_RL_L      = 0x15,
    CB_RL_HL     = 0x16,
    CB_RL_A      = 0x17,

    CB_RR_B      = 0x18,
    CB_RR_C      = 0x19,
    CB_RR_D      = 0x1A,
    CB_RR_E      = 0x1B,
    CB_RR_H      = 0x1C,
    CB_RR_L      = 0x1D,
    CB_RR_HL     = 0x1E,
    CB_RR_A      = 0x1F,

    CB_SLA_B     = 0x20,
    CB_SLA_C     = 0x21,
    CB_SLA_D     = 0x22,
    CB_SLA_E     = 0x23,
    CB_SLA_H     = 0x24,
    CB_SLA_L     = 0x25,
    CB_SLA_HL    = 0x26,
    CB_SLA_A     = 0x27,

    CB_SRA_B     = 0x28,
    CB_SRA_C     = 0x29,
    CB_SRA_D     = 0x2A,
    CB_SRA_E     = 0x2B,
    CB_SRA_H     = 0x2C,
    CB_SRA_L     = 0x2D,
    CB_SRA_HL    = 0x2E,
    CB_SRA_A     = 0x2F,

    CB_SWAP_B    = 0x30,
    CB_SWAP_C    = 0x31,
    CB_SWAP_D    = 0x32,
    CB_SWAP_E    = 0x33,
    CB_SWAP_H    = 0x34,
    CB_SWAP_L    = 0x35,
    CB_SWAP_HL   = 0x36,
    CB_SWAP_A    = 0x37,

    CB_SRL_B     = 0x38,
    CB_SRL_C     = 0x39,
    CB_SRL_D     = 0x3A,
    CB_SRL_E     = 0x3B,
    CB_SRL_H     = 0x3C,
    CB_SRL_L     = 0x3D,
    CB_SRL_HL    = 0x3E,
    CB_SRL_A     = 0x3F,

    // Bit Tests
    CB_BIT_0_B   = 0x40,
    CB_BIT_0_C   = 0x41,
    CB_BIT_0_D   = 0x42,
    CB_BIT_0_E   = 0x43,
    CB_BIT_0_H   = 0x44,
    CB_BIT_0_L   = 0x45,
    CB_BIT_0_HL  = 0x46,
    CB_BIT_0_A   = 0x47,
    // ... (bits 1-7 follow same pattern with opcodes 48-7F)

    // Reset Bits
    CB_RES_0_B   = 0x80,
    CB_RES_0_C   = 0x81,
    CB_RES_0_D   = 0x82,
    CB_RES_0_E   = 0x83,
    CB_RES_0_H   = 0x84,
    CB_RES_0_L   = 0x85,
    CB_RES_0_HL  = 0x86,
    CB_RES_0_A   = 0x87,
    // ... (bits 1-7 follow same pattern with opcodes 88-BF)

    // Set Bits
    CB_SET_0_B   = 0xC0,
    CB_SET_0_C   = 0xC1,
    CB_SET_0_D   = 0xC2,
    CB_SET_0_E   = 0xC3,
    CB_SET_0_H   = 0xC4,
    CB_SET_0_L   = 0xC5,
    CB_SET_0_HL  = 0xC6,
    CB_SET_0_A   = 0xC7
    // ... (bits 1-7 follow same pattern with opcodes C8-FF)
;
