## Thumbshift69 keyboard

65% keyboard that has split spacebar layout  

Using 5V 16MHz Pro Micro
How to write bootloader and fuse:  

[//]: # "make thumbshift69:nicola:production"

[//]: # "avrdude -p m32u4 -P COM4 -c avrisp -U flash:w:thumbshift69_nicola_production.hex:i"

[//]: # "avrdude -p m32u4 -P COM4 -c avrisp -U hfuse:w:0xd9:m"

Using ISP Programmer
    make thumbshift69:nicola:production
    avrdude -p m32u4 -c stk500v1 -P COM4 -b 19200 -U flash:w:thumbshift69_nicola_production.hex:i
    avrdude -p m32u4 -c stk500v1 -P COM4 -b 19200 -U hfuse:w:0xd9:m

http://eleccelerator.com/fusecalc/fusecalc.php?chip=atmega32u4&LOW=FF&HIGH=D9&EXTENDED=CB&LOCKBIT=FF

BOOTRST should be unprogrammed (Jump to application) when using qmk-dfu
