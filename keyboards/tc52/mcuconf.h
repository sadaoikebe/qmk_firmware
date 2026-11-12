// Copied from tc69/mcuconf.h (capacitive @690c285d), unchanged.
// Still required in current QMK: GENERIC_RP_RP2040 mcuconf.h has RP_ADC_USE_ADC1 FALSE.
#pragma once

#include_next <mcuconf.h>

#undef RP_ADC_USE_ADC1
#define RP_ADC_USE_ADC1 TRUE
