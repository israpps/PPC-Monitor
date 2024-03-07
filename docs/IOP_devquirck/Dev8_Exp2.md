---
layout: default
title: DEV8
parent: IOP Device Quircks
---
# DEV8

Range: `0x1F803800` - `0x1F803850`

Writes to `0x1F80380c` will also be passed to the PowerPC UART TX.

Everything else is written and read directly.