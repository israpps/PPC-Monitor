---
layout: default
title: PowerPC UART
nav_order: 99
---

# PPC-IOP UART

The PPC-IOP UART RX and TX are accessible through an unpopulated connector footprint located on the B side (underside) of the motherboard (together with the EE UART and some other signals). This connector footprint seems to be consistent across all 75K and later models. The PPC-IOP UART signals use 3.5V logic levels and the baud rate is 57600 bps. When powered on or reset the following message is sent:

```
D E C K A R D compiled $date
```

## UART location on `SCPH-75xxx` (`GH-040-??`):

![nox_75K](https://github.com/israpps/PPC-Monitor/blob/gh_pages/pictures/GH-040_nox_UART-DIAGRAM.jpg?raw=true)
> Photo credit: Nox

## UART location on `SCPH-75xxx` (`GH-037-12`):
![el_isra_75k](https://github.com/israpps/PPC-Monitor/blob/gh_pages/pictures/GH-037-12_isra_UART-DIAGRAM.png?raw=true)

> Photo credit: [El Isra](https://github.com/israpps)

## UART location on late `SCPH-90xxx` (`GH-071`/`GH-072`)
![late_90k_tschicki](https://github.com/israpps/PPC-Monitor/blob/gh_pages/pictures/late_90k.png?raw=true)

> Photo creadit tsichiki

## Using the UART from an application

The PowerPC UART TX/RX registers are accessible from both EE and IOP.

The TX reg corresponds to the address `0x1F80380C`

On latest PS2SDK, you already have available an IRX module that will redirect stdout to the UART. it is located on `$PS2SDK/iop/irx/ppctty.irx`
