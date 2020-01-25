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

#define TIMEOUT_THRESHOLD (150)
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

static int key_process_guard = 0;
void keypress_timer_expired(void);

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
}

// 親指シフトをオンオフ
void nicola_on(void) {
  is_nicola = true;
  nicola_clear();
  layer_on(nicola_layer);

//   tap_code(KC_LANG1); // Mac
//   tap_code(KC_HENK); // Win
}

void nicola_off(void) {
  is_nicola = false;
  nicola_clear();
  layer_off(nicola_layer);

//   tap_code(KC_LANG2); // Mac
//   tap_code(KC_MHEN); // Win
}

// 親指シフトの状態
bool nicola_state(void) {
  return is_nicola;
}

// バッファをクリアする
void nicola_clear(void) {
  nicola_int_state = NICOLA_STATE_S1_INIT;
  key_process_guard = 0;
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

void nicola_m_type(void) {
    switch(nicola_m_key) {
        case NG_1   : send_string(SS_ALNUM(SS_TAP(X_1))); break;
        case NG_2   : send_string(SS_ALNUM(SS_TAP(X_2))); break;
        case NG_3   : send_string(SS_ALNUM(SS_TAP(X_3))); break;
        case NG_4   : send_string(SS_ALNUM(SS_TAP(X_4))); break;
        case NG_5   : send_string(SS_ALNUM(SS_TAP(X_5))); break;
        case NG_6   : send_string(SS_ALNUM(SS_TAP(X_6))); break;
        case NG_7   : send_string(SS_ALNUM(SS_TAP(X_7))); break;
        case NG_8   : send_string(SS_ALNUM(SS_TAP(X_8))); break;
        case NG_9   : send_string(SS_ALNUM(SS_TAP(X_9))); break;
        case NG_0   : send_string(SS_ALNUM(SS_TAP(X_0))); break;
        case NG_MINS: send_string(SS_ALNUM(SS_TAP(X_MINUS))); break;
        case NG_EQL : send_string(SS_ALNUM(SS_TAP(X_EQUAL))); break;

        case NG_Q   : send_string(SS_LSFT(SS_TAP(X_DOT))); break; // 。
        case NG_W   : send_string(SS_TAP(X_T)); break; // か
        case NG_E   : send_string(SS_TAP(X_Q)); break; // た
        case NG_R   : send_string(SS_TAP(X_B)); break; // こ
        case NG_T   : send_string(SS_TAP(X_X)); break; // さ
        case NG_Y   : send_string(SS_TAP(X_O)); break; // ら
        case NG_U   : send_string(SS_TAP(X_A)); break; // ち
        case NG_I   : send_string(SS_TAP(X_H)); break; // く
        case NG_O   : send_string(SS_TAP(X_Z)); break; // つ
        case NG_P   : send_string(SS_ALNUM(SS_TAP(X_COMMA))); break; // ，
        case NG_LBRC: send_string(SS_LSFT(SS_TAP(X_COMMA))); break; // 、
        case NG_RBRC:                           break; //
        case NG_BSLS: send_string(SS_ALNUM(SS_TAP(X_BSLASH))); break;

        case NG_A   : send_string(SS_TAP(X_4)); break; // う
        case NG_S   : send_string(SS_TAP(X_D)); break; // し
        case NG_D   : send_string(SS_TAP(X_W)); break; // て
        case NG_F   : send_string(SS_TAP(X_QUOTE)); break; // け
        case NG_G   : send_string(SS_TAP(X_P)); break; // せ
        case NG_H   : send_string(SS_TAP(X_F)); break; // は
        case NG_J   : send_string(SS_TAP(X_S)); break; // と
        case NG_K   : send_string(SS_TAP(X_G)); break; // き
        case NG_L   : send_string(SS_TAP(X_E)); break; // い
        case NG_SCLN: send_string(SS_TAP(X_Y)); break; // ん
        case NG_QUOT: send_string(SS_TAP(X_BSPACE)); break;

        case NG_Z   : send_string(SS_ALNUM(SS_TAP(X_DOT))); break; // ．
        case NG_X   : send_string(SS_TAP(X_V)); break; // ひ
        case NG_C   : send_string(SS_TAP(X_R)); break; // す
        case NG_V   : send_string(SS_TAP(X_2)); break; // ふ
        case NG_B   : send_string(SS_TAP(X_EQUAL)); break; // へ
        case NG_N   : send_string(SS_TAP(X_SLASH)); break; // め
        case NG_M   : send_string(SS_TAP(X_C)); break; // そ
        case NG_COMM: send_string(SS_TAP(X_COMMA)); break; // ね
        case NG_DOT : send_string(SS_TAP(X_MINUS)); break; // ほ
        case NG_SLSH: send_string(SS_LSFT(SS_TAP(X_SLASH))); break; // ・
    }
}

void nicola_o_type(void) {
    if(nicola_o_key != 0) {
        send_string(" ");
    }
}

void nicola_om_type(void) {
    if(nicola_o_key == NG_SHFTL) {
        switch(nicola_m_key) {
            case NG_1   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_SLASH)))); break;
            case NG_2   : send_string(SS_ALNUM(SS_TAP(X_SLASH))); break;
            case NG_3   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_GRAVE)))); break;
            case NG_4   : send_string(SS_LSFT(SS_TAP(X_LBRACKET))); break;
            case NG_5   : send_string(SS_LSFT(SS_TAP(X_RBRACKET))); break;
            case NG_6   : send_string(SS_ALNUM(SS_TAP(X_LBRACKET))); break;
            case NG_7   : send_string(SS_ALNUM(SS_TAP(X_RBRACKET))); break;
            case NG_8   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_8)))); break;
            case NG_9   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_9)))); break;
            case NG_0   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_0)))); break;
            case NG_MINS: send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_MINUS)))); break;
            case NG_EQL : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_EQUAL)))); break;

            case NG_Q   : send_string(SS_LSFT(SS_TAP(X_3))); break; // ぁ
            case NG_W   : send_string(SS_TAP(X_5)); break; // え
            case NG_E   : send_string(SS_TAP(X_L)); break; // り
            case NG_R   : send_string(SS_LSFT(SS_TAP(X_7))); break; // ゃ
            case NG_T   : send_string(SS_TAP(X_SCOLON)); break; // れ
            case NG_Y   : send_string(SS_TAP(X_F) SS_TAP(X_RBRACKET)); break; // ぱ
            case NG_U   : send_string(SS_TAP(X_A) SS_TAP(X_LBRACKET)); break; // ぢ
            case NG_I   : send_string(SS_TAP(X_H) SS_TAP(X_LBRACKET)); break; // ぐ
            case NG_O   : send_string(SS_TAP(X_Z) SS_TAP(X_LBRACKET)); break; // づ
            case NG_P   : send_string(SS_TAP(X_V) SS_TAP(X_RBRACKET)); break; // ぴ
            case NG_LBRC: send_string(SS_LSFT(SS_TAP(X_LBRACKET))); break; // 「
            case NG_RBRC: send_string(SS_LSFT(SS_TAP(X_RBRACKET))); break; // 」
            case NG_BSLS: send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_BSLASH)))); break;

            case NG_A   : send_string(SS_LSFT(SS_TAP(X_0))); break; // を
            case NG_S   : send_string(SS_TAP(X_3)); break; // あ
            case NG_D   : send_string(SS_TAP(X_U)); break; // な
            case NG_F   : send_string(SS_LSFT(SS_TAP(X_8))); break; // ゅ
            case NG_G   : send_string(SS_TAP(X_M)); break; // も
            case NG_H   : send_string(SS_TAP(X_F) SS_TAP(X_LBRACKET)); break; // ば
            case NG_J   : send_string(SS_TAP(X_S) SS_TAP(X_LBRACKET)); break; // ど
            case NG_K   : send_string(SS_TAP(X_G) SS_TAP(X_LBRACKET)); break; // ぎ
            case NG_L   : send_string(SS_TAP(X_MINUS) SS_TAP(X_RBRACKET)); break; // ぽ
            case NG_SCLN:                    break; //
            case NG_QUOT: send_string(SS_TAP(X_BSPACE)); break; // BS

            case NG_Z   : send_string(SS_LSFT(SS_TAP(X_4))); break; // ぅ
            case NG_X   : send_string(SS_LSFT(SS_TAP(X_MINUS))); break; // ー
            case NG_C   : send_string(SS_TAP(X_GRAVE)); break; // ろ
            case NG_V   : send_string(SS_TAP(X_7)); break; // や
            case NG_B   : send_string(SS_LSFT(SS_TAP(X_E))); break; // ぃ
            case NG_N   : send_string(SS_TAP(X_2) SS_TAP(X_RBRACKET)); break; // ぷ
            case NG_M   : send_string(SS_TAP(X_C) SS_TAP(X_LBRACKET)); break; // ぞ
            case NG_COMM: send_string(SS_TAP(X_EQUAL) SS_TAP(X_RBRACKET)); break; // ぺ
            case NG_DOT : send_string(SS_TAP(X_MINUS) SS_TAP(X_LBRACKET)); break; // ぼ
            case NG_SLSH: send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_SLASH)))); break; // ？
        }
    } else if(nicola_o_key == NG_SHFTR) {
        switch(nicola_m_key) {
            case NG_1   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_SLASH)))); break;
            case NG_2   : send_string(SS_ALNUM(SS_TAP(X_SLASH))); break;
            case NG_3   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_GRAVE)))); break;
            case NG_4   : send_string(SS_LSFT(SS_TAP(X_LBRACKET))); break;
            case NG_5   : send_string(SS_LSFT(SS_TAP(X_RBRACKET))); break;
            case NG_6   : send_string(SS_ALNUM(SS_TAP(X_LBRACKET))); break;
            case NG_7   : send_string(SS_ALNUM(SS_TAP(X_RBRACKET))); break;
            case NG_8   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_8)))); break;
            case NG_9   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_9)))); break;
            case NG_0   : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_0)))); break;
            case NG_MINS: send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_MINUS)))); break;
            case NG_EQL : send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_EQUAL)))); break;

            case NG_Q   : send_string(SS_TAP(X_RBRACKET)); break; // ゜
            case NG_W   : send_string(SS_TAP(X_T) SS_TAP(X_LBRACKET)); break; // が
            case NG_E   : send_string(SS_TAP(X_Q) SS_TAP(X_LBRACKET)); break; // だ
            case NG_R   : send_string(SS_TAP(X_B) SS_TAP(X_LBRACKET)); break; // ご
            case NG_T   : send_string(SS_TAP(X_X) SS_TAP(X_LBRACKET)); break; // ざ
            case NG_Y   : send_string(SS_TAP(X_9)); break; // よ
            case NG_U   : send_string(SS_TAP(X_I)); break; // に
            case NG_I   : send_string(SS_TAP(X_DOT)); break; // る
            case NG_O   : send_string(SS_TAP(X_J)); break; // ま
            case NG_P   : send_string(SS_LSFT(SS_TAP(X_5))); break; // ぇ
            case NG_LBRC: send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_RO)))); break; // ＿
            case NG_RBRC: send_string(SS_ALNUM(SS_TAP(X_SCOLON))); break; // ；

            case NG_A   : send_string(SS_TAP(X_4) SS_TAP(X_LBRACKET)); break; // ヴ
            case NG_S   : send_string(SS_TAP(X_D) SS_TAP(X_LBRACKET)); break; // じ
            case NG_D   : send_string(SS_TAP(X_W) SS_TAP(X_LBRACKET)); break; // で
            case NG_F   : send_string(SS_TAP(X_QUOTE) SS_TAP(X_LBRACKET)); break; // げ
            case NG_G   : send_string(SS_TAP(X_P) SS_TAP(X_LBRACKET)); break; // ぜ
            case NG_H   : send_string(SS_TAP(X_N)); break; // み
            case NG_J   : send_string(SS_TAP(X_6)); break; // お
            case NG_K   : send_string(SS_TAP(X_K)); break; // の
            case NG_L   : send_string(SS_LSFT(SS_TAP(X_9))); break; // ょ
            case NG_SCLN: send_string(SS_LSFT(SS_TAP(X_Z))); break; // っ
            case NG_QUOT: send_string(SS_ALNUM(SS_TAP(X_QUOT))); break; // ’
            case NG_BSLS: send_string(SS_ALNUM(SS_LSFT(SS_TAP(X_BSLASH)))); break;

            case NG_Z   : send_string(SS_TAP(X_LBRACKET)); break; // ゛
            case NG_X   : send_string(SS_TAP(X_V) SS_TAP(X_LBRACKET)); break; // び
            case NG_C   : send_string(SS_TAP(X_R) SS_TAP(X_LBRACKET)); break; // ず
            case NG_V   : send_string(SS_TAP(X_2) SS_TAP(X_LBRACKET)); break; //    ぶ
            case NG_B   : send_string(SS_TAP(X_EQUAL) SS_TAP(X_LBRACKET)); break; // べ
            case NG_N   : send_string(SS_TAP(X_1)); break; // ぬ
            case NG_M   : send_string(SS_TAP(X_8)); break; // ゆ
            case NG_COMM: send_string(SS_TAP(X_JYEN)); break; // む
            case NG_DOT : send_string(SS_TAP(X_0)); break; // わ
            case NG_SLSH: send_string(SS_LSFT(SS_TAP(X_6))); break; // ぉ
        }
    }
}

// 親指シフトの入力処理
bool process_nicola(uint16_t keycode, keyrecord_t *record) {
  key_process_guard = 1; // timeout entrance guard
  bool cont_process = true;
  // if (!is_nicola || n_modifier > 0) return true;
  uint16_t curr_time = timer_read();

  if (record->event.pressed) {
    if(NG_M_TOP <= keycode && keycode <= NG_M_BOTTOM) {
        // M key
        switch(nicola_int_state) {
          case NICOLA_STATE_S1_INIT:
            // no timeout check
            nicola_int_state = NICOLA_STATE_S2_M;
            break;
          case NICOLA_STATE_S2_M:
            // same behavior for time out and sequential stroke
            nicola_m_type();
            break;
          case NICOLA_STATE_S3_O:
            // timeout check
            IF_TIMEOUT(curr_time - nicola_o_time > TIMEOUT_THRESHOLD) {
              // timeout => (output O) => S2
              nicola_o_type();
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
              nicola_om_type();
              nicola_int_state = NICOLA_STATE_S2_M;
            } else {
              // combo => three key judge
              uint16_t t1 = nicola_o_time - nicola_m_time;
              uint16_t t2 = curr_time - nicola_o_time;
              if(t1 < t2) {
                // the O key in between is combo with the leading M key
                nicola_om_type();
                nicola_int_state = NICOLA_STATE_S2_M;
              } else {
                // the leading M key is single, the O key in between is combo with current key
                nicola_m_type();
                nicola_int_state = NICOLA_STATE_S5_OM;
              }
            }
            break;
          case NICOLA_STATE_S5_OM:
            // same behavior for time out and sequential stroke
            nicola_om_type();
            nicola_int_state = NICOLA_STATE_S2_M;
            break;
        }
        nicola_m_key = keycode;
        nicola_m_time = curr_time;
        keypress_timer_start(TIMEOUT_THRESHOLD * 16);
        cont_process = false;
    } else if(keycode == NG_SHFTL || keycode == NG_SHFTR) {
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
              nicola_m_type();
              nicola_int_state = NICOLA_STATE_S3_O;
            } else {
              // combo => S4
              nicola_int_state = NICOLA_STATE_S4_MO;
            }
            break;
          case NICOLA_STATE_S3_O:
            // same behavior for time out and sequential stroke
            nicola_o_type();
            break;
          case NICOLA_STATE_S4_MO:
            // same behavior for time out and sequential stroke
            nicola_om_type();
            nicola_int_state = NICOLA_STATE_S3_O;
            break;
          case NICOLA_STATE_S5_OM:
            // timeout check
            IF_TIMEOUT(curr_time - nicola_m_time > TIMEOUT_THRESHOLD) {
              // timeout => (output MO) => S3
              nicola_om_type();
              nicola_int_state = NICOLA_STATE_S3_O;
            } else {
              // combo => three key judge
              uint16_t t1 = nicola_m_time - nicola_o_time;
              uint16_t t2 = curr_time - nicola_m_time;
              if(t1 < t2) {
                // the M key in between is combo with the leading O key
                nicola_om_type();
                nicola_int_state = NICOLA_STATE_S3_O;
              } else {
                // the leading O key is single, the M key in between is combo with current key
                nicola_o_type();
                nicola_int_state = NICOLA_STATE_S4_MO;
              }
            }
            break;
        }
        nicola_o_key = keycode;
        nicola_o_time = curr_time;
        keypress_timer_start(TIMEOUT_THRESHOLD * 16);
        cont_process = false;
    } else {
        // その他のキーが押された
        switch(nicola_int_state) {
          case NICOLA_STATE_S1_INIT:
            break;
          case NICOLA_STATE_S2_M:
            nicola_m_type();
            break;
          case NICOLA_STATE_S3_O:
            nicola_o_type();
            break;
          case NICOLA_STATE_S4_MO:
            nicola_om_type();
            break;
          case NICOLA_STATE_S5_OM:
            nicola_om_type();
            break;
        }
        nicola_int_state = NICOLA_STATE_S1_INIT;
        key_process_guard = 0;
        // continue processing current key, so this path returns true
    }
  } else { // key release
    if(NG_TOP <= keycode && keycode <= NG_BOTTOM) { // key off
        switch(nicola_int_state) {
          case NICOLA_STATE_S1_INIT:
            break;
          case NICOLA_STATE_S2_M:
            if(nicola_m_key == keycode) {
              nicola_m_type();
              nicola_int_state = NICOLA_STATE_S1_INIT;
            }
            break;
          case NICOLA_STATE_S3_O:
            if(nicola_o_key == keycode) {
              nicola_o_type();
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
                nicola_m_type();
                nicola_int_state = NICOLA_STATE_S3_O;
              } else {
                // M ON --> O ON --> M OFF (both M and O are output)
                nicola_om_type();
                nicola_int_state = NICOLA_STATE_S1_INIT;
              }
            } else if(nicola_o_key == keycode) {
              // M ON --> O ON --> O OFF (both M and O are output)
              nicola_om_type();
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
                nicola_o_type();
                nicola_int_state = NICOLA_STATE_S2_M;
              } else {
                // O ON --> M ON --> O OFF (both M and O are output)
                nicola_om_type();
                nicola_int_state = NICOLA_STATE_S1_INIT;
              }
            } else if(nicola_m_key == keycode) {
              // O ON --> M ON --> M OFF (both O and M are output)
              nicola_om_type();
              nicola_int_state = NICOLA_STATE_S1_INIT;
            }
            break;
        }
        cont_process = false;
    }
  }
  key_process_guard = 0;
  return cont_process;
}

void keypress_timer_expired(void) {
    if(!key_process_guard) {
        switch(nicola_int_state) {
            case NICOLA_STATE_S1_INIT:
                break;
            case NICOLA_STATE_S2_M:
                nicola_m_type();
                break;
            case NICOLA_STATE_S3_O:
                nicola_o_type();
                break;
            case NICOLA_STATE_S4_MO:
                nicola_om_type();
                break;
            case NICOLA_STATE_S5_OM:
                nicola_om_type();
                break;
        }
        nicola_int_state = NICOLA_STATE_S1_INIT;
    }
}
