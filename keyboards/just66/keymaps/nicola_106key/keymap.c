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
// NICOLA親指シフト
#include "nicola.h"
NGKEYS nicola_keys;
// NICOLA親指シフト
#include <print.h>

void update_led(void);

extern keymap_config_t keymap_config;

extern uint8_t is_master;

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
  KC_EISU = NG_SAFE_RANGE,
  KC_KANA2
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT( \
      KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,   KC_5,   KC_6,    KC_7,   KC_8,    KC_9,    KC_0,    KC_MINS,  KC_EQL,  KC_BSPC, KC_HOME, \
      KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,   KC_T,   KC_Y,    KC_U,   KC_I,    KC_O,    KC_P,    KC_LBRC,  KC_RBRC, KC_BSLS, KC_PGUP, \
      KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,   KC_G,   KC_H,    KC_J,   KC_K,    KC_L,    KC_SCLN, KC_QUOT,           KC_ENT,  KC_PGDN, \
      KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,   KC_B,   KC_N,    KC_M,   KC_COMM, KC_DOT,  KC_SLSH,           KC_RSFT, KC_UP,   KC_END, \
      MO(_FUNC), KC_LGUI, KC_LALT,        KC_SPC,         KC_SPC,          KC_RALT, KC_RGUI, LT(_FUNC, KC_APP), KC_LEFT, KC_DOWN, KC_RGHT ),

// NICOLA親指シフト
  // デフォルトレイヤーに関係なくQWERTYで
    [_NICOLA] = LAYOUT( \
      KC_ESC,  NG_1,    NG_2,    NG_3,    NG_4,   NG_5,     NG_6,    NG_7,   NG_8,    NG_9,    NG_0,    NG_MINS, NG_EQL,  KC_TRNS, KC_TRNS, \
      KC_TAB,  NG_Q,    NG_W,    NG_E,    NG_R,   NG_T,     NG_Y,    NG_U,   NG_I,    NG_O,    NG_P,    NG_LBRC, NG_RBRC, NG_BSLS, KC_TRNS, \
      KC_LCTL, NG_A,    NG_S,    NG_D,    NG_F,   NG_G,     NG_H,    NG_J,   NG_K,    NG_L,    NG_SCLN, NG_QUOT,          KC_TRNS, KC_TRNS, \
      KC_LSFT, NG_Z,    NG_X,    NG_C,    NG_V,   NG_B,     NG_N,    NG_M,   NG_COMM, NG_DOT,  NG_SLSH,          KC_TRNS, KC_TRNS, KC_TRNS, \
      MO(_FUNC), KC_LGUI, KC_LALT,        NG_SHFTL,         NG_SHFTR,                 KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS  ),

  // Function Layer
  [_FUNC] = LAYOUT( \
      KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,    KC_F5,   KC_F6,    KC_F7,   KC_F8,   KC_F9,    KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_TRNS, \
      KC_TRNS, KC_KP_7, KC_KP_8, KC_KP_9, KC_NLCK,  KC_TRNS, KC_HOME,  KC_PGDN, KC_PGUP, KC_END,   KC_PSCR, KC_TRNS, KC_BRK,  KC_TRNS, KC_TRNS, \
      KC_TRNS, KC_KP_4, KC_KP_5, KC_KP_6, KC_PPLS,  KC_TRNS, KC_LEFT,  KC_DOWN, KC_UP,   KC_RIGHT, KC_INS,  KC_DEL,           KC_ENT,  KC_TRNS, \
      KC_LSFT, KC_KP_1, KC_KP_2, KC_KP_3, KC_KP_0,  KC_TRNS, KC_TRNS,  KC_TRNS, KC_TRNS, KC_TRNS,  KC_TRNS,          KC_TRNS, KC_TRNS, KC_TRNS, \
      MO(_FUNC), KC_NO, LSFT(KC_SPC),     KC_EISU,           KC_KANA2,          LSFT(KC_SPC),      KC_MHEN, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS )
        // Fn+Altに Shift + Space (Androidでの言語変更キー) をマップ
        // モードずれ対策のため Fn + RWin に 無変換単独打鍵(just in case)
};

int RGB_current_mode;

void persistent_default_layer_set(uint16_t default_layer) {
  eeconfig_update_default_layer(default_layer);
  default_layer_set(default_layer);
}

// Setting ADJUST layer RGB back to default
void update_tri_layer_RGB(uint8_t layer1, uint8_t layer2, uint8_t layer3) {
  if (IS_LAYER_ON(layer1) && IS_LAYER_ON(layer2)) {
    layer_on(layer3);
  } else {
    layer_off(layer3);
  }
}

void matrix_init_user(void) {
  // NICOLA親指シフト
  set_nicola(_NICOLA);
  // NICOLA親指シフト
}

void update_led() {
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case KC_EISU:
      if (record->event.pressed) {
        // NICOLA親指シフト
        // 無変換3回送出は Android 対応のため。
        // Androidでは 無変換キーでひらがな → カタカナ → 半角カタカナと遷移する
        // しかしこの機能は不要 & Windowsでの動作を変えたくないため無変換3回送出でやり過ごすことにする
        send_string(SS_TAP(X_MHEN) SS_TAP(X_MHEN) SS_TAP(X_MHEN));
        nicola_off();
        // NICOLA親指シフト
        update_led();
      }
      return false;
      break;
    case KC_KANA2:
      if (record->event.pressed) {
        // NICOLA親指シフト
        send_string(SS_TAP(X_HENK));
        nicola_on();
        // NICOLA親指シフト
        update_led();
      }
      return false;
      break;
  }

  // NICOLA親指シフト
  bool a = true;
  if (nicola_state()) {
    nicola_mode(keycode, record);
    a = process_nicola(keycode, record);
    update_led();
  }
  if (a == false) return false;
  // NICOLA親指シフト

  return true;
}
