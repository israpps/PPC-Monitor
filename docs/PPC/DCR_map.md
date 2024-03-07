---
layout: default
title: DCR Map
parent: PowerPC miscelaneous stuff
---

{: .note-title }
> Special Thanks
>
> Special thanks to Wisi. This document builds upon their early DCR exploration
> in the dpLibMod project:
> https://www.psx-place.com/threads/hdd-for-ps2-scph75000x-later-models.30696/page-2#post-255399

```md
=============================================================================
DCR 0x0 - 0xB = r/o, unk.
    default: 0x0

=============================================================================
DCR 0xC = r/o, unk.
    default: 0xf0000000

=============================================================================
DCR 0xD = r/w, unk. Possibly PPC debug/performance register?
    default: 0x4833c000
    
    Values are often 0x48xxxxx which decodes to a valid PPC
    relative branch instruction.

=============================================================================
DCR 0xE - 0xF = r/o, unk.
    default: 0x0

=============================================================================
DCR 0x10 =  r/w, index for indirect access to SDRAM controller registers
DCR 0x11 =  r/w, value for indirect access to SDRAM controller registers

=============================================================================
DCR 0x12 - 0x15 = r/o, unk.
    default: 0x0

=============================================================================
DCR 0x16 =  r/w, index for indirect access to unknown device
DCR 0x17 =  r/w, value for indirect access to unknown device

=============================================================================
DCR 0x18 - 0x1F = r/o, unk.
    default: 0x0

=============================================================================
DCR 0x20 =  r/w, unk.

Wisi:
    0x49 = I
    0x42 = B
    0x4D = M
    0x10 / 0x11  dataLinkEscape / deviceControl1  - more likely some flag
    it means "IBM" and some control or status char

Writing any non-zero value swaps between 0x49424d11 and 0x49424d10.

APU GTE Register file is only accessible through DCR 0x21 and 0x22 when
this is == 0x49424d10.

=============================================================================
DCR 0x21 =  r/w, index for indirect access to APU GTE register file
DCR 0x22 =  r/w, value for indirect access to APU GTE register file

=============================================================================
DCR 0x23 - 0x2F = r/o, unk.
    default: 0x0

=============================================================================
DCR 0x30 =  r/w, unk
    default: 0x0
    range: 0x0 - 0xffffffff

=============================================================================
DCR 0x31 =  r/w, unk
    default: 0x0c000000
    0:3   r/o
    4:9   r/w
    10:31 r/o

=============================================================================
DCR 0x32 =  r/w, unk
    default: 0x0
    0    r/w
    1:31 r/o

=============================================================================
DCR 0x33 =  r/w, Reset Line
    default: 0xc0000000
    1 = inactive
    0 = active

    0:   r/w, ps2 mode reset
    1:   r/w, ps1 mode reset
    2:31 r/o 

    Used to reset IOP peripherals into desired mode

=============================================================================
DCR 0x34 =  r/w, unk
    0:   r/w
    1:31 r/o

=============================================================================
DCR 0x35 =  r/w, Serial Clock Control Register
    default: 0x80000050
    0:     clk enable(?)
    1:23:  reserved
    24:31: divisor

    UART Divisor = Serial clock / (16 x baud)
    Serial Clock * Divisor = PLB Clock

    Note: PLB clock supports CPU:PLB frequency ratios N:1, N:2, N:3 

    With mem mapped UART peripheral configured for 57600 baud:
    0x80000004 = 0.88uS
    0x80000005 = 1.080uS
    0x80000008 = 1.7uS
    0x80000010 = 3.40uS
    0x80000020 = 7uS
    0x80000030 = 10.40uS
    0x80000032 = 10.80uS
    0x80000033 = 11.20uS
    0x80000040 = 13.90uS
    0x80000050 = 17.40uS

    1.839MHz  * 0x50(80) = 147.12       147.12*3  = 441.36
    3.07MHz   * 0x30(48) = 147.36       147.36*3  = 442.08
    4.571 MHz * 0x20(32) = 146.272      146.272*3 = 438.816

    PPC 440 Clock = 440MHz

=============================================================================
DCR 0x36 =  r/o, unk
    default: 0x05262049

=============================================================================
DCR 0x37 =  r/w, Power management related?
    default: 0x0
    
    Each bit corresponds to a device, when set it appears to disable the
    corresponding device.
    
    0x0 = all enabled
    0x1 = INTC disabled
    0x2 = APU disabled
    0x4 = DMA controller, DMAC2, DMAC3 disabled
    0x8 = SPU 1? 2? disabled?
            0x1F803800 = 0x0 instead of 0xFFFFFFF
            0x1F801810 = 0x0 instead of 0x2660000
    
    0x20 = Unknown device (DCR 0x16-0x17) disabled
    0x80 = IOP SBUS inaccessible 0x1F801450
    0x100 = SIO2. If this is set mid SIO2 transfer it abruptly stops mid byte
    
    0x800 = DCR-0xA8 disabled. Possibly more registers associated with this.

    0x2000 = USB OHCI disabled
    
    0x4000 = IOP timer registers become inaccessible.
    0x8000 = DCR-0xC0 - 0xC8 disabled.

=============================================================================
DCR 0x38 = r/w, unk
    default: 0x0eaaea00
    0:3   = r
    4:23  = w
    24:29 = r
    30    = w
    31    = r

=============================================================================
DCR 0x39 = r/w, unk
    default: 0x0
    0x0 - 0xffffff00

=============================================================================
DCR 0x3a = r/w, unk
    default: 0x60000000
    only bits 1:2 can be set

=============================================================================
DCR 0x3B - 0x81 = r/o, unk.
    default: 0x0

=============================================================================
Very similar to PLB arbiter registers found in AMCC PPC 4xx SoCs

DCR 0x82 = r/o, REVID?
    default: 0x00000101

DCR 0x83 = r/w, unk, 
    default: 0xbb000000 <- Changes
    0:7  r/w
    8:31 r/o

DCR 0x84 = r/w, unk, status bits, Clear on write
    default: 0x0

DCR 0x85 = r/w, unk, status bits, Set on write
    default: 0x0

DCR 0x86 = r/o, unk, error address low
    default: 0xff7fffff

DCR 0x87 = r/o, unk, error address high
    default: 0xfbfffffe

=============================================================================
DCR 0x88 - 0x8F = r/o, unk.
    default: 0x0

=============================================================================
Very similar to PLB to OPB registers found in AMCC PPC 4xx SoCs

DCR 0x90 = r/w, status clear on write
    default: 0x0

DCR 0x91 = r/w, status set on write
    default: 0x0

DCR 0x92 = r/o, error address low
    default: 0x0

DCR 0x93 = r/o, error address high
    default: 0x0

DCR 0x94 = r/w, status 1 clear on write
    default: 0x0

DCR 0x95 = r/w, status 2 set on write
    default: 0x0

DCR 0x96 = r/w, config clear on write
    default: 0x0

DCR 0x97 = r/w, config set on write
    default: 0x0

DCR 0x98 = r/w, latency clear on write
    default: 0x78000000

DCR 0x99 = r/w, latency set on write
    default: 0x78000000

DCR 0x9A = r/o, rev id
    default: 0x122

=============================================================================
DCR 0x9B - 0xA7 = r/o, unk.
    default: 0x0

=============================================================================
DCR 0xA8 = r/w, unk
    default: 0xc0000000

    Set very early in PPC init.

    Possibly PPC core clock control?
    Seems to crash when changed. TODO: investigate further

=============================================================================
DCR 0xAC = r/o, unk revid?
    default: 0x130

=============================================================================
DCR 0xB0 = r/w, unk
    default: 0x0
    0x80 = seems to disable access to DCR 0xC0 - 0xC8

DCR 0xB1 = r/o, unk
    default: 0x0

DCR 0xB2 = r/o, unk status?
    default: 0x0
    
    Appears to be equal to 0xb0 | 0xb1

    there are a few exceptions to this.
    b0 in: 0x80000, b2 out: 0x0
    b0 in: 0x1000000, b2 out: 0x0
    b0 in: 0x2000000, b2 out: 0x0

    b1 in: 0x80000 = freeze
    b1 in: 0x1000000, b2 out: 0x1000000
    b1 in: 0x2000000 = freeze

=============================================================================
DCR 0x180 =  r/w, index for indirect access to APU register file 1
DCR 0x181 =  r/w, value for indirect access to APU register file 1

=============================================================================
DCR 0x182 = r/w, APU MIPS instruction register

DCR 0x183 = r/o, unk.

DCR 0x184 = r/o, resolved addr of operand

=============================================================================
DCR 0x185 - 0x3FF = r/o, unk.
    default: 0x0
```