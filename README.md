# PIC32MX Comparator

We will use comparator on PIC32MX to measure frequency of input
signal. We will simply count number of Comparator interrupts per
second to get signal frequency in Hz and print it on UART every
second.

Project status:
- Work in Progress
- RED LED blinking at 1/2 Hz using core timer.
- Also Dumps Uptime information on UART2 (you will need suitable
  USB <-> UART cable - see text below)

Example UART output (115200 baud, 8 data-bits, 1 stop, no parity, no flow control) on Pin21 U2TX:
```
../src/main.c:63 starting app v1.01
 CPU Frequency=47999998 [Hz]
 Uptime=3 [s]
 Uptime=4 [s]
 Uptime=5 [s]
 ...
```

Used CPU Pins:

| Pin | Peripheral | Function |
| --- | --- | --- |
| 1 | /MCLR | Reserved for CPU RESET |
| 2 | RA0 | LED blinking at 1/2 Hz frequency |
| 4 | PGED1 | Reserved for Programmer/Debugger Data |
| 5 | PGEC1 | Reserved for Programmer/Debugger Clock |
| 21 | U2TX | PIC UART Output (PC Input!) |
| 22 | U2RX | PIC UART Input (PC Output!) |

Used CPU blocks:
- `MIPS Core Timer` at 1 Hz
  - it uses CP0 (Coprocessor 0) Register 9 (Count)
    and Register 11 (Compare)
  - Interrupt is acknowledge by writing to Compare register (!)
  - please see Page 73 (real page 83) of 
    [MIPS32(R) M4K Manual][MIPS32 M4K Manual]
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
* USB to UART Adapter, tested:
  [USB Console Cable #954][USB Console Cable #954]

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

Wiring Microstick II and USB UART Cable
- I used [USB Console Cable #954][USB Console Cable #954]

| Microstick II Pin | Signal | USB UART Cable |
| --- | --- | --- |
| 27 | GND | GND (Black) |
| 21 | U1TX | PC RX - Input (White) * |

Important: I connected pin 21 to USB White wire through
1 kOhm protective resistor!!!

NOTE: Pin 21 and Pin 22 are 5V tolerant, which is good thing - so you
can use basically any USB <-> UART cable - both 3.3V TTL and 5V TTL should
work.

But do NOT connect RS232 convertes! They use +/-12V which will surely
blow CPU!

Use these parameters when connecting Putty to USB/UART Adapter:
- Speed (baud): `115200`
- Data bits: `8`
- Stop bit: `1`
- Parity: `none`
- Flow control: `None`

# Troubleshooting

## "This is not Prolific" Error

When using [USB Console Cable #954][USB Console Cable #954]
with Windows you may get horrible error "This is not Prolific"
in Device Manager. 

This is dirty trick of Prolific chip Vendor how to brick both clones and also
old authentic chips (that were indistinguishable from clones)!

For Windows 10 there exist easy workaround:
- select Update Driver
- select I will pick driver
- if there  is already shown single item list of drivers then
  UNCHECK "Show compatible drivers only"
- now select Vendor "Microsoft"
- and Driver "USB Serial Device"
- click on Next
- click Yes on scary Warning that you really want use this driver
- now Windows 10 Generic driver should be installed and working


Is URL below correct?

[MIPS32 M4K Manual]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00016-2B-4K-SUM-01.18.pdf
[Harmony]: https://www.microchip.com/mplab/mplab-harmony
[XC compilers]: https://www.microchip.com/mplab/compilers
[MPLAB X IDE]: https://www.microchip.com/mplab/mplab-x-ide
[PIC32MX250F128B]: https://www.microchip.com/wwwproducts/en/PIC32MX250F128B
[PIC Microstick II]: https://www.microchip.com/DevelopmentTools/ProductDetails/dm330013-2
[USB Console Cable #954]: https://www.modmypi.com/raspberry-pi/communication-1068/serial-1075/usb-to-ttl-serial-cable-debug--console-cable-for-raspberry-pi "USB Console Cable #954"
