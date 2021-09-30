#include QMK_KEYBOARD_H
#include "quantum.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
		S(KC_F11), KC_F10   , KC_F11  , KC_NO,
        KC_F9    , C(KC_F10), KC_F5   , KC_NO,
        MO(1)    , KC_NO    , S(KC_F5), KC_NO,
        KC_NO    , KC_NO    , KC_NO,    KC_NO
    ),
    [1] = LAYOUT(RGB_RMOD, RGB_VAI, RGB_MOD, KC_TRNS,
               RGB_HUI, RGB_HUD, RGB_SAI,    KC_TRNS,
               KC_TRNS, RGB_VAD, RGB_SAD,    KC_TRNS,
               KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS)
};

bool encoder_update_user(uint8_t index, bool clockwise) {
    if (index == 0) {
        if (clockwise) {
            tap_code(KC_DOWN);
        } else {
            tap_code(KC_UP);
        }
    } else if (index == 1) {
        if (clockwise) {
            tap_code(KC_PGDN);
        } else {
            tap_code(KC_PGUP);
        }
    } else if (index == 2) {
        if (clockwise) {
            tap_code(KC_MS_WH_UP);
        } else {
            tap_code(KC_MS_WH_DOWN);
        }
    } else if (index == 3) {
        if (clockwise) {
            tap_code(KC_MS_WH_LEFT);
        } else {
            tap_code(KC_MS_WH_RIGHT);
        }
    }
    return true;
}
