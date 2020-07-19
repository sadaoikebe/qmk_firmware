/*
Copyright 2019 Jianfei Wang

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID 0xFEED
#define PRODUCT_ID 0x1966
#define DEVICE_VER 0x0001
#define MANUFACTURER YDKB
#define PRODUCT Just66
#define DESCRIPTION Just66 Keyboard

/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 13

/*
B5 上
B4 上から2番目
B3 ????
B2 MOSI 下から2番目
B1 SCLK 最下段

D7 Caps
D6 A
E2 2W
C7 E
D0 F
B7 G
F7 Y
F6 U
F5 I
F4 O
F1 P
F0 [


B0 no response
B6 no response
D1 no response
D2
D3 no response
D5 no response
D4 no response
C6 no response


*/

#define MATRIX_ROW_PINS { B5, B4, B3, B2, B1, B0 }
#define MATRIX_COL_PINS { D7, D6, E2, C7, D0, B7, F7, F6, F5, F4, F1, F0, E6 }
#define UNUSED_PINS
#define DIODE_DIRECTION COL2ROW

#define QMK_ESC_OUTPUT D7 // usually COL
#define QMK_ESC_INPUT B5 // usually ROW

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is not needed */
#define DEBOUNCE 5

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* disable these deprecated features by default */
#ifndef LINK_TIME_OPTIMIZATION_ENABLE
  #define NO_ACTION_MACRO
  #define NO_ACTION_FUNCTION
#endif
