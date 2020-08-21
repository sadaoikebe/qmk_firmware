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
 For XD64 VER3.0 split-spacebar
 * (C) 2020 Sadao Ikebe @bonyarou
 */

#include QMK_KEYBOARD_H
#include "bootloader.h"

#include "nicola.h" // NICOLA親指シフト
#include "jtu.h"    // JIS keyboard on ANSI layout hardware

// Each layer gets a name for readability, which is then used in the keymap matrix below.
// The underscores don't mean anything - you can have a layer called STUFF or any other name.
// Layer names don't all need to be of the same length, obviously, and you can also skip them
// entirely and just use numbers.
enum keymap_layers {
  _QWERTY,
// NICOLA親指シフト
  _NICOLA, // NICOLA親指シフト入力レイヤー
// NICOLA親指シフト
  _FUNC
};

enum custom_keycodes {
  KC_EISU = JTU_SAFE_RANGE,
  KC_KANA2
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT_62_ansi_2space( \
      KC_ESC,  KC_1,    JU_2,    KC_3,    KC_4,   KC_5,   JU_6,    JU_7,   JU_8,    JU_9,    JU_0,    JU_MINS,  JU_EQL,  KC_BSPC, \
      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,   KC_T,   KC_Y,    KC_U,   KC_I,    KC_O,    KC_P,    JU_LBRC,  JU_RBRC, JU_BSLS, \
      KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,   KC_G,   KC_H,    KC_J,   KC_K,    KC_L,    JU_SCLN, JU_QUOT,  KC_ENT,           \
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,   KC_B,   KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,                    \
      MO(_FUNC),   KC_LGUI, KC_LALT,                      KC_SPC,  KC_SPC,                   KC_RALT, KC_RGUI, LT(_FUNC, KC_APP), KC_RCTL ),

// NICOLA親指シフト
  // デフォルトレイヤーに関係なくQWERTYで
    [_NICOLA] = LAYOUT_62_ansi_2space( \
      KC_ESC,  NG_1,    NG_2,    NG_3,    NG_4,   NG_5,     NG_6,    NG_7,   NG_8,    NG_9,    NG_0,    NG_MINS,  NG_EQL,  KC_BSPC, \
      KC_TAB,  NG_Q,    NG_W,    NG_E,    NG_R,   NG_T,     NG_Y,    NG_U,   NG_I,    NG_O,    NG_P,    NG_LBRC,  NG_RBRC, NG_BSLS, \
      KC_LCTL, NG_A,    NG_S,    NG_D,    NG_F,   NG_G,     NG_H,    NG_J,   NG_K,    NG_L,    NG_SCLN, NG_QUOT,  KC_ENT,           \
      KC_LSFT, NG_Z,    NG_X,    NG_C,    NG_V,   NG_B,     NG_N,    NG_M,   NG_COMM, NG_DOT,  NG_SLSH, KC_RSFT,                    \
      MO(_FUNC), KC_LGUI, KC_LALT,                         NG_SHFTL, NG_SHFTR,                 KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS  ),

  // Function Layer
  [_FUNC] = LAYOUT_62_ansi_2space( \
      JU_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,  KC_F5,   KC_F6,    KC_F7,   KC_F8,   KC_F9,    KC_F10,  KC_F11,  KC_F12,  KC_DEL, \
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_HOME,  KC_PGDN, KC_PGUP, KC_END,   KC_PSCR, KC_TRNS, KC_BRK,  KC_TRNS,\
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_LEFT,  KC_DOWN, KC_UP,   KC_RIGHT, KC_INS,  KC_DEL,  KC_ENT,          \
      KC_LSFT, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS,                  \
      MO(_FUNC), KC_TRNS, KC_TRNS,                         KC_EISU,  KC_KANA2,                   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS )
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
        // NICOLA親指シフト
        send_string(SS_TAP(X_MHEN)); // Win
        send_string(SS_TAP(X_LANG2)); // Mac
        nicola_off();
        // NICOLA親指シフト
      }
      return false;
      break;
    case KC_KANA2:
      if (record->event.pressed) {
        // NICOLA親指シフト
        send_string(SS_TAP(X_HENK)); // Win
        send_string(SS_TAP(X_LANG1)); // Mac
        nicola_on();
        // NICOLA親指シフト
      }
      return false;
      break;
  }

  // NICOLA親指シフト
  bool a = true;
  if (nicola_state()) {
    nicola_mode(keycode, record);
    a = process_nicola(keycode, record);
  }
  if (a == false) return false;
  // NICOLA親指シフト

  bool continue_process = process_jtu(keycode, record);
  if (continue_process == false) {
    return false;
  }

  return true;
}
