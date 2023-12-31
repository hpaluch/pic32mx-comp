# PIC32MX Comparator

Here is example how to (not) use Comparator on PIC32MX to measure signal
frequency.  We simply count number of Comparator interrupts per second to get
signal frequency in Hz and print it on UART every second.

To work properly signal must be in proper range from minimum V<sub>ss</sub>=0V to 
maximum V<sub>dd</sub>=3.3V and it must cross-over 1.65V (1/2 of Vdd) on every period.

And more important: input signal has to have some *minimum slope* to *avoid oscillations*
of Comparator! See text below for examples.

# Proper comparator operation

Here is example of Proper Comparator operation - when using Square signal on 
Inverting comparator input `C1INB` (I use inverting input because it is only
Comparator configuration that allows use `CVref` reference voltage source).

![Square Comparator on Scope](digilentad2/pic32mx-comp-scope-square.gif)

Notice that Comparator interrupt (`C1INT_TOGGLE`) occurs only on every *second* transition
(high-to-low transition because we use Inverting input) - which is as it was configured
on MCC Harmony tool.

Here is Square wave signal generator Configuration:

![Square Signal Generator](digilentad2/pic32mx-gen-square.gif)

Workspace file for [Digilent Analog Discovery 2 Scope][Digilent AD2]
is on: [digilentad2/PIC32MX-COMP-Square.dwf3work](digilentad2/PIC32MX-COMP-Square.dwf3work)

# Incorrect comparator operation

When signal slope is too slow (for example using Sine wave) you may encounter incorrect
comparator behaviour as shown on scope below:

![Sine Comparator on Scope](digilentad2/pic32mx-comp-scope-sine.gif)

Notice that Comparator interrupt (`C1INT_TOGGLE`) occurs on EVERY comparator
transition - which is wrong! Actually there are at least 2 forth and back
transitions of comparator on very cross - but it is impossible to detect them
with common scope (even using very short Time base)!

In such case we will incorrectly show double frequency on UART output!

Here is signal generator for Sine:

![Sine Signal Generator](digilentad2/pic32mx-gen-sine.gif)

Workspace file for [Digilent Analog Discovery 2 Scope][Digilent AD2]
is on: [digilentad2/PIC32MX-COMP-Sine.dwf3work](digilentad2/PIC32MX-COMP-Sine.dwf3work)

Possible fixes:
- I plan to implement hysteresis using positive feedback (basically connecting
  inverting C1OUT to inverting C1INB via resistor which should add hysteresis).
- I'm currently studying `DS41215C-page 7`
  from: http://ww1.microchip.com/downloads/en/DeviceDoc/41215c.pdf

# Project status

Project status:
- OK: reporting input signal frequency (number of high-to-low comparator transition
  interrupts per second)
- signal generator should be connected to Pin 6 C1INB (cross-over 1.65V, minimum 0V, maximum 3.3V)
  Recommended: use 1 kOhm serial protective resistor between generator and `C1INB` input.
- PROBLEM: When signal slope is too slow it will cause comparator oscillations which will
  be observed as double interrupts (looks like interrupt is generated on every transition,
  because of oscillations on such transitions). So measured frequency on UART output will
  be 2 times higher then real frequency (!)

Additionally:
- RA0 RED LED blinking at 1/2 Hz using Core timer.
- PIN18 C1OUT - current comparator output (for verification)
- PIN25 CVREFOUT should have output voltage 1.65V (threshold for comparator)


Example UART output (115200 baud, 8 data-bits, 1 stop, no parity, no flow control) on Pin21 U2TX:
```
../src/main.c:74 starting app v1.03
 CPU Frequency=48000004 [Hz]
 Uptime=3 [s] delta CMP1=997 [Hz]
 Uptime=4 [s] delta CMP1=996 [Hz]
 Uptime=5 [s] delta CMP1=997 [Hz]
 ...
```

Used CPU Pins:

| Pin | Peripheral | Function |
| --- | --- | --- |
| 1 | /MCLR | Reserved for CPU RESET |
| 2 | RA0 | LED blinking at 1/2 Hz frequency |
| 3 | RA1 | Output Toggle on CMP interrupt |
| 4 | PGED1 | Reserved for Programmer/Debugger Data |
| 5 | PGEC1 | Reserved for Programmer/Debugger Clock |
| 6 | C1INB | Signal input to comparator from 0V to 3.3V |
| 18 | C1OUT | Comparator output (for verification) |
| 21 | U2TX | PIC UART Output (PC Input!) |
| 22 | U2RX | PIC UART Input (PC Output!) |
| 25 | CVREFOUT | Reference for CMP, should be  1.65V (3.3V/2) |


Used CPU blocks:
- `MIPS Core Timer` at 1 Hz
  - it uses CP0 (Coprocessor 0) Register 9 (Count)
    and Register 11 (Compare)
  - Interrupt is acknowledge by writing to Compare register (!)
  - please see Page 73 (real page 83) of 
    [MIPS32(R) M4K Manual][MIPS32 M4K Manual]
  - it is similar to SYSTICK on ARM Cortex-M CPUs
- `CVREF`
  - voltage reference (threshold) for comparator
  - set to 1.65V (half of Vdd = 3.3/2) using `CVRR=1` and `CVRCON=12`
  - measured 1.63V on Pin 25 CV<sub>REFOUT</sub> with DMM
- `CMP1`
  - Comparator 1
  - `C+IN` (non-inverting input) connected to CVREF
  - `C-IN` (inverting input) connected to `C1INB` pin (because CVREF is allowed
    to be connected to non-inverting input only - for some reason).
  - generates interrupt on low-to-high transition, but actually opposite because we
    use inverting input for signal.
  - `CVOUT` available to check behaviour

Thanks to "After Build" command you can see generated assembler
listing under:
```
firmware/pic32mx_comp.X/dist/mstick2/MODE/pic32mx_comp.X.MODE.lst
```
Where `MODE` is `production` of `debug` - depends on selected
build in MPLAB Toolbar.

# Hardware requirements

* [Digilent Analog Discovery 2 Scope][Digilent AD2] - using signal generator, scope and 2 digital inputs
* [Microstick II][PIC Microstick II]  demo board
* [PIC32MX250F128B SPDIP][PIC32MX250F128B] inserted into socket U5
  (included with board, should be default)
* USB to UART Adapter, tested:
  [USB Console Cable #954][USB Console Cable #954]

PIC32MX is 32-bit `MIPS32(R) M4K` based CPU
- PIC32MX datasheet: [PIC32MX250F128B][PIC32MX250F128B]
- [MIPS32 M4K Manual][MIPS32 M4K Manual] from mips.com
- [MIPS32 M4K Core Datasheet][MIPS32 M4K DTS] from mips.com
- [MIPS32 Instruction Set][MIPS32 BIS] from mips.com
- [MIPS32 Instruction Set Quick Reference][MIPS32 QRC] from mips.com

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

But do NOT connect RS232 converters! They use +/-12V which will surely
blow CPU!

Use these parameters when connecting Putty to USB/UART Adapter:
- Speed (baud): `115200`
- Data bits: `8`
- Stop bit: `1`
- Parity: `none`
- Flow control: `None`

Wiring Microstick II and DMM (to check Voltage reference for comparator)

| Microstick II Pin | Signal | DMM |
| --- | --- | --- |
| 27 | GND | GND |
| 25 | CVREFOUT | Voltage Input |

Your DMM should show nominal voltage 1.65V (V<sub>dd</sub>/2 = 3.3/2 = 1.65 )

Wiring Microstick II to Digilent Analog Discovery 2 scope/generator:

| Microstick II Pin | Signal | Digilent Signal |
| --- | --- | --- |
| 3 | RA1 | Digital 0 input (toggles on Comparator interrupt) |
| 6 | C1INB | W1 generator output through 1 kOhm protective resistor |
| 6 | C1INB | 1+ Scope 1st channel|
| 18 | C1OUT | 2+ Scope 2nd channel|
| 27 | GND | GND |
| 27 | GND | `1-` 1st channel inverting input |
| 27 | GND | `2-` 2nd channel inverting input |


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
- if there is already shown single item list of drivers then
  UNCHECK "Show compatible drivers only"
- now select Vendor "Microsoft"
- and Driver "USB Serial Device"
- click on Next
- click Yes on scary Warning that you really want use this driver
- now Windows 10 Generic driver should be installed and working

## Measured frequency is 2 times higher

If measured frequency is 2 times higher it is very likely that
comparator is oscillating. I plan to fix that with adding hysteresis (TODO).

# Notes

Why not using "Shadow register set" to reduce interrupt latency?
- because the HSS of CP0,SRSCtl register equals to 0, which means that
  this CPU has 0 Shadow register sets...

[Digilent AD2]: https://digilent.com/shop/analog-discovery-2-100ms-s-usb-oscilloscope-logic-analyzer-and-variable-power-supply/
[MIPS32 M4K Manual]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00249-2B-M4K-SUM-02.03.pdf
[MIPS32 M4K DTS]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00247-2B-M4K-DTS-02.01.pdf
[MIPS32 BIS]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00086-2B-MIPS32BIS-AFP-05.04.pdf
[MIPS32 QRC]: https://s3-eu-west-1.amazonaws.com/downloads-mips/documents/MD00565-2B-MIPS32-QRC-01.01.pdf 
[Harmony]: https://www.microchip.com/mplab/mplab-harmony
[XC compilers]: https://www.microchip.com/mplab/compilers
[MPLAB X IDE]: https://www.microchip.com/mplab/mplab-x-ide
[PIC32MX250F128B]: https://www.microchip.com/wwwproducts/en/PIC32MX250F128B
[PIC Microstick II]: https://www.microchip.com/DevelopmentTools/ProductDetails/dm330013-2
[USB Console Cable #954]: https://www.modmypi.com/raspberry-pi/communication-1068/serial-1075/usb-to-ttl-serial-cable-debug--console-cable-for-raspberry-pi "USB Console Cable #954"
