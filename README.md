
### What this is:
- homebrew application operated via UART for the PowerPC CPU used by sony to replace the PS1 CPU on most slim models.
- loader for the software mentioned before
- toolchain install scripts for PowerPC compiled
- Documentation (hosted on github pages)

### What this not (yet):
- A complete SDK
- A PPC module loader

### Requirements:
+ hardware
  - PS2 DECKARD: `SCPH-75xxx` (PS2TV Included)
  - **3.3V** USB-TTL adapter
  - Connection to PowerPC UART (both TX and RX of course)
+ software
  - PS2SDK installed
  - serial monitor ([recommeneded one for windows](https://github.com/fasteddy516/SimplySerial))

### Building:
1. Install PS2SDK build requirements if you haven't already. (gcc/clang, make, cmake, patch, git, etc)
2. Make sure the PS2DEV environment variable is set. The toolchain buildscripts will install to the PS2DEV directory
3. Build binutils and GCC for PPC either by doing 
```sh
make toolchain
```
or running the following scripts
```sh
./buildscripts/toolchain/001-binutils.sh
./buildscripts/toolchain/002-gcc-stage1.sh
```
4. Once the toolchain is built and installed you can simply run `make` in the root directory and both the patch and patch loader will be built  
you may need to add `$PS2DEV/powerpc/bin` to your `$PATH`
5. Open a serial monitor of your choosing and set the baud rate to 57600. When the PS2 is powered on or reset "D E C K A R D compiled xxxxx" should appear in the serial monitor. If it does not, check your connection and ensure the baud is set correctly.
6. Run patch_loader.elf via ps2link or any other means of launching homebrew. You should see the following in your serial monitor:
```log
LUT resized and relocated
Welcome to PPC-MON v0.1
```

### Special Thanks
- [@Wisi](https://github.com/wisi-w) for allowing qnox32 to incorporate and build upon their work from dpLibMod. Without Wisi's initial work, none of this would exist.
- Nocash for providing an invaluable resource for all things PS1 (https://problemkaputt.de/psx-spx.htm)

### Other Projects Making Use of the PPC-IOP
 - [dpLibMod](https://www.psx-place.com/threads/hdd-for-ps2-scph75000x-later-models.30696/page-2#post-255399) HDD Emulator and set of PPC libraries for use with a custom CPLD by Wisi.
 - [DKWDRV](https://github.com/wisi-w/DKWDRV/releases) excellent PS1DRV replacement that someday will allow loading PS1 games via USB (by anonymous dev).
 - [PPC-TTY](https://cdn.discordapp.com/attachments/707601990422757448/1143638991728562329/ppctty.zip) An IOP module which redirects IOP printf to PPC-UART by asmblur.


### Additional Resources
- [PPC440x5 CPU Core User's Manual](http://class.ece.iastate.edu/cpre584/ref/Xilinx/edk/ppc440x5_um.pdf)
