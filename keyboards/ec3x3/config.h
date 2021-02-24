/*
Copyright 2012 Jun Wako <wakojun@gmail.com>
Copyright 2015 Jack Humbert

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

/* USB Device descriptor parameter */
#define VENDOR_ID 0xFEED

#define PRODUCT_ID      0x6363
#define DEVICE_VER      0x0001
#define MANUFACTURER    Shobon
#define PRODUCT         ec3x3
#define DESCRIPTION     Shobon EC3x3

#define TAPPING_FORCE_HOLD
#define TAPPING_TERM 100

/* key matrix size */
// Rows are doubled-up
#define MATRIX_ROWS 3
#define MATRIX_ROW_PINS { D4, C6, D7 }

// wiring of each half
#define MATRIX_COLS 3
#define MATRIX_COL_PINS { E6, B4, B5 }

// custom matrix
#define CHARGE_PIN B2
#define ADC_PIN B6
