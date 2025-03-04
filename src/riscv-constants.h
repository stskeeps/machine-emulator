// Copyright 2019 Cartesi Pte. Ltd.
//
// This file is part of the machine-emulator. The machine-emulator is free
// software: you can redistribute it and/or modify it under the terms of the GNU
// Lesser General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
//
// The machine-emulator is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the machine-emulator. If not, see http://www.gnu.org/licenses/.
//

#ifndef RISCV_CONSTANTS_H
#define RISCV_CONSTANTS_H

#include "machine-c-defines.h"
#include <cstdint>
#include <pma-defines.h>

/// \file
/// \brief RISC-V constants

namespace cartesi {

/// \brief Global RISC-V constants
enum RISCV_constants {
    XLEN = 64,   ///< Maximum XLEN
    FLEN = 64,   ///< Maximum FLEN
    ASIDLEN = 0, ///< Number of implemented ASID bits
    ASIDMAX = 16 ///< Maximum number of implemented ASID bits
};

/// \brief Register counts
enum REG_COUNT { X_REG_COUNT = 32, F_REG_COUNT = 32, UARCH_X_REG_COUNT = 32 };

/// \brief MIP shifts
enum MIP_shifts {
    MIP_SSIP_SHIFT = 1,
    MIP_MSIP_SHIFT = 3,
    MIP_STIP_SHIFT = 5,
    MIP_MTIP_SHIFT = 7,
    MIP_SEIP_SHIFT = 9,
    MIP_MEIP_SHIFT = 11
};

/// \brief MIP masks
enum MIP_masks : uint64_t {
    MIP_SSIP_MASK = UINT64_C(1) << MIP_SSIP_SHIFT, ///< Supervisor software interrupt
    MIP_MSIP_MASK = UINT64_C(1) << MIP_MSIP_SHIFT, ///< Machine software interrupt
    MIP_STIP_MASK = UINT64_C(1) << MIP_STIP_SHIFT, ///< Supervisor timer interrupt
    MIP_MTIP_MASK = UINT64_C(1) << MIP_MTIP_SHIFT, ///< Machine timer interrupt
    MIP_SEIP_MASK = UINT64_C(1) << MIP_SEIP_SHIFT, ///< Supervisor external interrupt
    MIP_MEIP_MASK = UINT64_C(1) << MIP_MEIP_SHIFT  ///< Machine external interrupt
};

/// \brief mcause for exceptions
enum MCAUSE_constants : uint64_t {
    MCAUSE_INSN_ADDRESS_MISALIGNED = 0x0,      ///< Instruction address misaligned
    MCAUSE_INSN_ACCESS_FAULT = 0x1,            ///< Instruction access fault
    MCAUSE_ILLEGAL_INSN = 0x2,                 ///< Illegal instruction
    MCAUSE_BREAKPOINT = 0x3,                   ///< Breakpoint
    MCAUSE_LOAD_ADDRESS_MISALIGNED = 0x4,      ///< Load address misaligned
    MCAUSE_LOAD_ACCESS_FAULT = 0x5,            ///< Load access fault
    MCAUSE_STORE_AMO_ADDRESS_MISALIGNED = 0x6, ///< Store/AMO address misaligned
    MCAUSE_STORE_AMO_ACCESS_FAULT = 0x7,       ///< Store/AMO access fault
    MCAUSE_ECALL_BASE = 0x8,                   ///< Environment call (+0: from U-mode, +1: from S-mode, +3: from M-mode)
    MCAUSE_USER_ECALL = 0x8,                   ///< Environment call from U-mode
    MCAUSE_SUPERVISOR_ECALL = 0x9,             ///< Environment call from S-mode
    MCAUSE_MACHINE_ECALL = 0xb,                ///< Environment call from M-mode
    MCAUSE_FETCH_PAGE_FAULT = 0xc,             ///< Instruction page fault
    MCAUSE_LOAD_PAGE_FAULT = 0xd,              ///< Load page fault
    MCAUSE_STORE_AMO_PAGE_FAULT = 0xf,         ///< Store/AMO page fault

    MCAUSE_INTERRUPT_FLAG = UINT64_C(1) << (XLEN - 1) ///< Interrupt flag
};

/// \brief mcause exceptions masks
enum MCAUSE_masks : uint64_t {
    MCAUSE_INSN_ADDRESS_MISALIGNED_MASK = UINT64_C(1) << MCAUSE_INSN_ADDRESS_MISALIGNED,
    MCAUSE_INSN_ACCESS_FAULT_MASK = UINT64_C(1) << MCAUSE_INSN_ACCESS_FAULT,
    MCAUSE_ILLEGAL_INSN_MASK = UINT64_C(1) << MCAUSE_ILLEGAL_INSN,
    MCAUSE_BREAKPOINT_MASK = UINT64_C(1) << MCAUSE_BREAKPOINT,
    MCAUSE_LOAD_ADDRESS_MISALIGNED_MASK = UINT64_C(1) << MCAUSE_LOAD_ADDRESS_MISALIGNED,
    MCAUSE_LOAD_ACCESS_FAULT_MASK = UINT64_C(1) << MCAUSE_LOAD_ACCESS_FAULT,
    MCAUSE_STORE_AMO_ADDRESS_MISALIGNED_MASK = UINT64_C(1) << MCAUSE_STORE_AMO_ADDRESS_MISALIGNED,
    MCAUSE_STORE_AMO_ACCESS_FAULT_MASK = UINT64_C(1) << MCAUSE_STORE_AMO_ACCESS_FAULT,
    MCAUSE_USER_ECALL_MASK = UINT64_C(1) << MCAUSE_USER_ECALL,
    MCAUSE_SUPERVISOR_ECALL_MASK = UINT64_C(1) << MCAUSE_SUPERVISOR_ECALL,
    MCAUSE_MACHINE_ECALL_MASK = UINT64_C(1) << MCAUSE_MACHINE_ECALL,
    MCAUSE_FETCH_PAGE_FAULT_MASK = UINT64_C(1) << MCAUSE_FETCH_PAGE_FAULT,
    MCAUSE_LOAD_PAGE_FAULT_MASK = UINT64_C(1) << MCAUSE_LOAD_PAGE_FAULT,
    MCAUSE_STORE_AMO_PAGE_FAULT_MASK = UINT64_C(1) << MCAUSE_STORE_AMO_PAGE_FAULT
};

/// \brief medeleg read/write masks
enum MEDELEG_RW_masks : uint64_t {
    MEDELEG_W_MASK = MCAUSE_INSN_ADDRESS_MISALIGNED_MASK | MCAUSE_INSN_ACCESS_FAULT_MASK | MCAUSE_ILLEGAL_INSN_MASK |
        MCAUSE_BREAKPOINT_MASK | MCAUSE_LOAD_ADDRESS_MISALIGNED_MASK | MCAUSE_LOAD_ACCESS_FAULT_MASK |
        MCAUSE_STORE_AMO_ADDRESS_MISALIGNED_MASK | MCAUSE_STORE_AMO_ACCESS_FAULT_MASK | MCAUSE_USER_ECALL_MASK |
        MCAUSE_SUPERVISOR_ECALL_MASK | MCAUSE_FETCH_PAGE_FAULT_MASK | MCAUSE_LOAD_PAGE_FAULT_MASK |
        MCAUSE_STORE_AMO_PAGE_FAULT_MASK
};

/// \brief Privilege modes
enum PRV_constants : uint8_t {
    PRV_U = 0,  ///< User mode
    PRV_S = 1,  ///< Supervisor mode
    PRV_HS = 2, ///< Hypervisor-extended supervisor mode
    PRV_M = 3   ///< Machine mode
};

/// \brief misa shifts
enum MISA_shifts {
    MISA_EXT_S_SHIFT = ('S' - 'A'),
    MISA_EXT_U_SHIFT = ('U' - 'A'),
    MISA_EXT_I_SHIFT = ('I' - 'A'),
    MISA_EXT_M_SHIFT = ('M' - 'A'),
    MISA_EXT_A_SHIFT = ('A' - 'A'),
    MISA_EXT_F_SHIFT = ('F' - 'A'),
    MISA_EXT_D_SHIFT = ('D' - 'A'),
    MISA_EXT_C_SHIFT = ('C' - 'A'),

    MISA_MXL_SHIFT = (XLEN - 2)
};

/// \brief misa masks
enum MISA_masks : uint64_t {
    MISA_EXT_S_MASK = UINT64_C(1) << MISA_EXT_S_SHIFT, ///< Supervisor mode implemented
    MISA_EXT_U_MASK = UINT64_C(1) << MISA_EXT_U_SHIFT, ///< User mode implemented
    MISA_EXT_I_MASK = UINT64_C(1) << MISA_EXT_I_SHIFT, ///< RV32I/64I/128I base ISA
    MISA_EXT_M_MASK = UINT64_C(1) << MISA_EXT_M_SHIFT, ///< Integer Multiply/Divide extension
    MISA_EXT_A_MASK = UINT64_C(1) << MISA_EXT_A_SHIFT, ///< Atomic extension
    MISA_EXT_F_MASK = UINT64_C(1) << MISA_EXT_F_SHIFT, ///< Single-precision floating-point extension
    MISA_EXT_D_MASK = UINT64_C(1) << MISA_EXT_D_SHIFT, ///< Double-precision floating-point extension
    MISA_EXT_C_MASK = UINT64_C(1) << MISA_EXT_C_SHIFT, ///< Compressed extension
};

/// \brief misa constants
enum MISA_constants : uint64_t { MISA_MXL_VALUE = UINT64_C(2) };

/// \brief mstatus shifts
enum MSTATUS_shifts {
    MSTATUS_SIE_SHIFT = 1,
    MSTATUS_MIE_SHIFT = 3,
    MSTATUS_SPIE_SHIFT = 5,
    MSTATUS_UBE_SHIFT = 6,
    MSTATUS_MPIE_SHIFT = 7,
    MSTATUS_SPP_SHIFT = 8,
    MSTATUS_VS_SHIFT = 9,
    MSTATUS_MPP_SHIFT = 11,
    MSTATUS_FS_SHIFT = 13,
    MSTATUS_XS_SHIFT = 15,
    MSTATUS_MPRV_SHIFT = 17,
    MSTATUS_SUM_SHIFT = 18,
    MSTATUS_MXR_SHIFT = 19,
    MSTATUS_TVM_SHIFT = 20,
    MSTATUS_TW_SHIFT = 21,
    MSTATUS_TSR_SHIFT = 22,

    MSTATUS_UXL_SHIFT = 32,
    MSTATUS_SXL_SHIFT = 34,
    MSTATUS_SBE_SHIFT = 36,
    MSTATUS_MBE_SHIFT = 37,
    MSTATUS_SD_SHIFT = XLEN - 1
};

/// \brief mstatus masks
enum MSTATUS_masks : uint64_t {
    MSTATUS_SIE_MASK = UINT64_C(1) << MSTATUS_SIE_SHIFT,
    MSTATUS_MIE_MASK = UINT64_C(1) << MSTATUS_MIE_SHIFT,
    MSTATUS_SPIE_MASK = UINT64_C(1) << MSTATUS_SPIE_SHIFT,
    MSTATUS_UBE_MASK = UINT64_C(1) << MSTATUS_UBE_SHIFT,
    MSTATUS_MPIE_MASK = UINT64_C(1) << MSTATUS_MPIE_SHIFT,
    MSTATUS_SPP_MASK = UINT64_C(1) << MSTATUS_SPP_SHIFT,
    MSTATUS_VS_MASK = UINT64_C(3) << MSTATUS_VS_SHIFT,
    MSTATUS_MPP_MASK = UINT64_C(3) << MSTATUS_MPP_SHIFT,
    MSTATUS_FS_MASK = UINT64_C(3) << MSTATUS_FS_SHIFT,
    MSTATUS_XS_MASK = UINT64_C(3) << MSTATUS_XS_SHIFT,
    MSTATUS_MPRV_MASK = UINT64_C(1) << MSTATUS_MPRV_SHIFT,
    MSTATUS_SUM_MASK = UINT64_C(1) << MSTATUS_SUM_SHIFT,
    MSTATUS_MXR_MASK = UINT64_C(1) << MSTATUS_MXR_SHIFT,
    MSTATUS_TVM_MASK = UINT64_C(1) << MSTATUS_TVM_SHIFT,
    MSTATUS_TW_MASK = UINT64_C(1) << MSTATUS_TW_SHIFT,
    MSTATUS_TSR_MASK = UINT64_C(1) << MSTATUS_TSR_SHIFT,

    MSTATUS_UXL_MASK = UINT64_C(3) << MSTATUS_UXL_SHIFT,
    MSTATUS_SXL_MASK = UINT64_C(3) << MSTATUS_SXL_SHIFT,
    MSTATUS_SBE_MASK = UINT64_C(1) << MSTATUS_SBE_SHIFT,
    MSTATUS_MBE_MASK = UINT64_C(1) << MSTATUS_MBE_SHIFT,
    MSTATUS_SD_MASK = UINT64_C(1) << MSTATUS_SD_SHIFT,

    MSTATUS_FS_OFF = UINT64_C(0) << MSTATUS_FS_SHIFT,
    MSTATUS_FS_INITIAL = UINT64_C(1) << MSTATUS_FS_SHIFT,
    MSTATUS_FS_CLEAN = UINT64_C(2) << MSTATUS_FS_SHIFT,
    MSTATUS_FS_DIRTY = UINT64_C(3) << MSTATUS_FS_SHIFT
};

/// \brief mstatus read-write masks
enum MSTATUS_RW_masks : uint64_t {
    MSTATUS_W_MASK = (MSTATUS_SIE_MASK | MSTATUS_MIE_MASK | MSTATUS_SPIE_MASK | MSTATUS_MPIE_MASK | MSTATUS_SPP_MASK |
        MSTATUS_MPP_MASK | MSTATUS_FS_MASK | MSTATUS_MPRV_MASK | MSTATUS_SUM_MASK | MSTATUS_MXR_MASK |
        MSTATUS_TVM_MASK | MSTATUS_TW_MASK | MSTATUS_TSR_MASK), ///< Write mask for mstatus
    MSTATUS_R_MASK = (MSTATUS_SIE_MASK | MSTATUS_MIE_MASK | MSTATUS_SPIE_MASK | MSTATUS_UBE_MASK | MSTATUS_MPIE_MASK |
        MSTATUS_SPP_MASK | MSTATUS_MPP_MASK | MSTATUS_FS_MASK | MSTATUS_VS_MASK | MSTATUS_MPRV_MASK | MSTATUS_SUM_MASK |
        MSTATUS_MXR_MASK | MSTATUS_TVM_MASK | MSTATUS_TW_MASK | MSTATUS_TSR_MASK | MSTATUS_UXL_MASK | MSTATUS_SXL_MASK |
        MSTATUS_SBE_MASK | MSTATUS_MBE_MASK | MSTATUS_SD_MASK) ///< Read mask for mstatus
};

/// \brief sstatus read/write masks
enum SSTATUS_rw_masks : uint64_t {
    SSTATUS_W_MASK = (MSTATUS_SIE_MASK | MSTATUS_SPIE_MASK | MSTATUS_SPP_MASK | MSTATUS_FS_MASK | MSTATUS_SUM_MASK |
        MSTATUS_MXR_MASK), ///< Write mask for sstatus
    SSTATUS_R_MASK = (MSTATUS_SIE_MASK | MSTATUS_SPIE_MASK | MSTATUS_UBE_MASK | MSTATUS_SPP_MASK | MSTATUS_VS_MASK |
        MSTATUS_FS_MASK | MSTATUS_XS_MASK | MSTATUS_SUM_MASK | MSTATUS_MXR_MASK | MSTATUS_UXL_MASK |
        MSTATUS_SD_MASK) ///< Read mask for sstatus
};

/// \brief satp shifts
enum SATP_shifts : uint64_t { SATP_PPN_SHIFT = 0, SATP_ASID_SHIFT = 44, SATP_MODE_SHIFT = 60 };

/// \brief satp masks
enum SATP_masks : uint64_t {
    SATP_PPN_MASK = (UINT64_C(1) << SATP_ASID_SHIFT) - 1,
    SATP_ASID_MASK = ((UINT64_C(1) << ASIDLEN) - 1) << SATP_ASID_SHIFT,
    SATP_MODE_MASK = UINT64_C(15) << SATP_MODE_SHIFT,
};

/// \brief satp modes
enum SATP_modes : uint64_t {
    SATP_MODE_BARE = 0,
    SATP_MODE_SV39 = 8,
    SATP_MODE_SV48 = 9,
    SATP_MODE_SV57 = 10,
};

/// \brief ASID masks
enum ASID_masks : uint64_t { ASID_R_MASK = (UINT64_C(1) << ASIDLEN) - 1, ASID_MAX_MASK = (UINT64_C(1) << ASIDMAX) - 1 };

/// \brief Page-table entry shifts
enum PTE_shifts {
    PTE_XWR_R_SHIFT = 0,
    PTE_XWR_W_SHIFT = 1,
    PTE_XWR_X_SHIFT = 2,
    PTE_V_SHIFT = 0,
    PTE_R_SHIFT = 1,
    PTE_W_SHIFT = 2,
    PTE_X_SHIFT = 3,
    PTE_U_SHIFT = 4,
    PTE_G_SHIFT = 5,
    PTE_A_SHIFT = 6,
    PTE_D_SHIFT = 7,
    PTE_PPN_SHIFT = 10,
    PTE_PBMT_SHIFT = 61,
    PTE_N_SHIFT = 63
};

/// \brief Page-table entry masks
enum PTE_masks : uint64_t {
    PTE_XWR_R_MASK = UINT64_C(1) << PTE_XWR_R_SHIFT,
    PTE_XWR_W_MASK = UINT64_C(1) << PTE_XWR_W_SHIFT,
    PTE_XWR_X_MASK = UINT64_C(1) << PTE_XWR_X_SHIFT,
    PTE_V_MASK = UINT64_C(1) << PTE_V_SHIFT,                   ///< Valid
    PTE_R_MASK = UINT64_C(1) << PTE_R_SHIFT,                   ///< Readable
    PTE_W_MASK = UINT64_C(1) << PTE_W_SHIFT,                   ///< Writable
    PTE_X_MASK = UINT64_C(1) << PTE_X_SHIFT,                   ///< Executable
    PTE_U_MASK = UINT64_C(1) << PTE_U_SHIFT,                   ///< Accessible to user mode
    PTE_G_MASK = UINT64_C(1) << PTE_G_SHIFT,                   ///< Global mapping
    PTE_A_MASK = UINT64_C(1) << PTE_A_SHIFT,                   ///< Accessed
    PTE_D_MASK = UINT64_C(1) << PTE_D_SHIFT,                   ///< Dirty
    PTE_PPN_MASK = ((UINT64_C(1) << 44) - 1) << PTE_PPN_SHIFT, ///< Physical page number
    PTE_60_54_MASK = UINT64_C(127) << 54,                      ///< Reserved for future use
    PTE_PBMT_MASK = UINT64_C(3) << PTE_PBMT_SHIFT,             ///< Svpbmt: Page-based memory types
    PTE_N_MASK = UINT64_C(1) << PTE_N_SHIFT                    ///< Svnapot: NAPOT translation contiguity
};

/// \brief menvcfg shifts
enum MENVCFG_shifts {
    MENVCFG_FIOM_SHIFT = 0,
    MENVCFG_CBIE_SHIFT = 4,
    MENVCFG_CBCFE_SHIFT = 6,
    MENVCFG_CBZE_SHIFT = 7,
    MENVCFG_PBMTE_SHIFT = 62,
    MENVCFG_STCE_SHIFT = 63
};

/// \brief menvcfg masks
enum MENVCFG_masks : uint64_t {
    MENVCFG_FIOM_MASK = UINT64_C(1) << MENVCFG_FIOM_SHIFT,   // Fence of I/O implies Memory
    MENVCFG_CBIE_MASK = UINT64_C(3) << MENVCFG_CBIE_SHIFT,   // forthcoming Zicbom extension
    MENVCFG_CBCFE_MASK = UINT64_C(1) << MENVCFG_CBCFE_SHIFT, // forthcoming Zicbom extension
    MENVCFG_CBZE_MASK = UINT64_C(1) << MENVCFG_CBZE_SHIFT,   // forthcoming Zicboz extension
    MENVCFG_PBMTE_MASK = UINT64_C(1) << MENVCFG_PBMTE_SHIFT, // Svpbmt extension
    MENVCFG_STCE_MASK = UINT64_C(1) << MENVCFG_STCE_SHIFT    // forthcoming Sstc extension
};

/// \brief senvcfg shifts
enum SENVCFG_shifts {
    SENVCFG_FIOM_SHIFT = 0,
    SENVCFG_CBIE_SHIFT = 4,
    SENVCFG_CBCFE_SHIFT = 6,
    SENVCFG_CBZE_SHIFT = 7,
};

/// \brief senvcfg masks
enum SENVCFG_masks : uint64_t {
    SENVCFG_FIOM_MASK = UINT64_C(1) << SENVCFG_FIOM_SHIFT,   // Fence of I/O implies Memory
    SENVCFG_CBIE_MASK = UINT64_C(3) << SENVCFG_CBIE_SHIFT,   // forthcoming Zicbom extension
    SENVCFG_CBCFE_MASK = UINT64_C(1) << SENVCFG_CBCFE_SHIFT, // forthcoming Zicbom extension
    SENVCFG_CBZE_MASK = UINT64_C(1) << SENVCFG_CBZE_SHIFT,   // forthcoming Zicboz extension
};

///< menvcfg read/write masks. Svpbmt is not implemented, thus ignoring PBMT bit
///  as it is always read-only zero. The rest extensions are not specified yet.
enum MENVCFG_RW_masks : uint64_t {
    MENVCFG_W_MASK = MENVCFG_FIOM_MASK, ///< write mask for menvcfg
    MENVCFG_R_MASK = MENVCFG_FIOM_MASK, ///< read mask for menvcfg
};

///< senvcfg read/write masks. Zicbom/Zicboz extensions are not specified yet, thus ignoring
///  the correspoding bits.
enum SENVCFG_RW_masks : uint64_t {
    SENVCFG_W_MASK = SENVCFG_FIOM_MASK, ///< write mask for senvcfg
    SENVCFG_R_MASK = SENVCFG_FIOM_MASK, ///< read mask for senvcfg
};

/// \brief fcsr fflags shifts
enum FFLAGS_shifts {
    FFLAGS_NX_SHIFT = 0, // Inexact
    FFLAGS_UF_SHIFT = 1, // Underflow
    FFLAGS_OF_SHIFT = 2, // Overflow
    FFLAGS_DZ_SHIFT = 3, // Divide by zero
    FFLAGS_NV_SHIFT = 4, // Invalid op
};

/// \brief fcsr fflags masks
enum FFLAGS_masks : uint64_t {
    FFLAGS_RW_MASK = 0b11111,
    FFLAGS_NX_MASK = UINT64_C(1) << FFLAGS_NX_SHIFT,
    FFLAGS_UF_MASK = UINT64_C(1) << FFLAGS_UF_SHIFT,
    FFLAGS_OF_MASK = UINT64_C(1) << FFLAGS_OF_SHIFT,
    FFLAGS_DZ_MASK = UINT64_C(1) << FFLAGS_DZ_SHIFT,
    FFLAGS_NV_MASK = UINT64_C(1) << FFLAGS_NV_SHIFT
};

/// \brief fcsr frm modes
enum FRM_modes : uint32_t {
    FRM_RNE = UINT32_C(0b000),
    FRM_RTZ = UINT32_C(0b001),
    FRM_RDN = UINT32_C(0b010),
    FRM_RUP = UINT32_C(0b011),
    FRM_RMM = UINT32_C(0b100),
    FRM_DYN = UINT32_C(0b111)
};

/// \brief float classes
enum FCLASS_classes : uint32_t {
    FCLASS_NINF = 1 << 0,
    FCLASS_NNORMAL = 1 << 1,
    FCLASS_NSUBNORMAL = 1 << 2,
    FCLASS_NZERO = 1 << 3,
    FCLASS_PZERO = 1 << 4,
    FCLASS_PSUBNORMAL = 1 << 5,
    FCLASS_PNORMAL = 1 << 6,
    FCLASS_PINF = 1 << 7,
    FCLASS_SNAN = 1 << 8,
    FCLASS_QNAN = 1 << 9
};

/// \brief frm masks
enum FCSR_FRM_masks : uint64_t { FRM_RW_MASK = 0b111 };

/// \brief fcsr shifts
enum FCSR_shifts {
    FCSR_FFLAGS_SHIFT = 0,
    FCSR_FRM_SHIFT = 5,
};

/// \brief fcsr masks
enum FCSR_rw_masks : uint64_t {
    FCSR_FFLAGS_RW_MASK = FFLAGS_RW_MASK << FCSR_FFLAGS_SHIFT,
    FCSR_FRM_RW_MASK = FRM_RW_MASK << FCSR_FRM_SHIFT,
    FCSR_RW_MASK = FCSR_FFLAGS_RW_MASK | FCSR_FRM_RW_MASK
};

/// \brief Translate virtual address constants
enum TRANSLATE_VADDR_constants {
    LOG2_PAGE_SIZE = 12,
    LOG2_PTE_SIZE = 3,                              ///< All page table entries have 8 bytes
    LOG2_VPN_SIZE = LOG2_PAGE_SIZE - LOG2_PTE_SIZE, ///< Number of bits to index all PTE entries
};

/// \brief Translate virtual address masks
enum TRANSLATE_VADDR_masks : uint64_t {
    PAGE_OFFSET_MASK = (UINT64_C(1) << LOG2_PAGE_SIZE) - 1,
    VPN_MASK = (UINT64_C(1) << LOG2_VPN_SIZE) - 1
};

/// \brief mcounteren shifts
enum MCOUNTEREN_shifts { MCOUNTEREN_CY_SHIFT = 0, MCOUNTEREN_TM_SHIFT = 1, MCOUNTEREN_IR_SHIFT = 2 };

/// \brief mcounteren masks
enum MCOUNTEREN_masks : uint64_t {
    MCOUNTEREN_CY_MASK = UINT64_C(1) << MCOUNTEREN_CY_SHIFT, ///< Enable rdcycle
    MCOUNTEREN_TM_MASK = UINT64_C(1) << MCOUNTEREN_TM_SHIFT, ///< Enable rdtime
    MCOUNTEREN_IR_MASK = UINT64_C(1) << MCOUNTEREN_IR_SHIFT, ///< Enable rdinstret
};

/// \brief counteren write masks
enum COUNTEREN_rw_masks : uint64_t {
    MCOUNTEREN_R_MASK = UINT64_C(0xffffffff),
    MCOUNTEREN_RW_MASK = MCOUNTEREN_CY_MASK | MCOUNTEREN_TM_MASK | MCOUNTEREN_IR_MASK,
    SCOUNTEREN_RW_MASK = MCOUNTEREN_RW_MASK
};

/// \brief Cartesi-specific iflags shifts
enum IFLAGS_shifts { IFLAGS_H_SHIFT = 0, IFLAGS_Y_SHIFT = 1, IFLAGS_X_SHIFT = 2, IFLAGS_PRV_SHIFT = 3 };

enum IFLAGS_masks : uint64_t {
    IFLAGS_H_MASK = UINT64_C(1) << IFLAGS_H_SHIFT,
    IFLAGS_Y_MASK = UINT64_C(1) << IFLAGS_Y_SHIFT,
    IFLAGS_X_MASK = UINT64_C(1) << IFLAGS_X_SHIFT,
    IFLAGS_PRV_MASK = UINT64_C(3) << IFLAGS_PRV_SHIFT
};

/// \brief Initial values for Cartesi machines
enum CARTESI_init : uint64_t {
    PC_INIT = UINT64_C(0x1000),                    ///< Initial value for pc
    FCSR_INIT = UINT64_C(0),                       ///< Initial value for fcsr
    MVENDORID_INIT = UINT64_C(0x6361727465736920), ///< Initial value for mvendorid
    MARCHID_INIT = CM_MARCHID,                     ///< Initial value for marchid
    MIMPID_INIT = CM_MIMPID,                       ///< Initial value for mimpid
    MCYCLE_INIT = UINT64_C(0),                     ///< Initial value for mcycle
    ICYCLEINSTRET_INIT = UINT64_C(0),              ///< Initial value for icycleinstret
    MSTATUS_INIT =
        (MISA_MXL_VALUE << MSTATUS_UXL_SHIFT) | (MISA_MXL_VALUE << MSTATUS_SXL_SHIFT), ///< Initial value for mstatus
    MTVEC_INIT = UINT64_C(0),                                                          ///< Initial value for mtvec
    MSCRATCH_INIT = UINT64_C(0),                                                       ///< Initial value for mscratch
    MEPC_INIT = UINT64_C(0),                                                           ///< Initial value for mepc
    MCAUSE_INIT = UINT64_C(0),                                                         ///< Initial value for mcause
    MTVAL_INIT = UINT64_C(0),                                                          ///< Initial value for mtval
    MISA_INIT = (MISA_MXL_VALUE << MISA_MXL_SHIFT) | MISA_EXT_S_MASK | MISA_EXT_U_MASK | MISA_EXT_I_MASK |
        MISA_EXT_M_MASK | MISA_EXT_A_MASK | MISA_EXT_F_MASK | MISA_EXT_D_MASK |
        MISA_EXT_C_MASK,                                            ///< Initial value for misa
    MIE_INIT = UINT64_C(0),                                         ///< Initial value for mie
    MIP_INIT = UINT64_C(0),                                         ///< Initial value for mip
    MEDELEG_INIT = UINT64_C(0),                                     ///< Initial value for medeleg
    MIDELEG_INIT = UINT64_C(0),                                     ///< Initial value for mideleg
    MCOUNTEREN_INIT = UINT64_C(0),                                  ///< Initial value for mcounteren
    STVEC_INIT = UINT64_C(0),                                       ///< Initial value for stvec
    SSCRATCH_INIT = UINT64_C(0),                                    ///< Initial value for sscratch
    SEPC_INIT = UINT64_C(0),                                        ///< Initial value for sepc
    SCAUSE_INIT = UINT64_C(0),                                      ///< Initial value for scause
    STVAL_INIT = UINT64_C(0),                                       ///< Initial value for stval
    SATP_INIT = UINT64_C(0),                                        ///< Initial value for satp
    SCOUNTEREN_INIT = UINT64_C(0),                                  ///< Initial value for scounteren
    ILRSC_INIT = UINT64_C(-1),                                      ///< Initial value for ilrsc
    IFLAGS_INIT = static_cast<uint64_t>(PRV_M) << IFLAGS_PRV_SHIFT, ///< Initial value for iflags
    MTIMECMP_INIT = UINT64_C(0),                                    ///< Initial value for mtimecmp
    FROMHOST_INIT = UINT64_C(0),                                    ///< Initial value for fromhost
    TOHOST_INIT = UINT64_C(0),                                      ///< Initial value for tohost
    MENVCFG_INIT = UINT64_C(0),                                     ///< Initial value for menvcfg
    SENVCFG_INIT = UINT64_C(0),                                     ///< Initial value for senvcfg
    UARCH_PC_INIT = UINT64_C(0x70000000),                           ///< Initial value for microarchitecture pc
    UARCH_CYCLE_INIT = UINT64_C(0),                                 ///< Initial value for microarchitecture cycle
};

/// \brief Mapping between CSR names and addresses
enum class CSR_address : uint32_t {
    fflags = 0x001,
    frm = 0x002,
    fcsr = 0x003,

    ucycle = 0xc00,
    utime = 0xc01,
    uinstret = 0xc02,

    sstatus = 0x100,
    sie = 0x104,
    stvec = 0x105,
    scounteren = 0x106,

    senvcfg = 0x10A,

    sscratch = 0x140,
    sepc = 0x141,
    scause = 0x142,
    stval = 0x143,
    sip = 0x144,

    satp = 0x180,

    mvendorid = 0xf11,
    marchid = 0xf12,
    mimpid = 0xf13,
    mhartid = 0xf14,
    mconfigptr = 0xf15,

    mstatus = 0x300,
    misa = 0x301,
    medeleg = 0x302,
    mideleg = 0x303,
    mie = 0x304,
    mtvec = 0x305,
    mcounteren = 0x306,

    menvcfg = 0x30a,

    mscratch = 0x340,
    mepc = 0x341,
    mcause = 0x342,
    mtval = 0x343,
    mip = 0x344,

    mcycle = 0xb00,
    minstret = 0xb02,
    mhpmcounter3 = 0xb03,
    mhpmcounter4 = 0xb04,
    mhpmcounter5 = 0xb05,
    mhpmcounter6 = 0xb06,
    mhpmcounter7 = 0xb07,
    mhpmcounter8 = 0xb08,
    mhpmcounter9 = 0xb09,
    mhpmcounter10 = 0xb0a,
    mhpmcounter11 = 0xb0b,
    mhpmcounter12 = 0xb0c,
    mhpmcounter13 = 0xb0d,
    mhpmcounter14 = 0xb0e,
    mhpmcounter15 = 0xb0f,
    mhpmcounter16 = 0xb10,
    mhpmcounter17 = 0xb11,
    mhpmcounter18 = 0xb12,
    mhpmcounter19 = 0xb13,
    mhpmcounter20 = 0xb14,
    mhpmcounter21 = 0xb15,
    mhpmcounter22 = 0xb16,
    mhpmcounter23 = 0xb17,
    mhpmcounter24 = 0xb18,
    mhpmcounter25 = 0xb19,
    mhpmcounter26 = 0xb1a,
    mhpmcounter27 = 0xb1b,
    mhpmcounter28 = 0xb1c,
    mhpmcounter29 = 0xb1d,
    mhpmcounter30 = 0xb1e,
    mhpmcounter31 = 0xb1f,

    mcountinhibit = 0x320,
    mhpmevent3 = 0x323,
    mhpmevent4 = 0x324,
    mhpmevent5 = 0x325,
    mhpmevent6 = 0x326,
    mhpmevent7 = 0x327,
    mhpmevent8 = 0x328,
    mhpmevent9 = 0x329,
    mhpmevent10 = 0x32a,
    mhpmevent11 = 0x32b,
    mhpmevent12 = 0x32c,
    mhpmevent13 = 0x32d,
    mhpmevent14 = 0x32e,
    mhpmevent15 = 0x32f,
    mhpmevent16 = 0x330,
    mhpmevent17 = 0x331,
    mhpmevent18 = 0x332,
    mhpmevent19 = 0x333,
    mhpmevent20 = 0x334,
    mhpmevent21 = 0x335,
    mhpmevent22 = 0x336,
    mhpmevent23 = 0x337,
    mhpmevent24 = 0x338,
    mhpmevent25 = 0x339,
    mhpmevent26 = 0x33a,
    mhpmevent27 = 0x33b,
    mhpmevent28 = 0x33c,
    mhpmevent29 = 0x33d,
    mhpmevent30 = 0x33e,
    mhpmevent31 = 0x33f,

    tselect = 0x7a0,
    tdata1 = 0x7a1,
    tdata2 = 0x7a2,
    tdata3 = 0x7a3,
};

/// \brief The result of insn & 0b1110000000000011 can be used to identify
/// most compressed instructions directly
enum class insn_c_funct3 : uint32_t {
    // Quadrant 0
    C_ADDI4SPN = 0b0000000000000000,
    C_FLD = 0b0010000000000000,
    C_LW = 0b0100000000000000,
    C_LD = 0b0110000000000000,
    C_FSD = 0b1010000000000000,
    C_SW = 0b1100000000000000,
    C_SD = 0b1110000000000000,

    // Quadrant 1
    C_Q1_SET0 = 0b0000000000000001, // C_NOP and C_ADDI
    C_ADDIW = 0b0010000000000001,
    C_LI = 0b0100000000000001,
    C_Q1_SET1 = 0b0110000000000001, // C_ADDI16SP and C_LUI
    C_Q1_SET2 = 0b1000000000000001, // C_SRLI64, C_SRAI64, C_ANDI, C_SUB
                                    // C_XOR, C_OR, C_AND, C_SUBW and C_ADDW
    C_J = 0b1010000000000001,
    C_BEQZ = 0b1100000000000001,
    C_BNEZ = 0b1110000000000001,

    // Quadrant 2
    C_SLLI = 0b0000000000000010,
    C_FLDSP = 0b0010000000000010,
    C_LWSP = 0b0100000000000010,
    C_LDSP = 0b0110000000000010,
    C_Q2_SET0 = 0b1000000000000010, // C_JR, C_MV, C_EBREAK, C_JALR, C_ADD
    C_FSDSP = 0b1010000000000010,
    C_SWSP = 0b1100000000000010,
    C_SDSP = 0b1110000000000010,
};

/// \brief The result of insn & 0b1110110000000011 can be used to identify
/// most compressed instructions directly
enum class insn_CB_funct2 : uint32_t {
    C_SRLI = 0b1000000000000001,
    C_SRAI = 0b1000010000000001,
    C_ANDI = 0b1000100000000001,
};

/// \brief The result of insn & 0b1111110001100011 can be used to identify
/// most compressed instructions directly
enum class insn_CA_funct6_funct2 : uint32_t {
    C_SUB = 0b1000110000000001,
    C_XOR = 0b1000110000100001,
    C_OR = 0b1000110001000001,
    C_AND = 0b1000110001100001,
    C_SUBW = 0b1001110000000001,
    C_ADDW = 0b1001110000100001,
};

/// \brief The result of insn & 0b111000001111111 can be used to identify
/// most instructions directly
enum class insn_funct3_00000_opcode : uint32_t {
    LB = 0b000000000000011,
    LH = 0b001000000000011,
    LW = 0b010000000000011,
    LD = 0b011000000000011,
    LBU = 0b100000000000011,
    LHU = 0b101000000000011,
    LWU = 0b110000000000011,
    SB = 0b000000000100011,
    SH = 0b001000000100011,
    SW = 0b010000000100011,
    SD = 0b011000000100011,
    FENCE = 0b000000000001111,
    FENCE_I = 0b001000000001111,
    ADDI = 0b000000000010011,
    SLLI = 0b001000000010011,
    SLTI = 0b010000000010011,
    SLTIU = 0b011000000010011,
    XORI = 0b100000000010011,
    ORI = 0b110000000010011,
    ANDI = 0b111000000010011,
    ADDIW = 0b000000000011011,
    SLLIW = 0b001000000011011,
    SLLW = 0b001000000111011,
    DIVW = 0b100000000111011,
    REMW = 0b110000000111011,
    REMUW = 0b111000000111011,
    BEQ = 0b000000001100011,
    BNE = 0b001000001100011,
    BLT = 0b100000001100011,
    BGE = 0b101000001100011,
    BLTU = 0b110000001100011,
    BGEU = 0b111000001100011,
    JALR = 0b000000001100111,
    CSRRW = 0b001000001110011,
    CSRRS = 0b010000001110011,
    CSRRC = 0b011000001110011,
    CSRRWI = 0b101000001110011,
    CSRRSI = 0b110000001110011,
    CSRRCI = 0b111000001110011,
    AUIPC_000 = 0b000000000010111,
    AUIPC_001 = 0b001000000010111,
    AUIPC_010 = 0b010000000010111,
    AUIPC_011 = 0b011000000010111,
    AUIPC_100 = 0b100000000010111,
    AUIPC_101 = 0b101000000010111,
    AUIPC_110 = 0b110000000010111,
    AUIPC_111 = 0b111000000010111,
    LUI_000 = 0b000000000110111,
    LUI_001 = 0b001000000110111,
    LUI_010 = 0b010000000110111,
    LUI_011 = 0b011000000110111,
    LUI_100 = 0b100000000110111,
    LUI_101 = 0b101000000110111,
    LUI_110 = 0b110000000110111,
    LUI_111 = 0b111000000110111,
    JAL_000 = 0b000000001101111,
    JAL_001 = 0b001000001101111,
    JAL_010 = 0b010000001101111,
    JAL_011 = 0b011000001101111,
    JAL_100 = 0b100000001101111,
    JAL_101 = 0b101000001101111,
    JAL_110 = 0b110000001101111,
    JAL_111 = 0b111000001101111,
    FSW = 0b010000000100111,
    FSD = 0b011000000100111,
    FLW = 0b010000000000111,
    FLD = 0b011000000000111,
    FMADD_RNE = 0b000000001000011,
    FMADD_RTZ = 0b001000001000011,
    FMADD_RDN = 0b010000001000011,
    FMADD_RUP = 0b011000001000011,
    FMADD_RMM = 0b100000001000011,
    FMADD_DYN = 0b111000001000011,
    FMSUB_RNE = 0b000000001000111,
    FMSUB_RTZ = 0b001000001000111,
    FMSUB_RDN = 0b010000001000111,
    FMSUB_RUP = 0b011000001000111,
    FMSUB_RMM = 0b100000001000111,
    FMSUB_DYN = 0b111000001000111,
    FNMSUB_RNE = 0b000000001001011,
    FNMSUB_RTZ = 0b001000001001011,
    FNMSUB_RDN = 0b010000001001011,
    FNMSUB_RUP = 0b011000001001011,
    FNMSUB_RMM = 0b100000001001011,
    FNMSUB_DYN = 0b111000001001011,
    FNMADD_RNE = 0b000000001001111,
    FNMADD_RTZ = 0b001000001001111,
    FNMADD_RDN = 0b010000001001111,
    FNMADD_RUP = 0b011000001001111,
    FNMADD_RMM = 0b100000001001111,
    FNMADD_DYN = 0b111000001001111,
    // some instructions need additional inspection of funct7 (or part thereof)
    FD_000 = 0b000000001010011,
    FD_001 = 0b001000001010011,
    FD_010 = 0b010000001010011,
    FD_011 = 0b011000001010011,
    FD_100 = 0b100000001010011,
    FD_111 = 0b111000001010011,
    SRLI_SRAI = 0b101000000010011,
    SRLIW_SRAIW = 0b101000000011011,
    AMO_W = 0b010000000101111,
    AMO_D = 0b011000000101111,
    ADD_MUL_SUB = 0b000000000110011,
    SLL_MULH = 0b001000000110011,
    SLT_MULHSU = 0b010000000110011,
    SLTU_MULHU = 0b011000000110011,
    XOR_DIV = 0b100000000110011,
    SRL_DIVU_SRA = 0b101000000110011,
    OR_REM = 0b110000000110011,
    AND_REMU = 0b111000000110011,
    ADDW_MULW_SUBW = 0b000000000111011,
    SRLW_DIVUW_SRAW = 0b101000000111011,
    privileged = 0b000000001110011,
};

/// \brief The result of insn >> 26 (6 most significant bits of funct7) can be
/// used to identify the SRI instructions
enum insn_SRLI_SRAI_funct7_sr1 : uint32_t { SRLI = 0b000000, SRAI = 0b010000 };

/// \brief funct7 constants for SRW instructions
enum insn_SRLIW_SRAIW_funct7 : uint32_t { SRLIW = 0b0000000, SRAIW = 0b0100000 };

/// \brief The result of insn >> 27 (5 most significant bits of funct7) can be
/// used to identify the atomic operation
enum insn_AMO_funct7_sr2 : uint32_t {
    AMOADD = 0b00000,
    AMOSWAP = 0b00001,
    LR = 0b00010,
    SC = 0b00011,
    AMOXOR = 0b00100,
    AMOOR = 0b01000,
    AMOAND = 0b01100,
    AMOMIN = 0b10000,
    AMOMAX = 0b10100,
    AMOMINU = 0b11000,
    AMOMAXU = 0b11100
};

/// \brief funct7 constants for ADD, MUL, SUB instructions
enum insn_ADD_MUL_SUB_funct7 : uint32_t { ADD = 0b0000000, MUL = 0b0000001, SUB = 0b0100000 };

/// \brief funct7 constants for SLL, MULH instructions
enum insn_SLL_MULH_funct7 : uint32_t { SLL = 0b0000000, MULH = 0b0000001 };

/// \brief funct7 constants for SLT, MULHSU instructions
enum insn_SLT_MULHSU_funct7 : uint32_t { SLT = 0b0000000, MULHSU = 0b0000001 };

/// \brief funct7 constants for SLTU, MULHU instructions
enum insn_SLTU_MULHU_funct7 : uint32_t { SLTU = 0b0000000, MULHU = 0b0000001 };

/// \brief funct7 constants for XOR, DIV instructions
enum insn_XOR_DIV_funct7 : uint32_t {
    XOR = 0b0000000,
    DIV = 0b0000001,
};

/// \brief funct7 constants for SRL, DIVU, SRA instructions
enum insn_SRL_DIVU_SRA_funct7 : uint32_t {
    SRL = 0b0000000,
    DIVU = 0b0000001,
    SRA = 0b0100000,
};

/// \brief funct7 constants for floating-point instructions
enum insn_FD_funct7 : uint32_t {
    FADD_S = 0b0000000,
    FADD_D = 0b0000001,
    FSUB_S = 0b0000100,
    FSUB_D = 0b0000101,
    FMUL_S = 0b0001000,
    FMUL_D = 0b0001001,
    FDIV_S = 0b0001100,
    FDIV_D = 0b0001101,
    FSGN_S = 0b0010000,
    FSGN_D = 0b0010001,
    FMINMAX_S = 0b0010100,
    FMINMAX_D = 0b0010101,
    FSQRT_S = 0b0101100,
    FSQRT_D = 0b0101101,
    FCMP_S = 0b1010000,
    FCMP_D = 0b1010001
};

/// \brief funct7_rs2 constants for floating-point instructions
enum insn_FD_funct7_rs2 : uint32_t {
    FCVT_W_S = 0b110000000000,
    FCVT_WU_S = 0b110000000001,
    FCVT_L_S = 0b110000000010,
    FCVT_LU_S = 0b110000000011,
    FCVT_W_D = 0b110000100000,
    FCVT_WU_D = 0b110000100001,
    FCVT_L_D = 0b110000100010,
    FCVT_LU_D = 0b110000100011,
    FCVT_S_D = 0b010000000001,
    FCVT_S_W = 0b110100000000,
    FCVT_S_WU = 0b110100000001,
    FCVT_S_L = 0b110100000010,
    FCVT_S_LU = 0b110100000011,
    FCVT_D_S = 0b010000100000,
    FCVT_D_W = 0b110100100000,
    FCVT_D_WU = 0b110100100001,
    FCVT_D_L = 0b110100100010,
    FCVT_D_LU = 0b110100100011,
    FMV_W_X = 0b111100000000,
    FMV_D_X = 0b111100100000,
    FMV_FCLASS_S = 0b111000000000,
    FMV_FCLASS_D = 0b111000100000
};

/// \brief rm constants for FSGNJ floating-point instructions
enum insn_FSGN_funct3_000000000000 : uint32_t { J = 0b000000000000000, JN = 0b001000000000000, JX = 0b010000000000000 };

/// \brief rm constants for FMIN and FMAX floating-point instructions
enum insn_FMIN_FMAX_funct3_000000000000 : uint32_t {
    MIN = 0b000000000000000,
    MAX = 0b001000000000000,
};

/// \brief rm constants for FLE, FLT, and FEQ floating-point instructions
enum insn_FCMP_funct3_000000000000 : uint32_t {
    LE = 0b000000000000000,
    LT = 0b001000000000000,
    EQ = 0b010000000000000,
};

/// \brief funct7 constants for OR, REM instructions
enum insn_OR_REM_funct7 : uint32_t { OR = 0b0000000, REM = 0b0000001 };

/// \brief funct7 constants for AND, REMU instructions
enum insn_AND_REMU_funct7 : uint32_t { AND = 0b0000000, REMU = 0b0000001 };

/// \brief funct7 constants for ADDW, MULW, SUBW instructions
enum insn_ADDW_MULW_SUBW_funct7 : uint32_t { ADDW = 0b0000000, MULW = 0b0000001, SUBW = 0b0100000 };

/// \brief funct7 constants for SRLW, DIVUW, SRAW instructions
enum insn_SRLW_DIVUW_SRAW_funct7 : uint32_t { SRLW = 0b0000000, DIVUW = 0b0000001, SRAW = 0b0100000 };

/// \brief Privileged instructions, except for SFENCE.VMA, have no parameters
enum class insn_privileged : uint32_t {
    ECALL = 0b00000000000000000000000001110011,
    EBREAK = 0b00000000000100000000000001110011,
    SRET = 0b00010000001000000000000001110011,
    MRET = 0b00110000001000000000000001110011,
    WFI = 0b00010000010100000000000001110011
};

/// \brief funct2 constants for FMADD, FMSUB, FNMADD, FMNSUB instructions
enum insn_FM_funct2_0000000000000000000000000 : uint32_t {
    S = 0b000000000000000000000000000,
    D = 0b010000000000000000000000000
};

/// \brief rm constants for FMV and FCLASS instructions
enum insn_FMV_FCLASS_funct3_000000000000 : uint32_t { FMV = 0b000000000000000, FCLASS = 0b001000000000000 };

} // namespace cartesi

#endif
