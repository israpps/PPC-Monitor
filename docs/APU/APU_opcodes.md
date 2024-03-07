---
layout: default
title: Opcodes
parent: PowerPC APU
---

> Special Thanks
>
> Special thanks to Wisi. This document builds upon their early DCR exploration
> in the [dpLibMod project](https://www.psx-place.com/threads/hdd-for-ps2-scph75000x-later-models.30696/page-2#post-255399)
>
{: .note-title }

```md
=============================================================================
The APU instruction format resembles the PPC X / XO instruction format

  Primary OP       Rx          Ry         Rz          Secondary OP    
[0 0 0 0 0 0] [0 0 0 0 0] [0 0 0 0 0] [0 0 0 0 0] [0 0 0 0 0 0 0 0 0 0 0]             
    31-25        25-20        20-15      15-10           10-0

The primary opcode is always 0x0 [000000]
The secondary or extended opcode always contains 0xC [1100]
Register fields vary between instructions. 

=============================================================================
Possible secondary opcode encoding scheme:

bits 0-3 =
    0xC, constant

bit 4 =
    1 = move from APU
    0 = move to APU

bit 5 = reserved always 0

bit 6-10 =
    register number or unique number of some kind?
    0x0 = exec
    0x1 = exec 
    0x2 = exec
    0x3-0x7 = ? 
    0x8 = A01A PC
    0x9 = A013 Cycle Count
    0xA = A018 HI
    0xB = A019 LO
    0xC = ?
    0xE = A020 - A03F GP registers
    0xF = GTE registers
    
    0x10 = A004 status reg
    0x11 = A005 unk 
    0x12 = A006 opcode << 2
    0x13 = A007 RD

    0x14 = A004 status reg
    0x15 = A005 unk
    0x16 = A006 opcode << 2
    0x17 = A007 RD
    0x18 = A008 opcode not shifted
    
    0x19 = A009 RS
    0x1A = A00A RT
    0x1B = A00B RD
    0x1C = A00C IMM
    0x1D = A00D ADDR
    0x1E = A00E FUNCT
    0x1F = A00F SHIFT

=============================================================================

Observed opcodes sorted by secondary opcode number:

=============================================================================
OPCODE: 0xC  -"aexec" - Execute MIPS instruction (normal)
Rx: [OUTPUT] = Address referenced in MIPS operand (if applicable)
Ry: [INPUT]  = MIPS instruction
Rz: [N/A]

This executes / decodes the MIPS instruction currently in Ry, and outputs the
address referenced by the operand (if applicable) to Rx. It also sets condition
register field 6 (CR6) to signify what type of instruction it is, arithmetic/logical,
load, store, branch, or other.

If there are pending interrupts or if the cycle counter has reached zero, 
execution will fail. That is CR6 bits will be set to 0, the cycle counter will
not decrement, and the PC will not increment.

CR6 values:
N/A       0= other / move from/to coprocessor
SO   0,   1= branch
ZEQ  1,   2= store
GT   2,   4= load
LT   3,   8= arithmetic/logical

=============================================================================
OPCODE: 0x4C -"aexeci" - Execute MIPS instruction (ignore interrupts)
Rx: [OUTPUT] = Address referenced in MIPS operand (if applicable)
Ry: [INPUT]  = MIPS instruction
Rz: [N/A]

This executes / decodes the MIPS instruction currently in Ry, and outputs the
address referenced by the operand (if applicable) to Rx. It also sets condition
register field 6 (CR6) to signify what type of instruction it is, arithmetic/logical,
load, store, branch, or other.

This instruction is used in delay slots to ensure their execution.

If there are pending interrupts or if the cycle counter has reached zero,
execution will still succeed. CR6 bits will be set, the cycle counter will
decrement, and the PC will increment.

CR6 values:
N/A     0= other / move from/to coprocessor
SO      1= branch
ZEQ     2= store
GT      4= load
LT      8= arithmetic/logical

=============================================================================
OPCODE: 0x8c -"aexecn" - Execute MIPS instruction (no PC increment)
Rx: [OUTPUT] = Address referenced in MIPS operand (if applicable)
Ry: [INPUT]  = MIPS instruction
Rz: [N/A]

This is the same as aexec however it does not increment the PC upon successful execution.

=============================================================================
OPCODE: 0x20c -"amtpc" - Set MIPS PC
Rx: [N/A]
Ry: [INPUT] = PC value to set
Rz: [N/A]

Sets the current MIPS PC to the value stored in Ry.

=============================================================================
OPCODE: 0x21c -"amfpc" - Get MIPS PC
Rx: [OUTPUT] = MIPS PC
Ry: [N/A]
Rz: [N/A]

Gets the current MIPS PC and stores it in Rx.

=============================================================================
OPCODE: 0x24c -"amtcc" - Set MIPS cycle count
Rx: [N/A]
Ry: [INPUT] = Cycle count value to set
Rz: [N/A]

Sets the current MIPS cycle count to the value stored in Ry.

=============================================================================
OPCODE: 0x25c -"amfcc" - Get MIPS cycle count 
Rx: [OUTPUT] = MIPS cycle count
Ry: [N/A]
Rz: [N/A]

Gets the current MIPS cycle count and stores it in Rx.

=============================================================================
OPCODE: 0x28c -"amthi" - Set MIPS HI register
Rx: [N/A]
Ry: [INPUT] = Register that contains the value to be set  
Rz: [N/A]

Sets the MIPS HI register to the value at Ry.

=============================================================================
OPCODE: 0x29c -"amfhi" - Get MIPS HI register
Rx: [OUTPUT]  = MIPS HI value
Ry: [N/A]
Rz: [N/A]

Gets the value of MIPS HI register and stores it in Rx.

=============================================================================
OPCODE: 0x2cc -"amtlo" - Set MIPS LO register
Rx: [N/A]
Ry: [INPUT] = Register that contains the value to be set  
Rz: [N/A]

Sets the MIPS lo register to the value at Ry.

=============================================================================
OPCODE: 0x2dc -"amflo" - Get MIPS LO register
Rx: [OUTPUT]  = MIPS LO value
Ry: [N/A]
Rz: [N/A]

Gets the value of MIPS LO register and stores it in Rx.

=============================================================================
OPCODE: 0x30c -"aunk0" - Unknown
Rx: [N/A]
Ry: [N/A]
Rz: [N/A]

TODO: Unknown.

=============================================================================
OPCODE: 0x38c -"amtgp" - Set MIPS GP register
Rx: [N/A]
Ry: [INPUT] = Register containing number of the MIPS GP register to set 
Rz: [INPUT] = Register containing value to be set

Sets the MIPS GP register specified by the value at Ry to the value stored at Rz. 

=============================================================================
OPCODE: 0x39c -"amfgp" - Get MIPS GP register
Rx: [OUTPUT] = Value at MIPS register
Ry: [INPUT]  = Register containing number of MIPS GP register to get 
Rz: [N/A]

Gets the value of the MIPS GP register specified by the value in Ry and stores
it in Rx.

=============================================================================
OPCODE: 0x3cc -"amtgte" - Set GTE register        
Rx: [N/A]
Ry: [INPUT] = Register containing number of the GTE register to set 
Rz: [INPUT] = Register containing value to be set

Sets the GTE register specified by the value at Ry to the value stored at Rz. 

=============================================================================
OPCODE: 0x3dc -"amfgte" - Get GTE register
Rx: [OUTPUT] = Value at GTE register
Ry: [INPUT]  = Register containing number of GTE register to get 
Rz: [N/A]

Gets the value at the GTE register specified by the value at Ry and stores it
in Rx.

This is used to access the GTE register file which contains more than 
just standard GTE registers. The full range is 0x0-0xFF (see APU_registers.txt).

=============================================================================
OPCODE: 0x41c -"amf04" - Get APU register 0x4 (Reg file 1)
Rx: [OUTPUT] = APU Status
Ry: [N/A]  
Rz: [N/A]

Register 0x4 is a status register of some kind (see APU_registers.txt).

It also appears to contain instruction type which is used before handling interrupts
mtc0, mfc0, mflo, mfhi = 0x4
sw = 0x2

TODO: verify

=============================================================================
OPCODE: 0x45c -"amf05" - Get APU register 0x5 (Reg file 1)
Rx: [OUTPUT] = APU Register 0x5
Ry: [N/A]  
Rz: [N/A]

Currently unknown what register 0x5 is used for. 

=============================================================================
OPCODE: 0x49c -"amfop" - Get opcode (shifted) APU Register 0x6 (Reg file 1)
Rx: [OUTPUT] = Opcode of current instruction, shifted
Ry: [N/A]  
Rz: [N/A]

The exact value returned depends on whether or not the primary opcode is zero.

For instructions with non-zero primary opcode:
output = primary opcode * 4

For instructions with primary opcode zero:
output = secondary opcode * 4 + 0x100

TODO: There are a few more special cases

=============================================================================
OPCODE: 0x4dc -"amfrd" - Get RD field
Rx: [OUTPUT] = RD field of current MIPS instruction
Ry: [N/A]
Rz: [N/A]

Gets the RD field of the current MIPS instruction and stores it in Rx
If the instruction is JAL_x, it returns 0x1F.

=============================================================================
OPCODE: 0x51c -"amf04_u" - Get APU register 0x4 (reg file 1) / Duplicate of 0x41c
Rx: [OUTPUT] = APU register 0x4
Ry: [N/A]
Rz: [N/A]

=============================================================================
OPCODE: 0x55c -"amf05_u" - Get APU register 0x5 (reg file 1) / Duplicate of 0x45c
Rx: [OUTPUT] = APU register 0x5
Ry: [N/A]
Rz: [N/A]

=============================================================================
OPCODE: 0x59c -"amf06_u" - Get APU register 0x6 (reg file 1) / Duplicate of 0x49c
Rx: [OUTPUT] = APU register 0x6
Ry: [N/A]
Rz: [N/A]

=============================================================================
OPCODE: 0x5dc -"amf07_u" - Get APU register 0x7 (reg file 1) / Duplicate of 0x4dc
Rx: [OUTPUT] = APU register 0x7
Ry: [N/A]
Rz: [N/A]

=============================================================================
OPCODE: 0x61c -"amf08_u" - Get Opcode (not shifted)
Rx: [OUTPUT] = Opcode of current instruction, not shifted
Ry: [N/A]
Rz: [N/A]

Gets the opcode of the current MIPS instruction and stores it in Rx

=============================================================================
OPCODE: 0x65c -"amf09_u" - Get RS field
Rx: [OUTPUT] = RS field of current MIPS instruction
Ry: [N/A]
Rz: [N/A]

Gets the RS field of the current MIPS instruction and stores it in Rx

=============================================================================
OPCODE: 0x69c -"amf0A_u" - Get RT field
Rx: [OUTPUT] = RT field of current MIPS instruction
Ry: [N/A]
Rz: [N/A]

Gets the RT field of the current MIPS instruction and stores it in Rx

=============================================================================
OPCODE: 0x6dc -"amf0B_u" - Get RD field
Rx: [OUTPUT] = RD field of current MIPS instruction
Ry: [N/A] 
Rz: [N/A]

Gets the RD field of the current MIPS instruction and stores it in Rx

=============================================================================
OPCODE: 0x71c -"amf0C_u" - Get IMM field
Rx: [OUTPUT] = IMM field of current MIPS instruction
Ry: [N/A] 
Rz: [N/A]

Gets the IMM field of the current MIPS instruction and stores it in Rx

=============================================================================
OPCODE: 0x75c -"amf0D_u" - Get ADDR field
Rx: [OUTPUT] = ADDR field of current MIPS instruction
Ry: [N/A] 
Rz: [N/A]

Gets the ADDR field of the current MIPS instruction and stores it in Rx

=============================================================================
OPCODE: 0x79c -"amf0E_u" - Get FUNCT field
Rx: [OUTPUT] = FUNCT field of current MIPS instruction
Ry: [N/A] 
Rz: [N/A]

Gets the FUNCT field of the current MIPS instruction and stores it in Rx

=============================================================================
OPCODE: 0x7dc -"amf0F_u" - Get SHIFT field
Rx: [OUTPUT] = SHIFT field of current MIPS instruction
Ry: [N/A] 
Rz: [N/A]

Gets the SHIFT field of the current MIPS instruction and stores it in Rx

=============================================================================
```