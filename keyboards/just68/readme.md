# Just68

Just68 keyboard produced by Yang.

To use a QMK based firmware, you might want to install a QMK bootloader. The PCB exposes 6 pins for ISP(In-System Programming), and they are located just under the ATMega32U4 chip. From left to right, the pins are `VCC`, `SCLK`, `MOSI`, `MISO`, `RESET`, `GND`. The `GND` is the square one. You could program the flash with any AVR programmer, or a Raspberry Pi with `avrdude`.

You also need to program the fuse in order to skip the bootloader. You might also need to enable bootmagic to fallback into bootloader as the PCB doesn't provide reset button.

    avrdude -P COM4 -p m32u4 -c avrisp -U hfuse:w:0xd9:m

* Keyboard Maintainer: [Sadao Ikebe](https://github.com/sadaoikebe)
* Hardware Supported: Just68 (atmega32u4)
* Hardware Availability: https://item.taobao.com/item.htm?id=604019589546

Make example for this keyboard (after setting up your build environment):

    make just68:nicola_106key

See the [build environment setup](https://docs.qmk.fm/#/getting_started_build_tools) and the [make instructions](https://docs.qmk.fm/#/getting_started_make_guide) for more information. Brand new to QMK? Start with our [Complete Newbs Guide](https://docs.qmk.fm/#/newbs).
