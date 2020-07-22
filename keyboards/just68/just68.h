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
 * The Just68 has electrically 6 rows 13 cols switch matrix, while it looks like 5 rows 15 cols layout.
 * The keys at the right are actually wired to 6th row.
 */
#define LAYOUT( \
      K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07,   K08,   K09,   K0A,   K0B,   K0C,   K5A,  K58, \
      K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17,   K18,   K19,   K1A,   K1B,   K1C,   K2C,  K59, \
      K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29,   K2A,   K2B,          K5C,  K57, \
      K30,   K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K3A,   K3B,          K3C,   K55,  K53, \
      K40,   K41,   K42,          K44,   K48,                        K4A,   K4B,   K4C,   K56,   K54,  K52  \
) { \
    { K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07,   K08,   K09,   K0A,   K0B,   K0C }, \
    { K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17,   K18,   K19,   K1A,   K1B,   K1C }, \
    { K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29,   K2A,   K2B,   K2C }, \
    { K30,   KC_NO, K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K3A,   K3B,   K3C }, \
    { K40,   K41,   K42,   KC_NO, K44,   KC_NO, KC_NO, KC_NO, K48,   KC_NO, K4A,   K4B,   K4C }, \
    { KC_NO, KC_NO, K52,   K53,   K54,   K55,   K56,   K57,   K58,   K59,   K5A,   KC_NO, K5C }, \
}
