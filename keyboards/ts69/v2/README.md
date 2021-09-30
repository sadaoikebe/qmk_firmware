# TS69V2 keyboard firmware

65% keyboard that has split spacebar layout  

hardware: https://github.com/sadaoikebe/ts69v2  


## Pro Micro Flashing info

ATmega32u4 With 16MHz oscillator  
How to write bootloader and fuse:  

[//]: # "make ts69/v2:nicola:production"

[//]: # "avrdude -p m32u4 -P COM4 -c avrisp -U flash:w:ts69_v2_nicola_production.hex:i"

[//]: # "avrdude -p m32u4 -P COM4 -c avrisp -U hfuse:w:0xd9:m"

Using ISP Programmer

    make ts69v2:nicola:production
    avrdude -p m32u4 -c stk500v1 -P COM4 -b 19200 -U flash:w:ts69_v2_nicola_production.hex:i
    avrdude -p m32u4 -c stk500v1 -P COM4 -b 19200 -U hfuse:w:0xd9:m
    avrdude -p m32u4 -c stk500v1 -P COM4 -b 19200 -U lfuse:w:0xff:m
    avrdude -p m32u4 -c stk500v1 -P COM4 -b 19200 -U efuse:w:0xcb:m

http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega32u4&LOW=FF&HIGH=D9&EXTENDED=CB&LOCKBIT=FF

BOOTRST should be unprogrammed (Jump to application) when using qmk-dfu
