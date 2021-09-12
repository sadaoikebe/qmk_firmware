#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID 0xFEED
#define PRODUCT_ID 0x1968
#define DEVICE_VER 0x0001
#define MANUFACTURER Winry
#define PRODUCT Winry3X3A
#define DESCRIPTION Winry3X3A

/* key matrix size */
#define MATRIX_ROWS 1
#define MATRIX_COLS 9

#define MATRIX_ROW_PINS { F1 }
#define MATRIX_COL_PINS { B1, B2, D2, F4, B3, D3, E6, B4, D7 }
#define UNUSED_PINS
#define DIODE_DIRECTION COL2ROW

#define QMK_ESC_OUTPUT B1 // usually COL
#define QMK_ESC_INPUT F1 // usually ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* RGB Underglow
 */
#define RGB_DI_PIN F7
#define RGBLIGHT_ANIMATIONS
#define RGBLED_NUM 8
#define RGBLIGHT_HUE_STEP 8
#define RGBLIGHT_SAT_STEP 8
#define RGBLIGHT_VAL_STEP 8

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
//#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
//#define LOCKING_RESYNC_ENABLE

/* disable these deprecated features by default */
#ifndef LINK_TIME_OPTIMIZATION_ENABLE
  #define NO_ACTION_MACRO
  #define NO_ACTION_FUNCTION
#endif
