---
layout: default
title: Core Registers
parent: IOP Device Quircks
---

# Core Registers

Range: `0xFFFE0000` - `0xFFFExxxx`

All IOP core registers are emulated in software.

Scratchpad mapping word 0 (`0xbe03f0`)  
Scratchpad mapping word 1 (`0xbe03f4`)  
Rom Ver emulation enable  (`0xbe082c`)  
Cache control (`0xbe03e8`)

## `0x130`:
if `0xc04` is written here the PPC data cache range `0xbde800` - `0xbe1000` gets
flushed to memory and then the entire data cache is invalided.

## `0x140`:
Store value in scratchpad mapping word 0 and update cache ctrl.

## `0x144`:
Store value in scratchpad mapping word 1 and update cache ctrl. 

## `0x184`:
if rom_ver_emu is enabled, store/load value in/from ptr @ `0xbe0830` 

## `0x180`:
set rom_ver_emu to value

## `0x188`: 
*TODO*
?? Dev9 related

## `0x18c`: 
*TODO*

?? Dev9 related

## `0x1a0`:
XPARAM index
Updates a global var with the current XPARAM index

## `0x1a4`: XPARAM value
Sets the **XPARAM** value with the current global index var

## `0x400`:
*TODO*

?? SPEED related