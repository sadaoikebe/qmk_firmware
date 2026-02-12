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
/*
 For TS52K split-spacebar
 * (C) 2020 Sadao Ikebe @bonyarou
 */

#include QMK_KEYBOARD_H
#include "bootloader.h"

#include "nicola.h" // NICOLA親指シフト

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
enum keymap_layers {
  _QWERTY,
  _NICOLA,
  _FUNC
};

enum custom_keycodes {
  KC_EISU = NG_SAFE_RANGE,
  KC_KANA2
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT( \
      NG_E_TAB,  NG_E_Q, NG_E_W, NG_E_E, NG_E_R, NG_E_T, NG_E_Y, NG_E_U, NG_E_I,  NG_E_O,    NG_E_P,    NG_E_LBRC,NG_E_RBRC, NG_E_BSLS, KC_NO,  \
      KC_LCTL,   NG_E_A, NG_E_S, NG_E_D, NG_E_F, NG_E_G, NG_E_H, NG_E_J, NG_E_K,  NG_E_L,    NG_E_SCLN, NG_E_QUOT,           KC_ENT,    KC_ESC, \
      KC_LSFT,           NG_E_Z, NG_E_X, NG_E_C, NG_E_V, NG_E_B, NG_E_N, NG_E_M,  NG_E_COMM, NG_E_DOT,  NG_E_SLSH,  KC_RSFT, NG_UP,             \
      MO(_FUNC), KC_LGUI, KC_LALT,       NG_SHFTL,               NG_SHFTR,     KC_RALT, KC_RGUI,  KC_APP, NG_LEFT, NG_DOWN, NG_RIGHT ),

    [_NICOLA] = LAYOUT( \
      KC_TRNS, NG_Q,    NG_W,    NG_E,    NG_R,   NG_T,   NG_Y,    NG_U,   NG_I,     NG_O,    NG_P,    NG_LBRC,  NG_RBRC, NG_BSLS, KC_TRNS, \
      KC_TRNS, NG_A,    NG_S,    NG_D,    NG_F,   NG_G,   NG_H,    NG_J,   NG_K,     NG_L,    NG_SCLN, NG_QUOT,           KC_TRNS, KC_TRNS, \
      KC_TRNS,          NG_Z,    NG_X,    NG_C,   NG_V,   NG_B,    NG_N,   NG_M,     NG_COMM, NG_DOT,  NG_SLSH,  KC_RSFT, KC_TRNS,          \
      KC_TRNS, KC_TRNS, KC_TRNS,      NG_SHFTL,                NG_SHFTR,             KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS ),

  [_FUNC] = LAYOUT( \
      KC_ESC,  KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP,  KC_END,   KC_PSCR, KC_TRNS, KC_BRK,  KC_DEL,  KC_TRNS, \
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_LEFT, KC_DOWN, KC_UP,    KC_RIGHT, KC_INS,  KC_DEL,           KC_TRNS, KC_TRNS, \
      KC_TRNS,          KC_TRNS, KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, \
      KC_TRNS, KC_TRNS, KC_TRNS,     KC_EISU,                      KC_KANA2,       KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS )
};

void matrix_init_user(void) {
  // NICOLA親指シフト
  set_nicola(_NICOLA);
  // NICOLA親指シフト
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  switch (keycode) {
    case KC_EISU:
      if (record->event.pressed) {
        register_code(KC_MHEN); // Win
        register_code(KC_LANG2); // Mac
        nicola_off();
      } else {
        unregister_code(KC_LANG2);
        unregister_code(KC_MHEN);
      }
      return false;
      break;
    case KC_KANA2:
      if (record->event.pressed) {
        register_code(KC_HENK); // Win
        register_code(KC_LANG1); // Mac
        nicola_on();
      } else {
        unregister_code(KC_LANG1);
        unregister_code(KC_HENK);
      }
      return false;
      break;
  }

  bool a = true;
  nicola_mode(keycode, record);
  a = process_nicola(keycode, record);
  if (a == false) return false;

  return true;
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    }
    return true;
}
