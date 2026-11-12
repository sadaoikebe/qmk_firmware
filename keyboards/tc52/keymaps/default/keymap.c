/*
 * (C) 2020-2026 Sadao Ikebe @bonyarou
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

/*
 * NICOLA 抜きの素の QWERTY。
 * duplex matrix の配線が正しいかを、親指シフトの状態機械と切り離して
 * 検証するための keymap。全キーが期待した位置に出れば matrix.c は正しい。
 */

#include QMK_KEYBOARD_H

enum layers {
    _QWERTY,
    _FUNC,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /*
     * ,-----------------------------------------------------------------------------.
     * |Tab| Q | W | E | R | T | Y | U | I | O | P | - | = |BSpc|   |
     * |-----------------------------------------------------------------------------|
     * |Ctl| A | S | D | F | G | H | J | K | L | ; | ' | Enter |Esc|
     * |-----------------------------------------------------------------------------|
     * |Shift| Z | X | C | V | B | N | M | , | . | / |RShift| Up |
     * |-----------------------------------------------------------------------------|
     * |Fn |GUI|Alt|  SHFTL  |  SHFTR  |Alt|GUI|App| < | v | > |
     * `-----------------------------------------------------------------------------'
     */
    [_QWERTY] = LAYOUT(
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_MINS, KC_EQL,  KC_BSPC, KC_NO,
        KC_LCTL, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,          KC_ENT,  KC_ESC,
        KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,                   KC_RSFT, KC_UP,
        MO(_FUNC), KC_LGUI, KC_LALT,        KC_SPC,           KC_SPC,           KC_RALT, KC_RGUI, KC_APP,  KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [_FUNC] = LAYOUT(
        KC_ESC,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_DEL,  KC_NO,
        KC_TRNS, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_LBRC,          KC_RBRC, KC_BSLS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP, KC_END,                    KC_TRNS, KC_PGUP,
        KC_TRNS, KC_TRNS, KC_TRNS,         KC_TRNS,          KC_TRNS,           KC_TRNS, KC_TRNS, KC_INS,  KC_HOME, KC_PGDN, KC_END
    ),
};
// clang-format on

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        tap_code(clockwise ? KC_PGDN : KC_PGUP);
    }
    return true;
}
