/* TS52K — NICOLA 親指シフト
 *
 * Copyright 2018-2019 eswai <@eswai>
 * Copyright 2020-2026 Sadao Ikebe @bonyarou
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * 状態機械は users/nicola/ にある (TS69 V3 と共有するため)。
 * このファイルが持つのはレイアウトとゲートだけ。
 */

#include QMK_KEYBOARD_H

#include "nicola.h"

enum keymap_layers {
    _QWERTY,
    _NICOLA,
    _FUNC,
};

enum custom_keycodes {
    KANA_OFF = NG_SAFE_RANGE, /* 英数へ */
    KANA_ON,                  /* かなへ */
};

/* clang-format off */
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* 英数レイヤー。
     * 全キーが NG_E_* なので、英数でも同時打鍵判定を通る。
     * 親指シフト同側で数字・F キー、クロスシフトでその Shift 付きが出る。 */
    [_QWERTY] = LAYOUT(
        NG_E_TAB,  NG_E_Q,  NG_E_W,  NG_E_E,  NG_E_R,  NG_E_T,  NG_E_Y,  NG_E_U,  NG_E_I,  NG_E_O,    NG_E_P,    NG_E_LBRC, NG_E_RBRC, NG_E_BSLS, KC_NO,
        KC_LCTL,   NG_E_A,  NG_E_S,  NG_E_D,  NG_E_F,  NG_E_G,  NG_E_H,  NG_E_J,  NG_E_K,  NG_E_L,    NG_E_SCLN, NG_E_QUOT,            KC_ENT,    KC_ESC,
        KC_LSFT,            NG_E_Z,  NG_E_X,  NG_E_C,  NG_E_V,  NG_E_B,  NG_E_N,  NG_E_M,  NG_E_COMM, NG_E_DOT,  NG_E_SLSH,            KC_RSFT,   NG_UP,
        MO(_FUNC), KC_LGUI, KC_LALT,          NG_SHFTL,                  NG_SHFTR,         KC_RALT,   KC_RGUI,   KC_APP,    NG_LEFT,   NG_DOWN,   NG_RIGHT
    ),

    /* かなレイヤー */
    [_NICOLA] = LAYOUT(
        KC_TRNS,   NG_Q,    NG_W,    NG_E,    NG_R,    NG_T,    NG_Y,    NG_U,    NG_I,    NG_O,      NG_P,      NG_LBRC,   NG_RBRC,   NG_BSLS,   KC_TRNS,
        KC_TRNS,   NG_A,    NG_S,    NG_D,    NG_F,    NG_G,    NG_H,    NG_J,    NG_K,    NG_L,      NG_SCLN,   NG_QUOT,              KC_TRNS,   KC_TRNS,
        KC_TRNS,            NG_Z,    NG_X,    NG_C,    NG_V,    NG_B,    NG_N,    NG_M,    NG_COMM,   NG_DOT,    NG_SLSH,              KC_RSFT,   KC_TRNS,
        KC_TRNS,   KC_TRNS, KC_TRNS,          NG_SHFTL,                  NG_SHFTR,         KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS
    ),

    [_FUNC] = LAYOUT(
        KC_ESC,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_HOME, KC_PGDN, KC_PGUP, KC_END,    KC_PSCR,   KC_TRNS,   KC_BRK,    KC_DEL,    KC_TRNS,
        KC_TRNS,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_LEFT, KC_DOWN, KC_UP,   KC_RIGHT,  KC_INS,    KC_DEL,               KC_TRNS,   KC_TRNS,
        KC_TRNS,            KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,   KC_TRNS,   KC_TRNS,              KC_TRNS,   KC_TRNS,
        KC_TRNS,   KC_TRNS, KC_TRNS,          KANA_OFF,                  KANA_ON,          KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS,   KC_TRNS
    ),
};
/* clang-format on */

void keyboard_post_init_user(void) {
    set_nicola(_NICOLA);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    /* かな ON/OFF。IME 切り替えキーを Windows / Mac 両方に送る。
     * press と release を物理キーに対応させる (原則1) ため tap_code は使わない。
     *
     * 旧 KC_MHEN (無変換) / KC_HENK (変換) は現行 QMK で名前が変わった。
     * HID usage は同じ: 無変換 = 0x8B = KC_INT5、変換 = 0x8A = KC_INT4 */
    switch (keycode) {
        case KANA_OFF:
            if (record->event.pressed) {
                nicola_off();
                register_code(KC_INT5);  /* Windows 無変換 */
                register_code(KC_LNG2);  /* Mac 英数 */
            } else {
                unregister_code(KC_LNG2);
                unregister_code(KC_INT5);
            }
            return false;

        case KANA_ON:
            if (record->event.pressed) {
                nicola_on();
                register_code(KC_INT4);  /* Windows 変換 */
                register_code(KC_LNG1);  /* Mac かな */
            } else {
                unregister_code(KC_LNG1);
                unregister_code(KC_INT4);
            }
            return false;

        default:
            break;
    }

    /* [ゲート1] TS52K は英数でも同時打鍵判定を通すので、ここにガードを置かない。
     * 英数レイヤーが NG_E_* を使っているため、通さないと何も出なくなる。
     *
     * TS69 V3 のように「英数では判定を一切使わない」構成にする場合は、
     * 以下 2 行を if (nicola_state()) { ... } で囲むだけでよい。 */
    nicola_mode(keycode, record);
    return process_nicola(keycode, record);
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    /* エンコーダには物理的な release が存在しないので tap_code でよい */
    if (index == 0) {
        tap_code(clockwise ? KC_PGDN : KC_PGUP);
    }
    return true;
}
