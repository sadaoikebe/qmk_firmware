## Shobon62 keyboard

Handwired keyboard that has split spacebar layout  
How to write bootloader and fuse:  

    make handwired/shobon62:nicola:production
    avrdude -p m32u4 -P COM4 -c avrisp -U flash:w:handwired_shobon62_nicola_production.hex:i
    avrdude -p m32u4 -P COM4 -c avrisp -U hfuse:w:0xd9:m
    avrdude -p m32u4 -P COM4 -c avrisp -U lfuse:w:0x5e:m
    avrdude -p m32u4 -P COM4 -c avrisp -U efuse:w:0xc6:m
