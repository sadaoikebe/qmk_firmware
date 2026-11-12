# TC69 capacitive (EC) — rebuilt for current QMK.
# Original (capacitive @690c285d): CUSTOM_MATRIX=yes, ANALOG_ENABLE=yes, NKRO_ENABLE=yes,
#   SRC += matrix.c analog.c
# MCU / BOOTLOADER / NKRO now live in keyboard.json.

# Full custom matrix, as the original. QMK's debounce is NOT applied
# (matrix_scan() is the original function; "lite" would insert debounce()).
CUSTOM_MATRIX = yes
SRC += matrix.c

# Replaces ANALOG_ENABLE = yes + SRC += analog.c (also sets HAL_USE_ADC=TRUE).
ANALOG_DRIVER_REQUIRED = yes

# Raw-ADC diagnostics over the QMK console. Default OFF; the normal build is unaffected.
#   qmk compile -kb tc69 -km nicola -e TC69_ADC_DIAG=yes
TC69_ADC_DIAG ?= no
ifeq ($(strip $(TC69_ADC_DIAG)), yes)
    CONSOLE_ENABLE = yes
    OPT_DEFS += -DTC69_ADC_DIAG
endif
