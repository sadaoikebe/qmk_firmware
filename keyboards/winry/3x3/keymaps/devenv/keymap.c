#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
		S(KC_F11), KC_F10   , KC_F11  ,
        KC_F9    , C(KC_F10), KC_F5   ,
        MO(1)    , KC_NO    , S(KC_F5)
    ),

    [1] = LAYOUT(RGB_RMOD, RGB_VAI, RGB_MOD,
               RGB_HUI, RGB_HUD, RGB_SAI,
               KC_TRNS, RGB_VAD, RGB_SAD),
};
