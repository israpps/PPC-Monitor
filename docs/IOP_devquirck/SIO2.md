---
layout: default
title: SIO2 Interface
parent: IOP Device Quircks
---

All SIO2 register data is also mirrored to a struct in mem. This is used for patching certain parameters.

- SIO2 transfer queue register struct: ``0xbe07e4`` - `0xbe0824`

pCtrl0 struct: `0xbe07c4` - `0xbe07d4`
pCtrl1 struct: `0xbe07d4` - `0xbe07e4`

pCtrl0_2 and pCtrl0_3 have their *PCTRL0_ATT_MIN_HIGH_PER* patched to `0x60` 
regardless of what value is set.

pCtrl1_2 and pCtrl1_3 have their *PCTRL1_INTER_BYTE_PER* patched to `0x2` if it's 
greater than `0x4` AND the active baud div is `0x2`. This might be to compensate for
the longer inter byte delay in the SIO2 silicon on DECKARD models. (~370ns 75K+ vs ~160ns fats)

When reading from SIO2 registers all data will be read from the registers directly EXCEPT for 
pCtrl0_2, pCtrl0_3, pCtrl1_2, and pCtrl1_3. These will be read from memory.

Writing `0x0` or `0x1` to the baud div register directly from the PPC side does 
not stick...It will always default to `0x2`