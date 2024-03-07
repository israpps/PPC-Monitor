---
layout: default
title: Unknown device
parent: PowerPC miscelaneous stuff
---

{: .highlight }
Unknown device found while probing DCRs, it has not been explored.

Indirect DCR access:
- DCR-0x16: register number
- DCR-0x17: register value

Registers 0x0 - 0x3F

```md
=============================================================================
0x0: 
    default: 0x0
    function: possibly a status register?
    r/w
    
    Reg 0x3 seems to be a decrementing timer and when it reaches zero this is 
    set to 0x20000. Bit 17 is set (LE)

=============================================================================
0x01:
    default: 0x0
    r/o?

=============================================================================
0x02:
    default: 0x0
    r/w
    writing 0x2 resets timer in 03
    writing 0x4 starts timer in 03

    setting bit 2 of this register causes 0x03 to begin decrementing 
    from 0xFFFFFFFF to 0; on completion register 0x0 is set to 0x20000

    seems to accept values 0x0-0xFFFFFFFF

=============================================================================
0x03:
    default: 0xffffffff
    r/w

    decrementing timer. 

    Seems to go from 0xFFFFFFFF -> 0x0 in 0x3000000A6
    PPC ticks.

    Sets bit 17 of 0x00 when 0x0 is reached.

    Accepts values 0x0-0xFFFFFFFF

=============================================================================
0x04:
    r/w
    default: 0x0
    0x0-0xffffffff

=============================================================================
0x05:
    r/w
    default: 0x0
    0x0-0xffffffff

=============================================================================
0x06:
    r/w
    default: 0xffffffff
    0x0-0xffffffff

=============================================================================
0x07:
    r/w
    default: 0xffffffff
    0x0-0xffffffff

=============================================================================
0x08:
    r/o
    default: 0xc27e3411

=============================================================================
0x09:
    r/w
    default: 0x0
    0x0 - 0xeffff000
    lower 12 bits cannot be set as well as upper bit 3

=============================================================================
0x0a:
    r/w
    default: 0x0
    0x0 - 0xeffff000
    lower 12 bits cannot be set as well as upper bit 3

=============================================================================
0x0b:
    r/w
    default: 0x0
    0x0 - 0xeffff000
    lower 12 bits cannot be set as well as upper bit 3

=============================================================================
0x0c:
    r/w
    default: 0x0
    0x0 - 0xeffff000
    lower 12 bits cannot be set as well as upper bit 3

=============================================================================
0x0d:
    r/o
    default: 0x0

=============================================================================
0x0e:
    r/o
    default: 0x0

=============================================================================
0x0f:
    r/o
    default: 0x0

=============================================================================
0x10:
    r/o
    default: 0x0

=============================================================================
0x11:
    r/w
    default: 0x0
    0x0 - 0xefff8000
    lower 15 bits cant be set

=============================================================================
0x12:
    r/w
    default: 0x0
    0x0 - 0xefff8000
    lower 15 bits cant be set

=============================================================================
0x13:
    r/w
    default: 0x0
    0x0 - 0xefff8000
    lower 15 bits cant be set

=============================================================================
0x14:
    r/w
    default: 0x0
    0x0 - 0xefff8000
    lower 15 bits cant be set

=============================================================================
0x15:
    r/o
    default: 0x0

=============================================================================
0x16:
    r/o
    default: 0x0

=============================================================================
0x17:
    r/o
    default: 0x0

=============================================================================
0x18:
    r/o
    default: 0x0

=============================================================================
0x19:
    r/w
    default: 0x0
    only upper 4 bits can be set

=============================================================================
0x1a:
    r/w
    default: 0x0
    only upper 4 bits can be set

=============================================================================
0x1b:
    r/w
    default: 0x0
    only upper 4 bits can be set

=============================================================================
0x1c:
    r/w
    default: 0x0
    only upper 4 bits can be set

=============================================================================
0x1d:
    r/w
    default: 0x0
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    
    0x0 - 0x00ffffff
    //0xfffff000 = 0x00fff000
    //0x8ffffc01 = 0x0

=============================================================================
0x1e:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x1f:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x20:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x21:
    r/o
    default: 0x0

=============================================================================
0x22:
    r/o
    default: 0x0

=============================================================================
0x23:
    r/o
    default: 0x0

=============================================================================
0x24:
    r/o
    default: 0x0

=============================================================================
0x25:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x26:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x27:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x28:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x29:
    r/o
    default: 0x0

=============================================================================
0x2a:
    r/o
    default: 0x0

=============================================================================
0x2b:
    r/o
    default: 0x0

=============================================================================
0x2c:
    r/o
    default: 0x0

=============================================================================
0x2d:
    r/w
    default: 0x0
    0x0 - 0x00ffffff
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x2e:
    r/w
    default: 0x0
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x2f:
    r/w
    default: 0x0
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x30:
    r/w
    default: 0x0
    upper 8 bits cannot be set.
    some bits can not be set simultaneously
    0xfffff000 = 0x00fff000

=============================================================================
0x31:
    r/w
    default: 0x0
    0x9F000000
    first byte can only be set and not bits 1-2
    0xFFFFF000 = 0x9f000000

=============================================================================
0x32:
    r/w 
    default: 0x0
    0x9F000000
    first byte can only be set and not bits 1-2
     = 0x9f000000

=============================================================================
0x33:
    r/o
    default: 0x0

=============================================================================
0x34:
    r/o
    default: 0x0

=============================================================================
0x35:
    r/o
    default: 0x00ffffff

=============================================================================
0x36:
    r/o
    default: 0x00ffffff

=============================================================================
0x37:
    r/o
    default: 0x0

=============================================================================
0x38:
    r/o
    default: 0x0

=============================================================================
0x39:
    r/o
    default: 0x0

=============================================================================
0x3a:
    r/o
    default: 0x0

=============================================================================
0x3b:
    r/o
    default: 0x0

=============================================================================
0x3c:
    r/o
    default: 0x0

=============================================================================
0x3d:
    r/o
    default: 0x0

=============================================================================
0x3e:
    r/o
    default: 0x0

=============================================================================
0x3f:
    r/o
    default: 0x0
=============================================================================
```