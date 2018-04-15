# stm32-hid-bootloader

This is a driverless (no USB drivers needed, even on Windows) USB HID bootloader
for STM32F10X devices. 

It doesn't use ST libraries for since they are bloated and buggy. Only CMSIS and
some required system files and macros have been used from ST provided SDK.

This allowed for a very small bootloader - under 3KB!

1KB after bootloader is reserved to the user for storing config data. 

If no config data is used, a blank config data (blank-config.bin) is provided so
firmwares could be flashed by the provided CLI tool like:

hid-flash blank-config.bin MYFIRMWARE.BIN

This bootloader should't have any compiler restrictions, so it should work with
any GCC ARM toolchain version (latest is always recommended!).
