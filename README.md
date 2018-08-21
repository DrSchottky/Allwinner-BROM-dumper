# Allwinner BROM dumper

Userspace tool to dump the internal BootROM of Allwinner SoCs

### Usage
`sudo ./dump <BROM start address> <BROM size> <outfile>`

Refer to your SoC datasheet for the correct values

Example for Allwinner H3/H2+ 

`sudo ./dump 0xffff0000 0x8000 H3-BROM.bin`
