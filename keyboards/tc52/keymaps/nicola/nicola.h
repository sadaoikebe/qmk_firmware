/* NICOLA 親指シフト
 *
 * Copyright 2018-2019 eswai <@eswai>          (原型)
 * Copyright 2020-2026 Sadao Ikebe @bonyarou   (NICOLA 規格のタイミング判定・再実装)
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * 挙動の仕様は docs/NICOLA-SPEC.md を参照。
 * このファイルは仕様書の第0節「設計原則」を必ず守ること:
 *
 *   原則1: press と release は物理キーと厳密に対応させる。
 *          tap_code() / tap_code16() は使わない。
 *   原則2: 同時打鍵判定を通すかどうかは keymap 側で決める。
 */

#pragma once

#include "quantum.h"

/* ------------------------------------------------------------------
 * キーコード
 *
 * 分類は範囲比較で行うので、この enum の順序には意味がある。
 * NG_M_TOP..NG_M_BOTTOM      = M キー (文字キー)
 * NG_SHFTL, NG_SHFTR         = O キー (親指キー)
 * NG_LEFT_FINGER_*           = 左手で打つキー
 * NG_RIGHT_FINGER_*          = 右手で打つキー
 * NG_EISU1_*, NG_EISU2_*     = 英数キー
 * ------------------------------------------------------------------ */
typedef enum nicola_keycodes {
    NG_TOP = SAFE_RANGE,
    NG_M_TOP = NG_TOP,

    NG_E_TAB = NG_TOP,
    NG_LEFT_FINGER_TOP = NG_E_TAB,
    NG_EISU1_TOP       = NG_E_TAB,

    NG_E_Q,
    NG_E_W,
    NG_E_E,
    NG_E_R,
    NG_E_T,

    NG_E_A,
    NG_E_S,
    NG_E_D,
    NG_E_F,
    NG_E_G,

    NG_E_Z,
    NG_E_X,
    NG_E_C,
    NG_E_V,
    NG_E_B,
    NG_EISU1_BOTTOM = NG_E_B,

    NG_Q,
    NG_W,
    NG_E,
    NG_R,
    NG_T,

    NG_A,
    NG_S,
    NG_D,
    NG_F,
    NG_G,

    NG_Z,
    NG_X,
    NG_C,
    NG_V,
    NG_B,
    NG_LEFT_FINGER_BOTTOM = NG_B,

    NG_E_Y,
    NG_RIGHT_FINGER_TOP = NG_E_Y,
    NG_EISU2_TOP        = NG_E_Y,
    NG_E_U,
    NG_E_I,
    NG_E_O,
    NG_E_P,
    NG_E_LBRC,
    NG_E_RBRC,
    NG_E_BSLS,

    NG_E_H,
    NG_E_J,
    NG_E_K,
    NG_E_L,
    NG_E_SCLN,
    NG_E_QUOT,

    NG_E_N,
    NG_E_M,
    NG_E_COMM,
    NG_E_DOT,
    NG_E_SLSH,
    NG_EISU2_BOTTOM = NG_E_SLSH,

    NG_Y,
    NG_U,
    NG_I,
    NG_O,
    NG_P,
    NG_LBRC,
    NG_RBRC,
    NG_BSLS,

    NG_H,
    NG_J,
    NG_K,
    NG_L,
    NG_SCLN,
    NG_QUOT,

    NG_N,
    NG_M,
    NG_COMM,
    NG_DOT,
    NG_SLSH,
    NG_RIGHT_FINGER_BOTTOM = NG_SLSH,

    /* カーソルキーは M キーだが、左手・右手のどちらの範囲にも入らない。
     * したがって is_cross_shift() は常に false になる。旧実装からの仕様。 */
    NG_LEFT,
    NG_DOWN,
    NG_UP,
    NG_RIGHT,
    NG_M_BOTTOM = NG_RIGHT,

    NG_SHFTL,
    NG_SHFTR,
    NG_BOTTOM = NG_SHFTR,
} NGKEYS;

/* keymap 側で独自キーコードを足すときの開始位置 */
#define NG_SAFE_RANGE (NG_BOTTOM + 1)

/* ------------------------------------------------------------------
 * API
 * ------------------------------------------------------------------ */

/* NICOLA レイヤー番号を登録する。keymap_init 相当のタイミングで一度呼ぶ */
void set_nicola(uint8_t layer);

/* かなモードの ON/OFF。レイヤーの出し入れも行う */
void nicola_on(void);
void nicola_off(void);
bool nicola_state(void);

/* 保留中の打鍵を確定させて待機状態に戻す */
void nicola_clear(void);

/* [ゲート2] modifier 押下中は NICOLA レイヤーを一時的に外す。
 * process_nicola() より前に呼ぶこと */
void nicola_mode(uint16_t keycode, keyrecord_t *record);

/* 同時打鍵判定の本体。false を返したらそのキーは QMK に渡さない */
bool process_nicola(uint16_t keycode, keyrecord_t *record);
