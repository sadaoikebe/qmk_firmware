/*
 * (C) 2020 Sadao Ikebe @bonyarou
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "quantum.h"

/* This is a shortcut to help you visually see your layout.
 *
 * The first section contains all of the arguments representing the physical
 * layout of the board and position of the keys.
 *
 * The second converts the arguments into a two-dimensional array which
 * represents the switch matrix.
 *
 * This keyboard has electrically 5 rows 6 cols duplex matrix
 *
 */
#define LAYOUT( \
      K00,  K10,  K01,  K11,  K31,  K02,  K12,  K32,  K03,  K13,  K04,  K14,  K34,  K05,  K15,  \
      K20,  K30,  K21,  K41,  K51,  K22,  K52,  K63,  K23,  K33,  K24,  K54,  K25,        K35,  \
      K40,        K50,  K61,  K71,  K42,  K62,  K72,  K43,  K53,  K64,  K74,  K65,  K45,        \
      K60,  K80,  K90,        K81,              K92,        K83,  K93,  K84,  K94,  K85,  K95   \
) { \
    /* COL2ROW */ \
    { K00,   K01,   K02,   K03,   K04,   K05   }, \
    { K20,   K21,   K22,   K23,   K24,   K25   }, \
    { K40,   K41,   K42,   K43,   KC_NO, K45   }, \
    { K60,   K61,   K62,   K63,   K64,   K65   }, \
    { K80,   K81,   KC_NO, K83,   K84,   K85   }, \
    /* ROW2COL */ \
    { K10,   K11,   K12,   K13,   K14,   K15   }, \
    { K30,   K31,   K32,   K33,   K34,   K35   }, \
    { K50,   K51,   K52,   K53,   K54,   KC_NO }, \
    { KC_NO, K71,   K72,   KC_NO, K74,   KC_NO }, \
    { K90,   KC_NO, K92,   K93,   K94,   K95   } \
}
