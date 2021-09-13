#include QMK_KEYBOARD_H

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
		KC_ESC, KC_X, KC_DEL  ,
        KC_LGUI    , KC_R    , KC_M,
        MO(1)    , LSA(KC_Z)    , A(KC_Z)
    ),

    [1] = LAYOUT(RGB_RMOD, RGB_VAI, RGB_MOD,
               RGB_HUI, RGB_HUD, RGB_SAI,
               KC_TRNS, RGB_VAD, RGB_SAD),
};
