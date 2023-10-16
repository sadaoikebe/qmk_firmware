#pragma once
/* Copyright 2018-2019 eswai <@eswai>
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

void nicola_type(void);
void nicola_clear(void);

void nicola_mode(uint16_t, keyrecord_t *);

void nicola_on(void);
void nicola_off(void);
bool nicola_state(void);

bool process_nicola(uint16_t, keyrecord_t *);
void set_nicola(uint8_t);

typedef enum nicola_keycodes {
  NG_TOP = SAFE_RANGE,
  NG_M_TOP = NG_TOP,

  NG_E_TAB = NG_TOP,
  NG_LEFT_FINGER_TOP = NG_E_TAB,

  NG_E_Q,
  NG_E_W,
  NG_E_E,
  NG_E_R,
  NG_E_T,

  NG_E_A,
  NG_E_S,
  NG_E_D,
  NG_E_F,
  NG_E_G,

  NG_E_Z,
  NG_E_X,
  NG_E_C,
  NG_E_V,
  NG_E_B,

  NG_Q,
  NG_W,
  NG_E,
  NG_R,
  NG_T,

  NG_A,
  NG_S,
  NG_D,
  NG_F,
  NG_G,

  NG_Z,
  NG_X,
  NG_C,
  NG_V,
  NG_B,

  NG_LEFT_FINGER_BOTTOM = NG_B,

  NG_E_Y,
  NG_RIGHT_FINGER_TOP = NG_E_Y,
  NG_E_U,
  NG_E_I,
  NG_E_O,
  NG_E_P,
  NG_E_LBRC,
  NG_E_RBRC,
  NG_E_BSLS,

  NG_E_H,
  NG_E_J,
  NG_E_K,
  NG_E_L,
  NG_E_SCLN,
  NG_E_QUOT,

  NG_E_N,
  NG_E_M,
  NG_E_COMM,
  NG_E_DOT,
  NG_E_SLSH,

  NG_Y,
  NG_U,
  NG_I,
  NG_O,
  NG_P,
  NG_LBRC,
  NG_RBRC,
  NG_BSLS,

  NG_H,
  NG_J,
  NG_K,
  NG_L,
  NG_SCLN,
  NG_QUOT,

  NG_N,
  NG_M,
  NG_COMM,
  NG_DOT,
  NG_SLSH,

  NG_M_BOTTOM = NG_SLSH,
  NG_RIGHT_FINGER_BOTTOM = NG_SLSH,

  NG_SHFTL,
  NG_SHFTR,

  NG_BOTTOM = NG_SHFTR,
} NGKEYS;

#define NG_SAFE_RANGE (NG_BOTTOM+1)


