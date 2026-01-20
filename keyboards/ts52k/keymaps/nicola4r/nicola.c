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
 * introduced NICOLA state machine
 * (C) 2020 Sadao Ikebe @bonyarou
 */

#include QMK_KEYBOARD_H
#include "nicola.h"
#include "key_duration.h"
#include <timer.h>

static bool is_nicola = false; // 親指シフトがオンかオフか
static uint8_t nicola_layer = 0; // レイヤー番号
static uint8_t n_modifier = 0; // 押しているmodifierキーの数

#define TIMEOUT_THRESHOLD (80)
#define OVERLAP_THRESHOLD (20)

typedef enum {
  NICOLA_STATE_S1_INIT,
  NICOLA_STATE_S2_M,
  NICOLA_STATE_S3_O,
  NICOLA_STATE_S4_MO,
  NICOLA_STATE_S5_OM
} nicola_state_t;

static nicola_state_t nicola_int_state = NICOLA_STATE_S1_INIT;
static int nicola_m_key;
static int nicola_o_key;
static uint16_t nicola_m_time;
static uint16_t nicola_o_time;
static bool nicola_m_pressed;
static bool nicola_o_pressed;
static bool nicola_om_pressed;

#define NICOLA_MAX_PENDING_EVENTS 8
typedef struct {
    uint16_t keycode;
    bool pressed;
} pending_event_t;

pending_event_t nicola_pending_events[NICOLA_MAX_PENDING_EVENTS];
uint8_t nicola_pending_count = 0;

void nicola_flush_pending_events(void) {
    for(uint8_t i=0; i<nicola_pending_count; ++i) {
        if(nicola_pending_events[i].pressed) {
            register_code16(nicola_pending_events[i].keycode);
        } else {
            unregister_code16(nicola_pending_events[i].keycode);
        }
    }
    nicola_pending_count = 0;
}

static int key_process_guard = 0;
void keypress_timer_expired(void);

void nicola_m_press(void);
void nicola_o_press(void);
void nicola_om_press(void);

void nicola_m_release(void);
void nicola_o_release(void);
void nicola_om_release(void);

// if we have independent timeout routine, no need to check timeout on key press
#ifdef TIMEOUT_INTERRUPT
#define IF_TIMEOUT(x) if(0)
#else
#define IF_TIMEOUT(x) if(x)
#endif

// 親指シフトのレイヤー、シフトキーを設定
void set_nicola(uint8_t layer) {
  nicola_layer = layer;
#ifdef TIMEOUT_INTERRUPT
  keypress_timer_init(keypress_timer_expired);
#endif
  nicola_m_key = KC_NO;
  nicola_o_key = KC_NO;
}

// 親指シフトをオンオフ
void nicola_on(void) {
  is_nicola = true;
  nicola_clear();
  layer_on(nicola_layer);
}

void nicola_off(void) {
  is_nicola = false;
  nicola_clear();
  layer_off(nicola_layer);
}

// 親指シフトの状態
bool nicola_state(void) {
  return is_nicola;
}

// バッファをクリアする
void nicola_clear(void) {
    switch(nicola_int_state) {
        case NICOLA_STATE_S1_INIT:
            break;
        case NICOLA_STATE_S2_M:
            nicola_m_release();
            break;
        case NICOLA_STATE_S3_O:
            nicola_o_release();
            break;
        case NICOLA_STATE_S4_MO:
        case NICOLA_STATE_S5_OM:
            nicola_om_release();
            break;
    }
    nicola_int_state = NICOLA_STATE_S1_INIT;
}
void nicola_trigger(void) {
    switch(nicola_int_state) {
        case NICOLA_STATE_S1_INIT:
            break;
        case NICOLA_STATE_S2_M:
            nicola_m_press();
            break;
        case NICOLA_STATE_S3_O:
            nicola_o_press();
            break;
        case NICOLA_STATE_S4_MO:
        case NICOLA_STATE_S5_OM:
            nicola_om_press();
            break;
    }
}

// 入力モードか編集モードかを確認する
void nicola_mode(uint16_t keycode, keyrecord_t *record) {
  if (!is_nicola) return;

  // modifierが押されたらレイヤーをオフ
  switch (keycode) {
    case KC_LCTRL:
    case KC_LSHIFT:
    case KC_LALT:
    case KC_LGUI:
    case KC_RCTRL:
    case KC_RSHIFT:
    case KC_RALT:
    case KC_RGUI:
      if (record->event.pressed) {
        if(n_modifier == 0) {
          layer_off(nicola_layer);
        }
        n_modifier++;
      } else {
        n_modifier--;
        if (n_modifier == 0) {
          layer_on(nicola_layer);
        }
      }
      break;
  }

}

#define SS_ALNUM(x) SS_TAP(X_CAPSLOCK) x SS_TAP(X_CAPSLOCK)

bool is_nicola_m_key(uint16_t keycode) {
    return NG_M_TOP <= keycode && keycode <= NG_M_BOTTOM;
}

bool is_nicola_o_key(uint16_t keycode) {
    return keycode == NG_SHFTL || keycode == NG_SHFTR;
}

bool is_nicola_left_finger(uint16_t keycode) {
    return NG_LEFT_FINGER_TOP <= keycode && keycode <= NG_LEFT_FINGER_BOTTOM;
}

bool is_nicola_right_finger(uint16_t keycode) {
    return NG_RIGHT_FINGER_TOP <= keycode && keycode <= NG_RIGHT_FINGER_BOTTOM;
}

bool is_cross_shift(uint16_t m, uint16_t o) {
    return (is_nicola_left_finger(m) && o == NG_SHFTR) || (is_nicola_right_finger(m) && o == NG_SHFTL);
}

bool is_nicola_eisu(uint16_t keycode) {
    return (NG_EISU1_TOP <= keycode && keycode <= NG_EISU1_BOTTOM) || (NG_EISU2_TOP <= keycode && keycode <= NG_EISU2_BOTTOM);
}

void nicola_m_press(void) {
    if(nicola_m_pressed) {
        nicola_m_release();
    }
    if(nicola_o_pressed) {
        nicola_o_release();
    }
    if(nicola_om_pressed) {
        nicola_om_release();
    }
    switch(nicola_m_key) {
        case NG_Q   : register_code(KC_DOT); break; // "."
        case NG_W   : register_code(KC_K);register_code(KC_A); break; // "ka"
        case NG_E   : register_code(KC_T);register_code(KC_A); break; // "ta"
        case NG_R   : register_code(KC_K);register_code(KC_O); break; // "ko"
        case NG_T   : register_code(KC_S);register_code(KC_A); break; // "sa"
        case NG_Y   : register_code(KC_R);register_code(KC_A); break; // "ra"
        case NG_U   : register_code(KC_T);register_code(KC_I); break; // "ti"
        case NG_I   : register_code(KC_K);register_code(KC_U); break; // "ku"
        case NG_O   : register_code(KC_T);register_code(KC_U); break; // "tu"
        case NG_P   : register_code(KC_COMM); break; // ","
        case NG_LBRC: register_code(KC_COMM); break; // ","
        case NG_RBRC: register_code(KC_SCLN); break; // ";"
        case NG_BSLS: register_code(KC_BSPC); break;

        case NG_A   : register_code(KC_U); break; // "u"
        case NG_S   : register_code(KC_S);register_code(KC_I); break; // "si"
        case NG_D   : register_code(KC_T);register_code(KC_E); break; // "te"
        case NG_F   : register_code(KC_K);register_code(KC_E); break; // "ke"
        case NG_G   : register_code(KC_S);register_code(KC_E); break; // "se"
        case NG_H   : register_code(KC_H);register_code(KC_A); break; // "ha"
        case NG_J   : register_code(KC_T);register_code(KC_O); break; // "to"
        case NG_K   : register_code(KC_K);register_code(KC_I); break; // "ki"
        case NG_L   : register_code(KC_I); break; // "i"
        case NG_SCLN: register_code(KC_N);register_code(KC_N); break; // "nn"
        case NG_QUOT: register_code(KC_BSPC); break;

        case NG_Z   : register_code(KC_DOT); break; // "."
        case NG_X   : register_code(KC_H);register_code(KC_I); break; // "hi"
        case NG_C   : register_code(KC_S);register_code(KC_U); break; // "su"
        case NG_V   : register_code(KC_H);register_code(KC_U); break; // "hu"
        case NG_B   : register_code(KC_H);register_code(KC_E); break; // "he"
        case NG_N   : register_code(KC_M);register_code(KC_E); break; // "me"
        case NG_M   : register_code(KC_S);register_code(KC_O); break; // "so"
        case NG_COMM: register_code(KC_N);register_code(KC_E); break; // "ne"
        case NG_DOT : register_code(KC_H);register_code(KC_O); break; // "ho"
        case NG_SLSH: register_code(KC_SLSH); break; // "/"

        case NG_E_TAB   : register_code(KC_TAB); break;

        case NG_E_Q   : register_code(KC_Q); break;
        case NG_E_W   : register_code(KC_W); break;
        case NG_E_E   : register_code(KC_E); break;
        case NG_E_R   : register_code(KC_R); break;
        case NG_E_T   : register_code(KC_T); break;
        case NG_E_Y   : register_code(KC_Y); break;
        case NG_E_U   : register_code(KC_U); break;
        case NG_E_I   : register_code(KC_I); break;
        case NG_E_O   : register_code(KC_O); break;
        case NG_E_P   : register_code(KC_P); break;
        case NG_E_LBRC: register_code(KC_MINS); break;
        case NG_E_RBRC: register_code(KC_EQL);  break;
        case NG_E_BSLS: register_code(KC_BSPC); break;

        case NG_E_A   : register_code(KC_A); break;
        case NG_E_S   : register_code(KC_S); break;
        case NG_E_D   : register_code(KC_D); break;
        case NG_E_F   : register_code(KC_F); break;
        case NG_E_G   : register_code(KC_G); break;
        case NG_E_H   : register_code(KC_H); break;
        case NG_E_J   : register_code(KC_J); break;
        case NG_E_K   : register_code(KC_K); break;
        case NG_E_L   : register_code(KC_L); break;
        case NG_E_SCLN: register_code(KC_SCLN); break;
        case NG_E_QUOT: register_code(KC_QUOT); break;

        case NG_E_Z   : register_code(KC_Z   ); break;
        case NG_E_X   : register_code(KC_X   ); break;
        case NG_E_C   : register_code(KC_C   ); break;
        case NG_E_V   : register_code(KC_V   ); break;
        case NG_E_B   : register_code(KC_B   ); break;
        case NG_E_N   : register_code(KC_N   ); break;
        case NG_E_M   : register_code(KC_M   ); break;
        case NG_E_COMM: register_code(KC_COMM); break;
        case NG_E_DOT : register_code(KC_DOT ); break;
        case NG_E_SLSH: register_code(KC_SLSH); break;
    }
    nicola_m_pressed = true;
    nicola_flush_pending_events();
}

void nicola_m_release(void) {
    if(!nicola_m_pressed && is_nicola_m_key(nicola_m_key)) {
        nicola_m_press();
    }
    switch(nicola_m_key) {
        case NG_Q   : unregister_code(KC_DOT); break; // "."
        case NG_W   : unregister_code(KC_K);unregister_code(KC_A); break; // "ka"
        case NG_E   : unregister_code(KC_T);unregister_code(KC_A); break; // "ta"
        case NG_R   : unregister_code(KC_K);unregister_code(KC_O); break; // "ko"
        case NG_T   : unregister_code(KC_S);unregister_code(KC_A); break; // "sa"
        case NG_Y   : unregister_code(KC_R);unregister_code(KC_A); break; // "ra"
        case NG_U   : unregister_code(KC_T);unregister_code(KC_I); break; // "ti"
        case NG_I   : unregister_code(KC_K);unregister_code(KC_U); break; // "ku"
        case NG_O   : unregister_code(KC_T);unregister_code(KC_U); break; // "tu"
        case NG_P   : unregister_code(KC_COMM); break; // ","
        case NG_LBRC: unregister_code(KC_COMM); break; // ","
        case NG_RBRC: unregister_code(KC_SCLN); break; // ";"
        case NG_BSLS: unregister_code(KC_BSPC); break;

        case NG_A   : unregister_code(KC_U); break; // "u"
        case NG_S   : unregister_code(KC_S);unregister_code(KC_I); break; // "si"
        case NG_D   : unregister_code(KC_T);unregister_code(KC_E); break; // "te"
        case NG_F   : unregister_code(KC_K);unregister_code(KC_E); break; // "ke"
        case NG_G   : unregister_code(KC_S);unregister_code(KC_E); break; // "se"
        case NG_H   : unregister_code(KC_H);unregister_code(KC_A); break; // "ha"
        case NG_J   : unregister_code(KC_T);unregister_code(KC_O); break; // "to"
        case NG_K   : unregister_code(KC_K);unregister_code(KC_I); break; // "ki"
        case NG_L   : unregister_code(KC_I); break; // "i"
        case NG_SCLN: unregister_code(KC_N);unregister_code(KC_N); break; // "nn"
        case NG_QUOT: unregister_code(KC_BSPC); break;

        case NG_Z   : unregister_code(KC_DOT); break; // "."
        case NG_X   : unregister_code(KC_H);unregister_code(KC_I); break; // "hi"
        case NG_C   : unregister_code(KC_S);unregister_code(KC_U); break; // "su"
        case NG_V   : unregister_code(KC_H);unregister_code(KC_U); break; // "hu"
        case NG_B   : unregister_code(KC_H);unregister_code(KC_E); break; // "he"
        case NG_N   : unregister_code(KC_M);unregister_code(KC_E); break; // "me"
        case NG_M   : unregister_code(KC_S);unregister_code(KC_O); break; // "so"
        case NG_COMM: unregister_code(KC_N);unregister_code(KC_E); break; // "ne"
        case NG_DOT : unregister_code(KC_H);unregister_code(KC_O); break; // "ho"
        case NG_SLSH: unregister_code(KC_SLSH); break; // "/"

        case NG_E_TAB   : unregister_code(KC_TAB); break;

        case NG_E_Q   : unregister_code(KC_Q); break;
        case NG_E_W   : unregister_code(KC_W); break;
        case NG_E_E   : unregister_code(KC_E); break;
        case NG_E_R   : unregister_code(KC_R); break;
        case NG_E_T   : unregister_code(KC_T); break;
        case NG_E_Y   : unregister_code(KC_Y); break;
        case NG_E_U   : unregister_code(KC_U); break;
        case NG_E_I   : unregister_code(KC_I); break;
        case NG_E_O   : unregister_code(KC_O); break;
        case NG_E_P   : unregister_code(KC_P); break;
        case NG_E_LBRC: unregister_code(KC_MINS); break;
        case NG_E_RBRC: unregister_code(KC_EQL);  break;
        case NG_E_BSLS: unregister_code(KC_BSPC); break;

        case NG_E_A   : unregister_code(KC_A); break;
        case NG_E_S   : unregister_code(KC_S); break;
        case NG_E_D   : unregister_code(KC_D); break;
        case NG_E_F   : unregister_code(KC_F); break;
        case NG_E_G   : unregister_code(KC_G); break;
        case NG_E_H   : unregister_code(KC_H); break;
        case NG_E_J   : unregister_code(KC_J); break;
        case NG_E_K   : unregister_code(KC_K); break;
        case NG_E_L   : unregister_code(KC_L); break;
        case NG_E_SCLN: unregister_code(KC_SCLN); break;
        case NG_E_QUOT: unregister_code(KC_QUOT); break;

        case NG_E_Z   : unregister_code(KC_Z   ); break;
        case NG_E_X   : unregister_code(KC_X   ); break;
        case NG_E_C   : unregister_code(KC_C   ); break;
        case NG_E_V   : unregister_code(KC_V   ); break;
        case NG_E_B   : unregister_code(KC_B   ); break;
        case NG_E_N   : unregister_code(KC_N   ); break;
        case NG_E_M   : unregister_code(KC_M   ); break;
        case NG_E_COMM: unregister_code(KC_COMM); break;
        case NG_E_DOT : unregister_code(KC_DOT ); break;
        case NG_E_SLSH: unregister_code(KC_SLSH); break;
    }
    nicola_m_pressed = false;
    nicola_m_key = KC_NO;
    nicola_flush_pending_events();
}

void nicola_o_press(void) {
    if(nicola_m_pressed) {
        nicola_m_release();
    }
    if(nicola_o_pressed) {
        nicola_o_release();
    }
    if(nicola_om_pressed) {
        nicola_om_release();
    }

    if(is_nicola_o_key(nicola_o_key)){
        register_code(KC_SPC);
    }
    nicola_o_pressed = true;
    nicola_flush_pending_events();
}

void nicola_o_release(void) {
    if(!nicola_o_pressed && is_nicola_o_key(nicola_o_key)) {
        nicola_o_press();
    }
    if(nicola_o_key != 0) {
        unregister_code(KC_SPC);
    }
    nicola_o_pressed = false;
    nicola_o_key = KC_NO;
    nicola_flush_pending_events();
}

void nicola_om_press(void) {
    if(nicola_m_pressed) {
        nicola_m_release();
    }
    if(nicola_o_pressed) {
        nicola_o_release();
    }
    if(nicola_om_pressed) {
        nicola_om_release();
    }

    uint8_t saved_mods = 0;
    bool invert_pinky_shift = false;

    if(is_nicola_eisu(nicola_m_key)) {
        bool eisu_cross_shift = is_cross_shift(nicola_m_key, nicola_o_key);

        // 英数モード: クロスシフトで Shift+1(=!) や Shift+F7 つまり invert_pinky_shift = eisu_cross_shift
        // Nicolaモード: 英数キーがくるということはすでに小指Shiftが押されている。
        //    同側シフトで小指シフトリリースイベント  クロスシフトでは何もしない    invert_pinky_shift = !eisu_cross_shift
        invert_pinky_shift = (eisu_cross_shift != is_nicola);
    } else {
        if(nicola_o_key == NG_SHFTL) {
            switch(nicola_m_key) {
            case NG_LBRC:
            case NG_RBRC:
            case NG_SLSH:
                invert_pinky_shift = true;
                break;
            }
        }
    }

    if(invert_pinky_shift) {
        saved_mods = get_mods();
        uint8_t new_mods = saved_mods;
        uint8_t shift_bits = MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT);
        if (saved_mods & shift_bits) {
            new_mods &= ~shift_bits;
        } else {
            new_mods |= MOD_BIT(KC_LSFT);
        }
        set_mods(new_mods);
    }

    switch(nicola_m_key) {
        case NG_E_TAB   : register_code(KC_GRV); break;

        case NG_E_Q   : register_code(KC_F1); break;
        case NG_E_W   : register_code(KC_F2); break;
        case NG_E_E   : register_code(KC_F3); break;
        case NG_E_R   : register_code(KC_F4); break;
        case NG_E_T   : register_code(KC_F5); break;
        case NG_E_Y   : register_code(KC_F6); break;
        case NG_E_U   : register_code(KC_F7); break;
        case NG_E_I   : register_code(KC_F8); break;
        case NG_E_O   : register_code(KC_F9); break;
        case NG_E_P   : register_code(KC_F10); break;
        case NG_E_LBRC: register_code(KC_F11); break;
        case NG_E_RBRC: register_code(KC_F12); break;
        case NG_E_BSLS: register_code(KC_DEL); break;

        case NG_E_A   : register_code(KC_1); break;
        case NG_E_S   : register_code(KC_2); break;
        case NG_E_D   : register_code(KC_3); break;
        case NG_E_F   : register_code(KC_4); break;
        case NG_E_G   : register_code(KC_5); break;
        case NG_E_H   : register_code(KC_6); break;
        case NG_E_J   : register_code(KC_7); break;
        case NG_E_K   : register_code(KC_8); break;
        case NG_E_L   : register_code(KC_9); break;
        case NG_E_SCLN: register_code(KC_0); break;
        case NG_E_QUOT: register_code(KC_BSPC);break;

        case NG_E_Z   : break;
        case NG_E_X   : break;
        case NG_E_C   : break;
        case NG_E_V   : break;
        case NG_E_B   : break;
        case NG_E_N   : break;
        case NG_E_M   : break;
        case NG_E_COMM: register_code(KC_LBRC);break;
        case NG_E_DOT : register_code(KC_RBRC);break;
        case NG_E_SLSH: register_code(KC_BSLS);break;
    }

    if(nicola_o_key == NG_SHFTL) {
        switch(nicola_m_key) {
          case NG_Q   : register_code(KC_X);register_code(KC_A); break; // "xa"
          case NG_W   : register_code(KC_E);                     break; // "e"
          case NG_E   : register_code(KC_R);register_code(KC_I); break; // "ri"
          case NG_R   : register_code(KC_X);register_code(KC_Y);register_code(KC_A); break; // "xya"
          case NG_T   : register_code(KC_R);register_code(KC_E); break; // "re"
          case NG_Y   : register_code(KC_P);register_code(KC_A); break; // "pa"
          case NG_U   : register_code(KC_D);register_code(KC_I); break; // "di"
          case NG_I   : register_code(KC_G);register_code(KC_U); break; // "gu"
          case NG_O   : register_code(KC_D);register_code(KC_U); break; // "du"
          case NG_P   : register_code(KC_P);register_code(KC_I); break; // "pi"
          case NG_LBRC: register_code(KC_MINS); break; // "_" invert_pinky_shift
          case NG_RBRC: register_code(KC_EQL);  break; // "+" invert_pinky_shift
          case NG_BSLS: register_code(KC_DEL);  break;

          case NG_A   : register_code(KC_W);register_code(KC_O); break; // "wo"
          case NG_S   : register_code(KC_A);                     break; // "a"
          case NG_D   : register_code(KC_N);register_code(KC_A); break; // "na"
          case NG_F   : register_code(KC_X);register_code(KC_Y);register_code(KC_U); break; // "xyu"
          case NG_G   : register_code(KC_M);register_code(KC_O); break; // "mo"
          case NG_H   : register_code(KC_B);register_code(KC_A); break; // "ba"
          case NG_J   : register_code(KC_D);register_code(KC_O); break; // "do"
          case NG_K   : register_code(KC_G);register_code(KC_I); break; // "gi"
          case NG_L   : register_code(KC_P);register_code(KC_O); break; // "po"
          case NG_SCLN:                         break;
          case NG_QUOT: register_code(KC_BSPC); break;

          case NG_Z   : register_code(KC_X);register_code(KC_U); break; // "xu"
          case NG_X   : register_code(KC_MINS);                  break; // "-"
          case NG_C   : register_code(KC_R);register_code(KC_O); break; // "ro"
          case NG_V   : register_code(KC_Y);register_code(KC_A); break; // "ya"
          case NG_B   : register_code(KC_X);register_code(KC_I); break; // "xi"
          case NG_N   : register_code(KC_P);register_code(KC_U); break; // "pu"
          case NG_M   : register_code(KC_Z);register_code(KC_O); break; // "zo"
          case NG_COMM: register_code(KC_P);register_code(KC_E); break; // "pe"
          case NG_DOT : register_code(KC_B);register_code(KC_O); break; // "bo"
          case NG_SLSH: register_code(KC_SLSH);                  break; // "?" invert_pinky_shift
        }
    } else if(nicola_o_key == NG_SHFTR) {
        switch(nicola_m_key) {
          case NG_Q   :                         break;
          case NG_W   : register_code(KC_G);register_code(KC_A); break; // "ga"
          case NG_E   : register_code(KC_D);register_code(KC_A); break; // "da"
          case NG_R   : register_code(KC_G);register_code(KC_O); break; // "go"
          case NG_T   : register_code(KC_Z);register_code(KC_A); break; // "za"
          case NG_Y   : register_code(KC_Y);register_code(KC_O); break; // "yo"
          case NG_U   : register_code(KC_N);register_code(KC_I); break; // "ni"
          case NG_I   : register_code(KC_R);register_code(KC_U); break; // "ru"
          case NG_O   : register_code(KC_M);register_code(KC_A); break; // "ma"
          case NG_P   : register_code(KC_X);register_code(KC_E); break; // "xe"
          case NG_LBRC: register_code(KC_MINS); break; // "-"
          case NG_RBRC: register_code(KC_EQL);  break; // "="
          case NG_BSLS: register_code(KC_DEL);  break;

          case NG_A   : register_code(KC_V);register_code(KC_U); break; // "vu"
          case NG_S   : register_code(KC_Z);register_code(KC_I); break; // "zi"
          case NG_D   : register_code(KC_D);register_code(KC_E); break; // "de"
          case NG_F   : register_code(KC_G);register_code(KC_E); break; // "ge"
          case NG_G   : register_code(KC_Z);register_code(KC_E); break; // "ze"
          case NG_H   : register_code(KC_M);register_code(KC_I); break; // "mi"
          case NG_J   : register_code(KC_O);                     break; // "o"
          case NG_K   : register_code(KC_N);register_code(KC_O); break; // "no"
          case NG_L   : register_code(KC_X);register_code(KC_Y);register_code(KC_O); break; // "xyo"
          case NG_SCLN: register_code(KC_X);register_code(KC_T);register_code(KC_U); break; // "xtu"
          case NG_QUOT: register_code(KC_BSPC); break;

          case NG_Z   :                    break;
          case NG_X   : register_code(KC_B);register_code(KC_I); break; // "bi"
          case NG_C   : register_code(KC_Z);register_code(KC_U); break; // "zu"
          case NG_V   : register_code(KC_B);register_code(KC_U); break; // "bu"
          case NG_B   : register_code(KC_B);register_code(KC_E); break; // "be"
          case NG_N   : register_code(KC_N);register_code(KC_U); break; // "nu"
          case NG_M   : register_code(KC_Y);register_code(KC_U); break; // "yu"
          case NG_COMM: register_code(KC_M);register_code(KC_U); break; // "mu"
          case NG_DOT : register_code(KC_W);register_code(KC_A); break; // "wa"
          case NG_SLSH: register_code(KC_X);register_code(KC_O); break; // "xo"
        }
    }

    if(invert_pinky_shift) {
        set_mods(saved_mods);
    }
    nicola_om_pressed = true;
    nicola_flush_pending_events();
}

void nicola_om_release(void) {
    if(!nicola_om_pressed && is_nicola_m_key(nicola_m_key) && is_nicola_o_key(nicola_o_key)) {
        nicola_om_press();
    }
    switch(nicola_m_key) {
        case NG_E_TAB : unregister_code(KC_GRV); break;

        case NG_E_Q   : unregister_code(KC_F1); break;
        case NG_E_W   : unregister_code(KC_F2); break;
        case NG_E_E   : unregister_code(KC_F3); break;
        case NG_E_R   : unregister_code(KC_F4); break;
        case NG_E_T   : unregister_code(KC_F5); break;
        case NG_E_Y   : unregister_code(KC_F6); break;
        case NG_E_U   : unregister_code(KC_F7); break;
        case NG_E_I   : unregister_code(KC_F8); break;
        case NG_E_O   : unregister_code(KC_F9); break;
        case NG_E_P   : unregister_code(KC_F10); break;
        case NG_E_LBRC: unregister_code(KC_F11); break;
        case NG_E_RBRC: unregister_code(KC_F12); break;
        case NG_E_BSLS: unregister_code(KC_DEL); break;

        case NG_E_A   : unregister_code(KC_1); break;
        case NG_E_S   : unregister_code(KC_2); break;
        case NG_E_D   : unregister_code(KC_3); break;
        case NG_E_F   : unregister_code(KC_4); break;
        case NG_E_G   : unregister_code(KC_5); break;
        case NG_E_H   : unregister_code(KC_6); break;
        case NG_E_J   : unregister_code(KC_7); break;
        case NG_E_K   : unregister_code(KC_8); break;
        case NG_E_L   : unregister_code(KC_9); break;
        case NG_E_SCLN: unregister_code(KC_0); break;
        case NG_E_QUOT: unregister_code(KC_BSPC); break;

        case NG_E_Z   : break;
        case NG_E_X   : break;
        case NG_E_C   : break;
        case NG_E_V   : break;
        case NG_E_B   : break;
        case NG_E_N   : break;
        case NG_E_M   : break;
        case NG_E_COMM: unregister_code(KC_LBRC);break;
        case NG_E_DOT : unregister_code(KC_RBRC);break;
        case NG_E_SLSH: unregister_code(KC_BSLS);break;
    }
    if(nicola_o_key == NG_SHFTL) {
        switch(nicola_m_key) {
          case NG_Q   : unregister_code(KC_X);unregister_code(KC_A); break; // "xa"
          case NG_W   : unregister_code(KC_E);                     break; // "e"
          case NG_E   : unregister_code(KC_R);unregister_code(KC_I); break; // "ri"
          case NG_R   : unregister_code(KC_X);unregister_code(KC_Y);unregister_code(KC_A); break; // "xya"
          case NG_T   : unregister_code(KC_R);unregister_code(KC_E); break; // "re"
          case NG_Y   : unregister_code(KC_P);unregister_code(KC_A); break; // "pa"
          case NG_U   : unregister_code(KC_D);unregister_code(KC_I); break; // "di"
          case NG_I   : unregister_code(KC_G);unregister_code(KC_U); break; // "gu"
          case NG_O   : unregister_code(KC_D);unregister_code(KC_U); break; // "du"
          case NG_P   : unregister_code(KC_P);unregister_code(KC_I); break; // "pi"
          case NG_LBRC: unregister_code(KC_MINS); break; // "_" ***
          case NG_RBRC: unregister_code(KC_EQL);  break; // "+" ***
          case NG_BSLS: unregister_code(KC_DEL);  break;

          case NG_A   : unregister_code(KC_W);unregister_code(KC_O); break; // "wo"
          case NG_S   : unregister_code(KC_A);                       break; // "a"
          case NG_D   : unregister_code(KC_N);unregister_code(KC_A); break; // "na"
          case NG_F   : unregister_code(KC_X);unregister_code(KC_Y);unregister_code(KC_U); break; // "xyu"
          case NG_G   : unregister_code(KC_M);unregister_code(KC_O); break; // "mo"
          case NG_H   : unregister_code(KC_B);unregister_code(KC_A); break; // "ba"
          case NG_J   : unregister_code(KC_D);unregister_code(KC_O); break; // "do"
          case NG_K   : unregister_code(KC_G);unregister_code(KC_I); break; // "gi"
          case NG_L   : unregister_code(KC_P);unregister_code(KC_O); break; // "po"
          case NG_SCLN:                           break;
          case NG_QUOT: unregister_code(KC_BSPC); break;

          case NG_Z   : unregister_code(KC_X);unregister_code(KC_U); break; // "xu"
          case NG_X   : unregister_code(KC_MINS);                  break; // "-"
          case NG_C   : unregister_code(KC_R);unregister_code(KC_O); break; // "ro"
          case NG_V   : unregister_code(KC_Y);unregister_code(KC_A); break; // "ya"
          case NG_B   : unregister_code(KC_X);unregister_code(KC_I); break; // "xi"
          case NG_N   : unregister_code(KC_P);unregister_code(KC_U); break; // "pu"
          case NG_M   : unregister_code(KC_Z);unregister_code(KC_O); break; // "zo"
          case NG_COMM: unregister_code(KC_P);unregister_code(KC_E); break; // "pe"
          case NG_DOT : unregister_code(KC_B);unregister_code(KC_O); break; // "bo"
          case NG_SLSH: unregister_code(KC_SLSH);                  break; // "?" ***
        }
    } else if(nicola_o_key == NG_SHFTR) {
        switch(nicola_m_key) {
          case NG_Q   :                         break;
          case NG_W   : unregister_code(KC_G);unregister_code(KC_A); break; // "ga"
          case NG_E   : unregister_code(KC_D);unregister_code(KC_A); break; // "da"
          case NG_R   : unregister_code(KC_G);unregister_code(KC_O); break; // "go"
          case NG_T   : unregister_code(KC_Z);unregister_code(KC_A); break; // "za"
          case NG_Y   : unregister_code(KC_Y);unregister_code(KC_O); break; // "yo"
          case NG_U   : unregister_code(KC_N);unregister_code(KC_I); break; // "ni"
          case NG_I   : unregister_code(KC_R);unregister_code(KC_U); break; // "ru"
          case NG_O   : unregister_code(KC_M);unregister_code(KC_A); break; // "ma"
          case NG_P   : unregister_code(KC_X);unregister_code(KC_E); break; // "xe"
          case NG_LBRC: unregister_code(KC_MINS); break; // "-"
          case NG_RBRC: unregister_code(KC_EQL);  break; // "="
          case NG_BSLS: unregister_code(KC_DEL);  break;

          case NG_A   : unregister_code(KC_V);unregister_code(KC_U); break; // "vu"
          case NG_S   : unregister_code(KC_Z);unregister_code(KC_I); break; // "zi"
          case NG_D   : unregister_code(KC_D);unregister_code(KC_E); break; // "de"
          case NG_F   : unregister_code(KC_G);unregister_code(KC_E); break; // "ge"
          case NG_G   : unregister_code(KC_Z);unregister_code(KC_E); break; // "ze"
          case NG_H   : unregister_code(KC_M);unregister_code(KC_I); break; // "mi"
          case NG_J   : unregister_code(KC_O);                       break; // "o"
          case NG_K   : unregister_code(KC_N);unregister_code(KC_O); break; // "no"
          case NG_L   : unregister_code(KC_X);unregister_code(KC_Y);unregister_code(KC_O); break; // "xyo"
          case NG_SCLN: unregister_code(KC_X);unregister_code(KC_T);unregister_code(KC_U); break; // "xtu"
          case NG_QUOT: unregister_code(KC_BSPC); break;

          case NG_Z   :                    break;
          case NG_X   : unregister_code(KC_B);unregister_code(KC_I); break; // "bi"
          case NG_C   : unregister_code(KC_Z);unregister_code(KC_U); break; // "zu"
          case NG_V   : unregister_code(KC_B);unregister_code(KC_U); break; // "bu"
          case NG_B   : unregister_code(KC_B);unregister_code(KC_E); break; // "be"
          case NG_N   : unregister_code(KC_N);unregister_code(KC_U); break; // "nu"
          case NG_M   : unregister_code(KC_Y);unregister_code(KC_U); break; // "yu"
          case NG_COMM: unregister_code(KC_M);unregister_code(KC_U); break; // "mu"
          case NG_DOT : unregister_code(KC_W);unregister_code(KC_A); break; // "wa"
          case NG_SLSH: unregister_code(KC_X);unregister_code(KC_O); break; // "xo"
        }
    }
    nicola_om_pressed = false;
    nicola_m_key = KC_NO;
    nicola_o_key = KC_NO;
    nicola_flush_pending_events();
}

// 親指シフトの入力処理
bool process_nicola(uint16_t keycode, keyrecord_t *record) {
  key_process_guard = 1; // timeout entrance guard
  bool cont_process = true;
  // if (!is_nicola || n_modifier > 0) return true;
  uint16_t curr_time = timer_read();

  if (record->event.pressed) {
    if(is_nicola_m_key(keycode)) {
        // M key
        switch(nicola_int_state) {
          case NICOLA_STATE_S1_INIT:
            // no timeout check
            nicola_int_state = NICOLA_STATE_S2_M;
            break;
          case NICOLA_STATE_S2_M:
            // same behavior for time out and sequential stroke
            nicola_m_release();
            break;
          case NICOLA_STATE_S3_O:
            // timeout check
            IF_TIMEOUT(curr_time - nicola_o_time > TIMEOUT_THRESHOLD) {
              // timeout => (output O) => S2
              nicola_o_release();
              nicola_int_state = NICOLA_STATE_S2_M;
            } else {
              // combo => S5
              nicola_int_state = NICOLA_STATE_S5_OM;
            }
            break;
          case NICOLA_STATE_S4_MO:
            // timeout check
            IF_TIMEOUT(curr_time - nicola_o_time > TIMEOUT_THRESHOLD) {
              // timeout => (output MO) => S2
              nicola_om_release();
              nicola_int_state = NICOLA_STATE_S2_M;
            } else {
              // combo => three key judge
              uint16_t t1 = nicola_o_time - nicola_m_time;
              uint16_t t2 = curr_time - nicola_o_time;
              if(t1 < t2) {
                // the O key in between is combo with the leading M key
                nicola_om_release();
                nicola_int_state = NICOLA_STATE_S2_M;
              } else {
                // the leading M key is single, the O key in between is combo with current key
                nicola_m_release();
                nicola_int_state = NICOLA_STATE_S5_OM;
              }
            }
            break;
          case NICOLA_STATE_S5_OM:
            // same behavior for time out and sequential stroke
            nicola_om_release();
            nicola_int_state = NICOLA_STATE_S2_M;
            break;
        }
        nicola_m_key = keycode;
        //nicola_m_mods = get_mods();
        nicola_m_time = curr_time;
        keypress_timer_start(TIMEOUT_THRESHOLD * 16);
        cont_process = false;
    } else if(is_nicola_o_key(keycode)) {
        // O key
        switch(nicola_int_state) {
          case NICOLA_STATE_S1_INIT:
            // no timeout check
            nicola_int_state = NICOLA_STATE_S3_O;
            break;
          case NICOLA_STATE_S2_M:
            // timeout check
            IF_TIMEOUT(curr_time - nicola_m_time > TIMEOUT_THRESHOLD) {
              // timeout => (output M) => S3
              nicola_m_release();
              nicola_int_state = NICOLA_STATE_S3_O;
            } else {
              // combo => S4
              nicola_int_state = NICOLA_STATE_S4_MO;
            }
            break;
          case NICOLA_STATE_S3_O:
            // same behavior for time out and sequential stroke
            nicola_o_release();
            break;
          case NICOLA_STATE_S4_MO:
            // same behavior for time out and sequential stroke
            nicola_om_release();
            nicola_int_state = NICOLA_STATE_S3_O;
            break;
          case NICOLA_STATE_S5_OM:
            // timeout check
            IF_TIMEOUT(curr_time - nicola_m_time > TIMEOUT_THRESHOLD) {
              // timeout => (output MO) => S3
              nicola_om_release();
              nicola_int_state = NICOLA_STATE_S3_O;
            } else {
              // combo => three key judge
              uint16_t t1 = nicola_m_time - nicola_o_time;
              uint16_t t2 = curr_time - nicola_m_time;
              if(t1 < t2) {
                // the M key in between is combo with the leading O key
                nicola_om_release();
                nicola_int_state = NICOLA_STATE_S3_O;
              } else {
                // the leading O key is single, the M key in between is combo with current key
                nicola_o_release();
                nicola_int_state = NICOLA_STATE_S4_MO;
              }
            }
            break;
        }
        nicola_o_key = keycode;
        //nicola_o_mods = get_mods();
        nicola_o_time = curr_time;
        keypress_timer_start(TIMEOUT_THRESHOLD * 16);
        cont_process = false;
    } else {
        // その他のキーが押された
        if(nicola_int_state == NICOLA_STATE_S1_INIT) {
            cont_process = true;
        } else {
            if(nicola_pending_count < NICOLA_MAX_PENDING_EVENTS) {
                nicola_pending_events[nicola_pending_count].keycode = keycode;
                nicola_pending_events[nicola_pending_count].pressed = record->event.pressed;
                nicola_pending_count++;
                cont_process = false;
            } else {
                // too many non-nicola keys pressed; fulshing buffer
                nicola_trigger();
                cont_process = true;
            }
        }
    }
  } else { // key release
    if(NG_TOP <= keycode && keycode <= NG_BOTTOM) { // key off
        switch(nicola_int_state) {
          case NICOLA_STATE_S1_INIT:
            break;
          case NICOLA_STATE_S2_M:
            if(nicola_m_key == keycode) {
              nicola_m_release();
              nicola_int_state = NICOLA_STATE_S1_INIT;
            }
            break;
          case NICOLA_STATE_S3_O:
            if(nicola_o_key == keycode) {
              nicola_o_release();
              nicola_int_state = NICOLA_STATE_S1_INIT;
            }
            break;
          case NICOLA_STATE_S4_MO:
            if(nicola_m_key == keycode) {
              // M ON --> O ON --> M OFF
              uint16_t t1 = nicola_o_time - nicola_m_time;
              uint16_t t2 = curr_time - nicola_o_time;
              if(t1>=t2 && t2 < OVERLAP_THRESHOLD) {
                // M ON --> O ON --> M OFF (M is output, but O is still open to combo)
                nicola_m_release();
                nicola_int_state = NICOLA_STATE_S3_O;
              } else {
                // M ON --> O ON --> M OFF (both M and O are output)
                nicola_om_release();
                nicola_int_state = NICOLA_STATE_S1_INIT;
              }
            } else if(nicola_o_key == keycode) {
              // M ON --> O ON --> O OFF (both M and O are output)
              nicola_om_release();
              nicola_int_state = NICOLA_STATE_S1_INIT;
            }
            break;
          case NICOLA_STATE_S5_OM:
            if(nicola_o_key == keycode) {
              // O ON --> M ON --> O OFF
              uint16_t t1 = nicola_m_time - nicola_o_time;
              uint16_t t2 = curr_time - nicola_m_time;
              if(t1>=t2 && t2 < OVERLAP_THRESHOLD) {
                // O ON --> M ON --> O OFF (O is output, but M is still open to combo)
                nicola_o_release();
                nicola_int_state = NICOLA_STATE_S2_M;
              } else {
                // O ON --> M ON --> O OFF (both M and O are output)
                nicola_om_release();
                nicola_int_state = NICOLA_STATE_S1_INIT;
              }
            } else if(nicola_m_key == keycode) {
              // O ON --> M ON --> M OFF (both O and M are output)
              nicola_om_release();
              nicola_int_state = NICOLA_STATE_S1_INIT;
            }
            break;
        }
        cont_process = false;
    } else {
        // その他のキーが離された
        if(nicola_int_state == NICOLA_STATE_S1_INIT) {
            cont_process = true;
        } else {
            if(nicola_pending_count < NICOLA_MAX_PENDING_EVENTS) {
                nicola_pending_events[nicola_pending_count].keycode = keycode;
                nicola_pending_events[nicola_pending_count].pressed = record->event.pressed;
                nicola_pending_count++;
                cont_process = false;
            } else {
                // too many non-nicola keys pressed; fulshing buffer
                nicola_trigger();
                cont_process = true;
            }
        }
    }
  }
  key_process_guard = 0;
  return cont_process;
}

void keypress_timer_expired(void) {
    if(!key_process_guard) {
        nicola_trigger();
    }
}
