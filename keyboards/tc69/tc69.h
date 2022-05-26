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
 * This keyboard has electrically 9 rows 8 cols switch matrix, while it looks like 5 rows 16 cols layout.
 */
#define LAYOUT( \
      K05,   K07,   K06,   K04,   K02,   K01,   K00,   K03,   K0D,   K0F,   K0E,   K0C,   K0A,  K09,        K0B, \
      K15,   K17,   K16,   K14,   K12,   K11,   K10,   K13,   K1D,   K1F,   K1E,   K1C,   K1A,        K18,  K1B, \
      K25,   K27,   K26,   K24,   K22,   K21,   K20,   K23,   K2D,   K2F,   K2E,   K2C,         K29,        K2B, \
      K35,          K36,   K34,   K32,   K31,   K30,   K33,   K3D,   K3F,   K3E,   K3C,         K39,  K38,  K3B, \
      K45,   K47,          K44,          K41,                 K4D,          K4E,   K4C,   K4A,  K49,  K48,  K4B  \
) { \
    { K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07, KC_NO,   K09,   K0A,   K0B,   K0C,  K0D,  K0E,   K0F }, \
    { K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17,   K18, KC_NO,   K1A,   K1B,   K1C,  K1D,  K1E,   K1F }, \
    { K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27, KC_NO,   K29, KC_NO,   K2B,   K2C,  K2D,  K2E,   K2F }, \
    { K30,   K31,   K32,   K33,   K34,   K35,   K36, KC_NO,   K38,   K39, KC_NO,   K3B,   K3C,  K3D,  K3E,   K3F }, \
    {KC_NO,  K41, KC_NO, KC_NO,   K44,   K45, KC_NO,   K47,   K48,   K49,   K4A,   K4B,   K4C,  K4D,  K4E, KC_NO }, \
}
