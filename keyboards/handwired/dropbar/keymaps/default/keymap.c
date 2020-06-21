/*
Copyright 2012,2013 Jun Wako <wakojun@gmail.com>

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

#include QMK_KEYBOARD_H

// F5 F6 NA NA
// F7 F8 NA NA
// NA NA F4 F3
// NA NA F2 F1

// RIGHT HAND
// BODY
// F1 S
// F2 D
// F3 D
// F4 S
// BRACKET

// LEFT HAND
// BRACKET
// F5 SPACE
// F6 A
// F7 A
// F8 SPACE


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_C, KC_A, KC_NO, KC_NO, \
     KC_A, KC_C, KC_NO, KC_NO, \
     KC_NO, KC_NO, KC_S, KC_D, \
     KC_NO, KC_NO, KC_D, KC_S \
    ),
};
