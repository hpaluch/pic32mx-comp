# PIC32MX Comparator

We will use comparator on PIC32MX to measure frequency of input
signal. We will simply count number of Comparator interrupts per
second to get signal frequency in Hz and print it on UART every
second.

Project status:
- Work in Progress
- RED LED blinking at 1/2 Hz using core timer.

Used CPU Pins:

| Pin | Peripheral | Function |
| --- | --- | --- |
| 1 | /MCLR | Reserved for CPU RESET |
| 2 | RA0 | LED blinking at 1/2 Hz frequency |
| 4 | PGED1 | Reserved for Programmer/Debugger Data |
| 5 | PGEC1 | Reserved for Programmer/Debugger Clock |

Used CPU blocks:
- `MIPS Core Timer` at 1 Hz
  - it uses CP0 (Coprocessor 0) Register 9 (Count)
    and Register 11 (Compare)
  - Interrupt is acknowledge by writing to Compare register (!)
  - please see Page 73 (real page 83) of 
    [MIPS32(R) M4K Manual][MIPS32(R) M4K Manual]
  - it is similar to SYSTICK on ARM Cortex-M CPUs

Thanks to "After Build" command you can see generated assembler
listing under:
```
firmware/pic32mx_comp.X/dist/mstick2/MODE/pic32mx_comp.X.MODE.lst
```
Where `MODE` is `production` of `debug` - depends on selected
build in MPLAB Toolbar.

# Hardware requirements

* [Microstick II][PIC Microstick II]  demo board
* [PIC32MX250F128B SPDIP][PIC32MX250F128B] inserted into socket U5
  (included with board, should be default)
* programming pins set `1` (Pin4 & Pin5): switch `S1` in position `A`

PIC32MX is 32-bit `MIPS32(R) 4K` based CPU
- PIC32MX datasheet: [PIC32MX250F128B][PIC32MX250F128B]
- MIPS32 4K datasheet: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00086-2B-MIPS32BIS-AFP-05.04.pdf

# Software requirements

* [XC32 compiler][XC compilers] - tested version v4.30
* [MPLAB X IDE][MPLAB X IDE] - tested version v6.15
  - using `PIC32MX_DFP` v1.5.259
  - using default MCC Plugin 5 that now also contains Harmony

# Setup

Microstick setup:
- [PIC32MX250F128B][PIC32MX250F128B] in `U5` socket (mind the Key!)
- Jumper `J3` closed (connects pin 2 to RED LED)
- Swwitch S1 in position A

Wiring:

TODO

Is URL below correct?
[MIPS32(R) M4K Manual]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00016-2B-4K-SUM-01.18.pdf
[Harmony]: https://www.microchip.com/mplab/mplab-harmony
[XC compilers]: https://www.microchip.com/mplab/compilers
[MPLAB X IDE]: https://www.microchip.com/mplab/mplab-x-ide
[PIC32MX250F128B]: https://www.microchip.com/wwwproducts/en/PIC32MX250F128B
[PIC Microstick II]: https://www.microchip.com/DevelopmentTools/ProductDetails/dm330013-2
