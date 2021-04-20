# Just68

Just68 keyboard produced by Yang.

* Keyboard Maintainer: [Sadao Ikebe](https://github.com/sadaoikebe)
* Hardware Supported: Just68 (atmega32u4)
* Hardware Availability: https://item.taobao.com/item.htm?id=604019589546

Initially, TMK is installed on this board. To use a QMK based firmware, you might want to install a QMK bootloader. To install the bootloader, you need to prepare an `Arduino` and connect it to the pads on the board (usually you need to solder it onto the pads). The following is an explanation by qmk.

https://beta.docs.qmk.fm/using-qmk/guides/keyboard-building/isp_flashing_guide

The Just68 PCB exposes 6 pins for ISP(In-System Programming), and they are located just next to the ATMega32U4 chip. It is still accessible after the switch is mounted on the keyboard. From left to right, the pins are `VCC`, `SCLK`, `MOSI`, `MISO`, `RESET`, `GND`. The `GND` is the square one. You could program the flash with any AVR programmer, or a Raspberry Pi with `avrdude`.

When you use a Pro Micro as a programmer, the connection should look like:

![connection drawing](isp-flashing-promicro-conneciton.svg "Pro Micro ISP Flashing Connection")

```
Keyboard VCC (left)  - Pro Micro VCC
Keyboard SCLK        - Pro Micro 15
Keyboard MOSI        - Pro Micro 16
Keyboard MISO        - Pro Micro 14
Keyboard RESET       - Pro Micro 10
Keyboard GND (right) - Pro Micro GND
```

Note that the GND pin has a high thermal capacity, which makes soldering more difficult. Use of high-powered soldering iron is recommended.

Make example for this keyboard (after setting up your build environment):

    make just68:nicola

If you want to make an image which includes bootloader:

    make just68:nicola:production

Then you can flash the entire ROM including the bootloader this way:

    avrdude -P COM4 -p m32u4 -c avrisp -U flash:w:just68_nicola_production.hex:i

The COM port might be different, specify the port your Arduino is connected.

You also need to program the fuse in order to skip the bootloader. You might also need to enable bootmagic to fallback into bootloader as the PCB doesn't provide reset button.

    avrdude -P COM4 -p m32u4 -c avrisp -U hfuse:w:0xd9:m

other fuses might be L:0x5e, E:0xc6. While you don't need to update the fuse actually - if you really want to update the fuse the command would look like this:

    avrdude -P COM4 -p m32u4 -c avrisp -U lfuse:w:0x5e:m
    avrdude -P COM4 -p m32u4 -c avrisp -U efuse:w:0xc6:m

Fuse calculator:
http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega32u4&LOW=5E&HIGH=D8&EXTENDED=C6&LOCKBIT=FF

At this point, you can disconnect the Arduino and put back into the case. Programming using an ISP is required to write bootloader and fuse. All other parts can be written via the USB(DFU).

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
