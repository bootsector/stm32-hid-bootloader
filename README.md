# stm32-hid-bootloader

This is a driverless (no USB drivers needed, even on Windows) USB HID bootloader
for STM32F10X devices. 

It doesn't use ST libraries since they are bloated and buggy. Only CMSIS and
some required system files and macros have been used from ST provided SDK.

This allowed for a very small bootloader - less than 4KB! 

Firmware .bin files can be flashed with the provided CLI tool like:

hid-flash MYFIRMWARE.BIN

Please check the cli directory for the hid-flash tool source code.

Bootloader will activate (i.e., enter USB HID mode) if B2 (BOOT1) pin is driven HIGH 
when bootloader starts. Otherwise, it will simply jump to the user program address.

If you have a "Blue Pill" board, setting jumper BOOT1 to the 1 position (and leaving 
BOOT0 at the 0 position), the bootloader will kick in.

Latest version of the GCC ARM toolchain is recommended for building the bootloader.
