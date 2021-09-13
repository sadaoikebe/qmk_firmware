#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID 0xFEED
#define PRODUCT_ID 0x1968
#define DEVICE_VER 0x0001
#define MANUFACTURER CXT
#define PRODUCT 12E4
#define DESCRIPTION CXT12E4

/* key matrix size */
#define MATRIX_ROWS 4
#define MATRIX_COLS 4

#define MATRIX_ROW_PINS { C7, C6, D6, F4 }
#define MATRIX_COL_PINS { D4, D7, B4, B5 }
#define UNUSED_PINS
#define DIODE_DIRECTION ROW2COL

#define QMK_ESC_OUTPUT C7 // usually COL
#define QMK_ESC_INPUT D4 // usually ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* RGB Underglow
 */
#define RGB_DI_PIN F7
#define RGBLIGHT_ANIMATIONS
#define RGBLED_NUM 12
#define RGBLIGHT_HUE_STEP 8
#define RGBLIGHT_SAT_STEP 8
#define RGBLIGHT_VAL_STEP 8

#define ENCODERS_PAD_A { D3, B3, F0, F6 }
#define ENCODERS_PAD_B { D5, B2, E6, F5 }
#define ENCODER_RESOLUTIONS { 2, 4, 4, 4 }

/* disable these deprecated features by default */
#ifndef LINK_TIME_OPTIMIZATION_ENABLE
  #define NO_ACTION_MACRO
  #define NO_ACTION_FUNCTION
#endif
