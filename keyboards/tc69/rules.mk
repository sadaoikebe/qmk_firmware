MCU = RP2040
BOOTLOADER = rp2040
CUSTOM_MATRIX = yes
ANALOG_ENABLE = yes

NKRO_ENABLE = yes

SRC +=  matrix.c
SRC +=  analog.c
