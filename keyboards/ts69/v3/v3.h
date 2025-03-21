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
 * This keyboard has electrically 5 rows 8 cols duplex matrix
 *
 * R1: K16 = 2u Backspace
 * R2: (US)K27 = |\
 * R3: (JP)K56 = ]}, (US&JP)K47 = Enter
 * R4: (JP)K66 = _
 * R5: K91/K94 = Henkan/Muhenkan, K83 = 6.25u Spacebar
 */
#define LAYOUT( \
      K00,   K10,   K01,   K11,   K02,   K12,   K03,   K13,   K04,   K14,   K05,   K15,   K06,   K16,   K07,   K17, \
      K20,   K30,   K21,   K31,   K22,   K32,   K23,   K33,   K24,   K34,   K25,   K35,   K26,          K27,   K37, \
      K40,   K50,   K41,   K51,   K42,   K52,   K43,   K53,   K44,   K54,   K45,   K46,          K56,   K47,   K57, \
      K60,          K61,   K71,   K62,   K72,   K63,   K73,   K64,   K74,   K65,   K75,   K66,   K76,   K67,   K77, \
      K80,   K90,   K81,   K91,   K82,          K83,          K84,   K94,   K85,   K95,   K86,   K96,   K87,   K97  \
) { \
    /* COL2ROW */ \
    { K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07 }, \
    { K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27 }, \
    { K40,   K41,   K42,   K43,   K44,   K45,   K46,   K47 }, \
    { K60,   K61,   K62,   K63,   K64,   K65,   K66,   K67 }, \
    { K80,   K81,   K82,   K83,   K84,   K85,   K86,   K87 }, \
    /* ROW2COL */ \
    { K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17 }, \
    { K30,   K31,   K32,   K33,   K34,   K35,   KC_NO, K37 }, \
    { K50,   K51,   K52,   K53,   K54,   KC_NO, K56,   K57 }, \
    { KC_NO, K71,   K72,   K73,   K74,   K75,   K76,   K77 }, \
    { K90,   K91,   KC_NO, KC_NO, K94,   K95,   K96,   K97 }, \
}

/*
["K00","K10","K01","K11","K02","K12","K03","K13","K04","K14","K05","K15","K06","K16","K07","K17",{x:0.25,f:5,w:6,d:true},"2u Backspace = K16"],
[{f:3,w:1.5},"K20","K30","K21","K31","K22","K32","K23","K33","K24","K34","K25","K35","K26",{w:1.5},"K27","K37",{x:0.25,f:5,w:6,d:true},"No K27 on JP layout"],
[{f:3,w:1.75},"K40","K50","K41","K51","K42","K52","K43","K53","K44","K54","K45","K46","K56",{w:1.25},"K47","K57",{x:0.25,f:5,w:6,d:true},"No K56 on US layout<p>Enter = K47"],
[{f:3,w:2},"K60","K61","K71","K62","K72","K63","K73","K64","K74","K65","K75","K66","K76","K67","K77",{x:0.25,f:5,w:6,d:true},"No K66 on US layout<p>RShift = K76"],
[{f:3,w:1.25},"K80",{w:1.25},"K90","K81","K91",{w:1.75},"K82","K83",{w:1.75},"K84","K94","K85","K95","K86","K96","K87","K97",{x:0.25,f:5,w:6,d:true},"K91/K94 only usable with 2.25u Spacebar<p>6.25u Spacebar = K83"]
*/
