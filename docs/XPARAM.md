---
layout: default
title: PowerPC XPARAM compatibility modes
nav_order: 5
---

The `XPARAM` IOP module writes **PARAM** values to `0xFFFE01a0` and `0xFFFE01a4`.  
In the PPC store handler for these addresses copy the **PARAM** value to a  
struct in memory @ `0xbe09e0` - `0xbe0a24`. From there the values are used 
by their respective functions. The XPARAM struct can written to directly  
bypassing the need to use the `rom0:XPARAM` IOP module wich was operated by `rom0:PS2LOGO` on Sony's plans.  

original design included a database of games that needed of XPARAMs to work.  
aditionally, game's `SYSTEM.CNF` could provide their own XPARAM settings by adding an `PARAM2` entry. and the value passed had to be the XPARAM argumments followed by an MD5 hash. Disc params had higher priority than rom database params.


SYSTEM.CNF example taken from Critical Velocity (`SLPS_255.32`):
```ini
BOOT2 = cdrom0:\SLPS_255.32;1
VER = 1.01
VMODE = NTSC
PARAM2 = 0X10_0:812E98BD247B5EB74CD7A1F9EEDBF355
```

[^1]: PS3/PS4_emulators: on PS3/PS4 emulated PS2. the `SYSTEM.CNF` entry looked for is `PARAM4`, and the module was renamed to `rom0:XPARAM2`.

You can find copies of the two versions of the XPARAM database as well as some information about them [here](https://github.com/ps2homebrew/Open-PS2-Loader/tree/master/notes/xparam)

you can find a list of the IDs for the games that added their own XPARAM modules in [this piece of code](https://github.com/ps2homebrew/Open-PS2-Loader/blob/master/src/xparam.c#L16-L51)



`0xFFFE01a0` = index
`0xFFFE01a4` = value

## XPARAM Struct Address

range: `0xbe09e0` - `0xbe0a24`

| Value  | Name                             |
| ------ | -------------------------------- |
| `0x0`  | `PARAM_MDEC_DELAY_CYCLE`         |
| `0x4`  | `PARAM_SPU_INT_DELAY_LIMIT`      |
| `0x8`  | `PARAM_SPU_INT_DELAY_PPC_COEFF`  |
| `0xC`  | `PARAM_SPU2_INT_DELAY_LIMIT`     |
| `0x10` | `PARAM_SPU2_INT_DELAY_PPC_COEFF` |
| `0x14` | `PARAM_DMAC_CH10_INT_DELAY`      |
| `0x18` | `PARAM_CPU_DELAY`                |
| `0x1C` | `PARAM_SPU_DMA_WAIT_LIMIT`       |
| `0x20` | `PARAM_GPU_DMA_WAIT_LIMIT`       |
| `0x24` | `PARAM_DMAC_CH10_INT_DELAY_DPC`  |
| `0x28` | `PARAM_CPU_DELAY_DPC`            |
| `0x2c` | `PARAM_USB_DELAYED_INT_ENABLE`   |
| `0x30` | `PARAM_TIMER_LOAD_DELAY`         |
| `0x34` | `PARAM_SIO0_DTR_SCK_DELAY`       |
| `0x38` | `PARAM_SIO0_DSR_SCK_DELAY_C`     |
| `0x3c` | `PARAM_SIO0_DSR_SCK_DELAY_M`     |
| `0x40` | `PARAM_MIPS_DCACHE_ON`           |
| `0x44` | `PARAM_CACHE_FLASH_CHANNELS`     |

### PARAM_CPU_DELAY
used to control timing: 
```
(ppc_clk_delta * mips_clk_factor) / PARAM_CPU_DELAY or PARAM_CPU_DELAY_DPC
```

### PARAM_TIMER_LOAD_DELAY
Controls load delay when reading from timer0 in ps1 mode

### PARAM_SIO0_D* 
appear to be multiples of the CPU clock period (2.27ns @ 440MHz).
#### PARAM_SIO0_DTR_SCK_DELAY
```
0x229c == 8860 * 2.27ns = 20.1122 uS
```
#### PARAM_SIO0_DSR_SCK_DELAY_C
```
0x6ec  == 1772 * 2.27ns = 4.02244 uS
```
#### PARAM_SIO0_DSR_SCK_DELAY_M
```
0x6ec  == 1772 * 2.27ns = 4.02244 uS
```

### PARAM_MIPS_DCACHE_ON
Controls which set of TLB entries are used for the lower 2MBs of IOP RAM.
- `0`: uncached entries for this region
- `1`: cached entries for this region