---
layout: default
title: Registers
parent: PowerPC APU
---

{: .note-title }
> Special Thanks
>
> Special thanks to Wisi. This document builds upon their early DCR exploration
> in the [dpLibMod project](https://www.psx-place.com/threads/hdd-for-ps2-scph75000x-later-models.30696/page-2#post-255399)
>


===============================================================================
The APU appears to contain two separate register files.

The first file contains MIPS HI, LO, GP, CC, PC, instruction decode registers, and more.
It's accessible directly through custom instructions and indirectly through 
DCRs 0x180 and 0x181.

The second file contains GTE registers, an instruction register, execution 
status register, and cycle count register. It's accessible directly through 
custom instructions and indirectly through DCRs 0x21 and 0x22. Indirect DCR
access requires a bit to be flipped in DCR 0x20. Writing any non-zero value
to DCR 0x20 causes it to flip between 0x49424d11 and 0x49424d10. DCR 0x20 must 
be 0x49424d10 before this register file can be accessed through DCR 0x21 and 0x22.

There are 3 APU registers mapped directly to DCRs.
DCR 0x182 = r/w MIPS Instruction Register
DCR 0x183 = r/o Unknown
				sw =   0xac410000 / 0xac0002  / A004 = 0x0
				lw =   0x8c410000 / 0x8c0004  / A004 = 0x10000
				mfc0 = 0x400d6800 / 0x2800400 / A004 = 0x10004
				mtc0 = 0x40826000 / 0x2900400 / A004 = 0x10004
				jal =  0x0040f809 / 0x1240801 / A004 = 0x10008
				and =  0x00641824 / 0x1900008 / A004 = 0x0
DCR 0x184 = r/o Addr of operand

=============================================================================
Register File 1 - MIPS HI, LO, GP, CC, PC, and more 

Range: 0x0-0x7F

Indirect DCR access:
DCR-0x180 = register number
DCR-0x181 = register value

------------APU Internal / Decode Related------------
0x0	= r/o unk, C11E44A1 no observed change.
0x1	= r/o unk, F00644A1 no observed change.
0x2	= r/o unk, 00000000 no observed change.
0x3	= r/w unk, 00010003 no observed change. Can be cleared by writing 0x0

0x4	= r/w, status register?
	The act of reading A000 causes it to be set to 0x10000 depending on 
	what I_MASK is currently set to?

	Setting I_STAT 0x1 or 0x800 causes A004 to be set to 0x10000 
	as well.

	Bit 16 is for interrupts?

	Lower 4 bits indicate instruction type?

	0x4 for OTHER/CO
	0x2 for store(?)
	0x8 for jr, jal, beqz, 
	0x0 for everything else
	some stores cause it to be set to 0x20?

	0x000103FF, lower 10 bits and bit 16 = r/w

0x5	= r/o unk
	Appears to be set to 0x1 on execution of *some* but not all load instructions
	Possibly cleared on subsequent non load instruction? Unconfirmed.

0x6	= r/o word aligned opcode for use with function LUT
	For instructions with non-zero primary opcode:
	value = primary opcode * 4

	For instructions with primary opcode zero:
	value = secondary opcode * 4 + 0x100

	Primary:
		0x0   = special
		0x4	  = bcondz
		0x8	  = j
		0xC   = jal
		0x10  = beq
		0x14  = bne
		0x18  = blez
		0x1C  = bgtz
		0x20  = addi
		0x24  = addiu
		0x28  = slti
		0x2C  = sltiu
		0x30  = andi
		0x34  = ori
		0x38  = xori
		0x3C  = lui
		0x40  = COP0
		0x44  = COP1
		0x48  = COP2
		0x4C  = COP3

		0x50 - 0x7C = N/A
		
		0x80 = lb
		0x84 = lh
		0x88 = lwl
		0x8C = lw
		0x90 = lbu
		0x94 = lhu
		0x98 = lwr

		0x9C = N/A

		0xA0 = sb
		0xA4 = sh
		0xA8 = swl
		0xAC = sw

		0xB0-0xB4 = N/A

		0xB8 = swr

		0xBC = N/A
		
		0xC0 = LWC0
		0xC4 = LWC1
		0xC8 = LWC2
		0xCC = LWC3

		0xD0 - 0xDC = N/A

		0xE0 = SWC0
		0xE4 = SWC1
		0xE8 = SWC2
		0xEC = SWC3

		0xF0 - 0xFC = N/A

	Secondary:

		0x100 = sll

		0x104 = N/A

		0x108 = srl
		0x10C = sra
		0x110 = sllv

		0x114 = N/A

		0x118 = srlv
		0x11C = srav
		0x120 = jr	
		0x124 = jalr

		0x128 = N/A
		0x12C = N/A

		0x130 = syscall
		0x134 = break

		0x138 = N/A
		0x13C = N/A

		0x140 = mfhi
		0x144 = mthi
		0x148 = mflo
		0x14C = mtlo

		0x150 - 0x15C = N/A

		0x160 = mult
		0x164 = multu
		0x168 = div
		0x16C = divu

		0x170 - 0x17C = N/A

		0x180 = add
		0x184 = addu
		0x188 = sub
		0x18C = subu
		0x190 = and
		0x194 = or
		0x198 = xor
		0x19C = nor

		0x1A0 - 0x1A4 = N/A

		0x1A8 = slt
		0x1AC = sltu

		0x1B0 - 0x1FC = N/A

	COP:
		0x280 = mfc0
		0x290 = mtc0
		0x2C0 = rfe
		
		value range 0x0 - 0x3FC

I FORMAT:
0x7 - N/A   (instruction >> 11 & 0x1F) 0x0 - 0x1F
0x8 = opcode (not shifted)    0x0 - 0x3F
0x9 = RS    (instruction >> 21 & 0x1F) 0x0 - 0x1F
0xA = RT    (instruction >> 16 & 0x1F) 0x0 - 0x1F
0xB = N/A   (instruction >> 11 & 0x1F) 0x0 - 0x1F
0xC = IMM   (instruction & 0xFFFF)     0x0 - 0xFFFF
0xD = N/A   (instruction & 0x03FFFFFF) 0x0 - 0x03FFFFFF
0xE = N/A   (instruction & 0x3F)       0x0 - 0x3F
0xF = N/A   (instruction >> 6 & 0x1F)  0x0 - 0x1F

R FORMAT:
0x7 - RD    (instruction >> 11 & 0x1F) 0x0 - 0x1F
0x8 = opcode (not shifted) 0x0 - 0x3F
0x9 = RS    (instruction >> 21 & 0x1F) 0x0 - 0x1F
0xA = RT    (instruction >> 16 & 0x1F) 0x0 - 0x1F
0xB = RD    (instruction >> 11 & 0x1F) 0x0 - 0x1F
0xC = N/A   (instruction & 0xFFFF)     0x0 - 0xFFFF
0xD = N/A   (instruction & 0x03FFFFFF) 0x0 - 0x03FFFFFF
0xE = FUNCT (instruction & 0x3F)       0x0 - 0x3F
0xF = SHIFT (instruction >> 6 & 0x1F)  0x0 - 0x1F

J FORMAT:
0x7 - RD    (instruction >> 11 & 0x1F) 0x0 - 0x1F
0x8 = opcode (not shifted) 0x0 - 0x3F
0x9 = RS    (instruction >> 21 & 0x1F) 0x0 - 0x1F
0xA = RT    (instruction >> 16 & 0x1F) 0x0 - 0x1F
0xB = RD    (instruction >> 11 & 0x1F) 0x0 - 0x1F
0xC = N/A   (instruction & 0xFFFF)     0x0 - 0xFFFF
0xD = ADDR  (instruction & 0x03FFFFFF) 0x0 - 0x03FFFFFF
0xE = N/A   (instruction & 0x3F)       0x0 - 0x3F
0xF = N/A   (instruction >> 6 & 0x1F)  0x0 - 0x1F

0x10	= r/w unk, 05010C23 no observed change.	0x0-0xFFFFFFFF
0x11	= r/o unk, 00000000 no observed change.
0x12	= r/o unk, 00000000 no observed change.

0x13	= r/w MIPS cycle counter, set by software, decrementing.
		Used to control how many cycles the APU will run for. 
		Load instructions decrement counter by 5. When zero is 
		reached the next instruction given to the APU will not 
		be decoded and instruction type flags will not be set 
		causing the software to break out of the execution loop. 
		If in a delay slot instruction, it will proceed with correct
		decode until a non delay slot instruction is reached 
		and then break out.

0x14	= r/w unk, 1F000000 no observed change. 0x0-1FFFFC00
0x15	= r/o unk, 00000000 no observed change.
0x16	= r/o unk, 00000000 no observed change.
0x17	= r/o unk, 00000000 no observed change.

------------MIPS Registers------------
0x18	= r/w MIPS HI, 0x0-0xFFFFFFFF
0x19	= r/w MIPS LO, 0x0-0xFFFFFFFF

0x1A	= r/w MIPS PC

0x1B	= r/w COP0 DCIC
0x1C	= r/w COP0 BPC
0x1D	= r/w COP0 BPCM
0x1E	= r/w COP0 BDA
0x1F	= r/w COP0 BDAM

Note: The other COP0 registers are emulated in software

0x20 - 0x3F = r/w  MIPS GP Registers

------------Unknown------------
0x3F - 0x7F = r/w Unused? Hold misc values that remain mostly constant.

=============================================================================
Register File 2 - GTE Registers

Range: 0x0-0xFF

Indirect DCR access:
DCR-0x21 = register number
DCR-0x22 = register value

0x0-0x63 = Normal GTE registers

0x64-0x7F = Unknown.

0x80 = GTE instruction register
		Instructions are copied to this register using custom instructions
		from PPC. Once copied, execution begins immediately.

0x81 = Status register
		Bits 0:29 Unknown
		Bits 30:31 Busy status. These bits are polled by software after placing
		an instruction in register 0x80.

0x82 = Cycle count of last GTE instruction to run
		This value is read after completing a GTE instruction 
		and subtracted from the current MIPS cycle count. This suggests
		the GTE may run at the same speed as the PPC core (440MHz).

0x83-0xFF = Unknown.