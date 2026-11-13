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
#   make tc69:nicola TC69_ADC_DIAG=yes
#   make tc69:nicola TC69_ADC_DIAG=yes TC69_DIAG_PURGE=after   (EXPERIMENTAL purge order)
TC69_ADC_DIAG ?= no
ifeq ($(strip $(TC69_ADC_DIAG)), yes)
    CONSOLE_ENABLE = yes
    OPT_DEFS += -DTC69_ADC_DIAG
    # M1 toolkit options (diag builds only; see adc_diag_impl.h and ts52k docs/m1/)
    ifneq ($(strip $(TC69_DIAG_INTERVAL_MS)),)
        OPT_DEFS += -DTC69_ADC_DIAG_INTERVAL_MS=$(TC69_DIAG_INTERVAL_MS)
    endif
    ifneq ($(strip $(TC69_DIAG_ENV_MS)),)
        OPT_DEFS += -DTC69_DIAG_ENV_MS=$(TC69_DIAG_ENV_MS)
    endif
    ifneq ($(strip $(TC69_DIAG_TRACE_DECIM)),)
        OPT_DEFS += -DTC69_DIAG_TRACE_DECIM=$(TC69_DIAG_TRACE_DECIM)
    endif
    ifeq ($(strip $(TC69_DIAG_TRACE_ROW)), yes)
        OPT_DEFS += -DTC69_DIAG_TRACE_ROWVALS
    endif
    # fixed trace target "row,col", e.g. TC69_DIAG_TRACE_KEY=2,7 (default: first key pressed)
    TC69_COMMA := ,
    ifneq ($(strip $(TC69_DIAG_TRACE_KEY)),)
        OPT_DEFS += -DTC69_DIAG_TRACE_KEY_ROW=$(word 1,$(subst $(TC69_COMMA), ,$(TC69_DIAG_TRACE_KEY))) -DTC69_DIAG_TRACE_KEY_COL=$(word 2,$(subst $(TC69_COMMA), ,$(TC69_DIAG_TRACE_KEY)))
    endif
endif

# EXPERIMENTAL purge order (diag only): TC69_DIAG_PURGE=after -> sense.ino order
# (strobe, read, then purge for TC69_DIAG_PURGE_US, default 6 us as in sense.ino).
TC69_DIAG_PURGE ?= before
TC69_DIAG_PURGE_US ?= 6
ifeq ($(strip $(TC69_DIAG_PURGE)), after)
    ifneq ($(strip $(TC69_ADC_DIAG)), yes)
        $(error TC69_DIAG_PURGE=after is a diagnostic experiment and requires TC69_ADC_DIAG=yes)
    endif
    OPT_DEFS += -DTC69_DIAG_PURGE_AFTER -DTC69_DIAG_PURGE_US=$(TC69_DIAG_PURGE_US)
else ifneq ($(strip $(TC69_DIAG_PURGE)), before)
    $(error TC69_DIAG_PURGE must be "before" (default) or "after")
endif
