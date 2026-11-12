/* NICOLA 出力テーブル (データ)
 *
 * Copyright 2018-2019 eswai <@eswai>
 * Copyright 2020-2026 Sadao Ikebe @bonyarou
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * このファイルは旧実装 (master e3d92a7463 の nicola.c) の switch 文から
 * 機械的に抽出して生成したもの。かなの割り当ては NICOLA 規格。
 * 手で書き写していないので転記ミスはない。
 */

#include "nicola.h"
#include "nicola_table.h"
#include "progmem.h"
#include "util.h"

/* 単独打鍵 (73件) */
static const nicola_entry_t nicola_table_tap[] PROGMEM = {
    {NG_Q,        {KC_DOT, 0, 0}},  // "."
    {NG_W,        {KC_K, KC_A, 0}},  // "ka"
    {NG_E,        {KC_T, KC_A, 0}},  // "ta"
    {NG_R,        {KC_K, KC_O, 0}},  // "ko"
    {NG_T,        {KC_S, KC_A, 0}},  // "sa"
    {NG_Y,        {KC_R, KC_A, 0}},  // "ra"
    {NG_U,        {KC_T, KC_I, 0}},  // "ti"
    {NG_I,        {KC_K, KC_U, 0}},  // "ku"
    {NG_O,        {KC_T, KC_U, 0}},  // "tu"
    {NG_P,        {KC_COMM, 0, 0}},  // ","
    {NG_LBRC,     {KC_COMM, 0, 0}},  // ","
    {NG_RBRC,     {KC_SCLN, 0, 0}},  // ";"
    {NG_BSLS,     {KC_BSPC, 0, 0}},
    {NG_A,        {KC_U, 0, 0}},  // "u"
    {NG_S,        {KC_S, KC_I, 0}},  // "si"
    {NG_D,        {KC_T, KC_E, 0}},  // "te"
    {NG_F,        {KC_K, KC_E, 0}},  // "ke"
    {NG_G,        {KC_S, KC_E, 0}},  // "se"
    {NG_H,        {KC_H, KC_A, 0}},  // "ha"
    {NG_J,        {KC_T, KC_O, 0}},  // "to"
    {NG_K,        {KC_K, KC_I, 0}},  // "ki"
    {NG_L,        {KC_I, 0, 0}},  // "i"
    {NG_SCLN,     {KC_N, KC_N, 0}},  // "nn"
    {NG_QUOT,     {KC_BSPC, 0, 0}},
    {NG_Z,        {KC_DOT, 0, 0}},  // "."
    {NG_X,        {KC_H, KC_I, 0}},  // "hi"
    {NG_C,        {KC_S, KC_U, 0}},  // "su"
    {NG_V,        {KC_H, KC_U, 0}},  // "hu"
    {NG_B,        {KC_H, KC_E, 0}},  // "he"
    {NG_N,        {KC_M, KC_E, 0}},  // "me"
    {NG_M,        {KC_S, KC_O, 0}},  // "so"
    {NG_COMM,     {KC_N, KC_E, 0}},  // "ne"
    {NG_DOT,      {KC_H, KC_O, 0}},  // "ho"
    {NG_SLSH,     {KC_SLSH, 0, 0}},  // "/"
    {NG_E_TAB,    {KC_TAB, 0, 0}},
    {NG_E_Q,      {KC_Q, 0, 0}},
    {NG_E_W,      {KC_W, 0, 0}},
    {NG_E_E,      {KC_E, 0, 0}},
    {NG_E_R,      {KC_R, 0, 0}},
    {NG_E_T,      {KC_T, 0, 0}},
    {NG_E_Y,      {KC_Y, 0, 0}},
    {NG_E_U,      {KC_U, 0, 0}},
    {NG_E_I,      {KC_I, 0, 0}},
    {NG_E_O,      {KC_O, 0, 0}},
    {NG_E_P,      {KC_P, 0, 0}},
    {NG_E_LBRC,   {KC_MINS, 0, 0}},
    {NG_E_RBRC,   {KC_EQL, 0, 0}},
    {NG_E_BSLS,   {KC_BSPC, 0, 0}},
    {NG_E_A,      {KC_A, 0, 0}},
    {NG_E_S,      {KC_S, 0, 0}},
    {NG_E_D,      {KC_D, 0, 0}},
    {NG_E_F,      {KC_F, 0, 0}},
    {NG_E_G,      {KC_G, 0, 0}},
    {NG_E_H,      {KC_H, 0, 0}},
    {NG_E_J,      {KC_J, 0, 0}},
    {NG_E_K,      {KC_K, 0, 0}},
    {NG_E_L,      {KC_L, 0, 0}},
    {NG_E_SCLN,   {KC_SCLN, 0, 0}},
    {NG_E_QUOT,   {KC_QUOT, 0, 0}},
    {NG_E_Z,      {KC_Z, 0, 0}},
    {NG_E_X,      {KC_X, 0, 0}},
    {NG_E_C,      {KC_C, 0, 0}},
    {NG_E_V,      {KC_V, 0, 0}},
    {NG_E_B,      {KC_B, 0, 0}},
    {NG_E_N,      {KC_N, 0, 0}},
    {NG_E_M,      {KC_M, 0, 0}},
    {NG_E_COMM,   {KC_COMM, 0, 0}},
    {NG_E_DOT,    {KC_DOT, 0, 0}},
    {NG_E_SLSH,   {KC_SLSH, 0, 0}},
    {NG_LEFT,     {KC_LEFT, 0, 0}},
    {NG_DOWN,     {KC_DOWN, 0, 0}},
    {NG_UP,       {KC_UP, 0, 0}},
    {NG_RIGHT,    {KC_RIGHT, 0, 0}},
};

/* 同時打鍵 - 左右どちらの親指でも同じ (32件) */
static const nicola_entry_t nicola_table_both[] PROGMEM = {
    {NG_E_TAB,    {KC_GRV, 0, 0}},
    {NG_E_Q,      {KC_F1, 0, 0}},
    {NG_E_W,      {KC_F2, 0, 0}},
    {NG_E_E,      {KC_F3, 0, 0}},
    {NG_E_R,      {KC_F4, 0, 0}},
    {NG_E_T,      {KC_F5, 0, 0}},
    {NG_E_Y,      {KC_F6, 0, 0}},
    {NG_E_U,      {KC_F7, 0, 0}},
    {NG_E_I,      {KC_F8, 0, 0}},
    {NG_E_O,      {KC_F9, 0, 0}},
    {NG_E_P,      {KC_F10, 0, 0}},
    {NG_E_LBRC,   {KC_F11, 0, 0}},
    {NG_E_RBRC,   {KC_F12, 0, 0}},
    {NG_E_BSLS,   {KC_DEL, 0, 0}},
    {NG_E_A,      {KC_1, 0, 0}},
    {NG_E_S,      {KC_2, 0, 0}},
    {NG_E_D,      {KC_3, 0, 0}},
    {NG_E_F,      {KC_4, 0, 0}},
    {NG_E_G,      {KC_5, 0, 0}},
    {NG_E_H,      {KC_6, 0, 0}},
    {NG_E_J,      {KC_7, 0, 0}},
    {NG_E_K,      {KC_8, 0, 0}},
    {NG_E_L,      {KC_9, 0, 0}},
    {NG_E_SCLN,   {KC_0, 0, 0}},
    {NG_E_QUOT,   {KC_BSPC, 0, 0}},
    {NG_E_COMM,   {KC_LBRC, 0, 0}},
    {NG_E_DOT,    {KC_RBRC, 0, 0}},
    {NG_E_SLSH,   {KC_BSLS, 0, 0}},
    {NG_LEFT,     {KC_HOME, 0, 0}},
    {NG_DOWN,     {KC_PGDN, 0, 0}},
    {NG_UP,       {KC_PGUP, 0, 0}},
    {NG_RIGHT,    {KC_END, 0, 0}},
};

/* 同時打鍵 - 左親指 (NG_SHFTL) (37件) */
static const nicola_entry_t nicola_table_shftl[] PROGMEM = {
    {NG_Q,        {KC_X, KC_A, 0}},  // "xa"
    {NG_W,        {KC_E, 0, 0}},  // "e"
    {NG_E,        {KC_R, KC_I, 0}},  // "ri"
    {NG_R,        {KC_X, KC_Y, KC_A}},  // "xya"
    {NG_T,        {KC_R, KC_E, 0}},  // "re"
    {NG_Y,        {KC_P, KC_A, 0}},  // "pa"
    {NG_U,        {KC_D, KC_I, 0}},  // "di"
    {NG_I,        {KC_G, KC_U, 0}},  // "gu"
    {NG_O,        {KC_D, KC_U, 0}},  // "du"
    {NG_P,        {KC_P, KC_I, 0}},  // "pi"
    {NG_LBRC,     {KC_MINS, 0, 0}},  // "_" invert_pinky_shift
    {NG_RBRC,     {KC_EQL, 0, 0}},  // "+" invert_pinky_shift
    {NG_BSLS,     {KC_DEL, 0, 0}},
    {NG_A,        {KC_W, KC_O, 0}},  // "wo"
    {NG_S,        {KC_A, 0, 0}},  // "a"
    {NG_D,        {KC_N, KC_A, 0}},  // "na"
    {NG_F,        {KC_X, KC_Y, KC_U}},  // "xyu"
    {NG_G,        {KC_M, KC_O, 0}},  // "mo"
    {NG_H,        {KC_B, KC_A, 0}},  // "ba"
    {NG_J,        {KC_D, KC_O, 0}},  // "do"
    {NG_K,        {KC_G, KC_I, 0}},  // "gi"
    {NG_L,        {KC_P, KC_O, 0}},  // "po"
    {NG_QUOT,     {KC_BSPC, 0, 0}},
    {NG_Z,        {KC_X, KC_U, 0}},  // "xu"
    {NG_X,        {KC_MINS, 0, 0}},  // "-"
    {NG_C,        {KC_R, KC_O, 0}},  // "ro"
    {NG_V,        {KC_Y, KC_A, 0}},  // "ya"
    {NG_B,        {KC_X, KC_I, 0}},  // "xi"
    {NG_N,        {KC_P, KC_U, 0}},  // "pu"
    {NG_M,        {KC_Z, KC_O, 0}},  // "zo"
    {NG_COMM,     {KC_P, KC_E, 0}},  // "pe"
    {NG_DOT,      {KC_B, KC_O, 0}},  // "bo"
    {NG_SLSH,     {KC_SLSH, 0, 0}},  // "?" invert_pinky_shift
    {NG_E_C,      {KC_LEFT, 0, 0}},
    {NG_E_V,      {KC_DOWN, 0, 0}},
    {NG_E_N,      {KC_PGUP, 0, 0}},
    {NG_E_M,      {KC_END, 0, 0}},
};

/* 同時打鍵 - 右親指 (NG_SHFTR) (36件) */
static const nicola_entry_t nicola_table_shftr[] PROGMEM = {
    {NG_W,        {KC_G, KC_A, 0}},  // "ga"
    {NG_E,        {KC_D, KC_A, 0}},  // "da"
    {NG_R,        {KC_G, KC_O, 0}},  // "go"
    {NG_T,        {KC_Z, KC_A, 0}},  // "za"
    {NG_Y,        {KC_Y, KC_O, 0}},  // "yo"
    {NG_U,        {KC_N, KC_I, 0}},  // "ni"
    {NG_I,        {KC_R, KC_U, 0}},  // "ru"
    {NG_O,        {KC_M, KC_A, 0}},  // "ma"
    {NG_P,        {KC_X, KC_E, 0}},  // "xe"
    {NG_LBRC,     {KC_MINS, 0, 0}},  // "-"
    {NG_RBRC,     {KC_EQL, 0, 0}},  // "="
    {NG_BSLS,     {KC_DEL, 0, 0}},
    {NG_A,        {KC_V, KC_U, 0}},  // "vu"
    {NG_S,        {KC_Z, KC_I, 0}},  // "zi"
    {NG_D,        {KC_D, KC_E, 0}},  // "de"
    {NG_F,        {KC_G, KC_E, 0}},  // "ge"
    {NG_G,        {KC_Z, KC_E, 0}},  // "ze"
    {NG_H,        {KC_M, KC_I, 0}},  // "mi"
    {NG_J,        {KC_O, 0, 0}},  // "o"
    {NG_K,        {KC_N, KC_O, 0}},  // "no"
    {NG_L,        {KC_X, KC_Y, KC_O}},  // "xyo"
    {NG_SCLN,     {KC_X, KC_T, KC_U}},  // "xtu"
    {NG_QUOT,     {KC_BSPC, 0, 0}},
    {NG_X,        {KC_B, KC_I, 0}},  // "bi"
    {NG_C,        {KC_Z, KC_U, 0}},  // "zu"
    {NG_V,        {KC_B, KC_U, 0}},  // "bu"
    {NG_B,        {KC_B, KC_E, 0}},  // "be"
    {NG_N,        {KC_N, KC_U, 0}},  // "nu"
    {NG_M,        {KC_Y, KC_U, 0}},  // "yu"
    {NG_COMM,     {KC_M, KC_U, 0}},  // "mu"
    {NG_DOT,      {KC_W, KC_A, 0}},  // "wa"
    {NG_SLSH,     {KC_X, KC_O, 0}},  // "xo"
    {NG_E_C,      {KC_HOME, 0, 0}},
    {NG_E_V,      {KC_PGDN, 0, 0}},
    {NG_E_N,      {KC_UP, 0, 0}},
    {NG_E_M,      {KC_RIGHT, 0, 0}},
};

static uint8_t table_find(const nicola_entry_t *table, uint8_t count, uint16_t ng, uint8_t out[NICOLA_MAX_OUT]) {
    for (uint8_t i = 0; i < count; i++) {
        if (pgm_read_word(&table[i].ng) != ng) {
            continue;
        }
        uint8_t n = 0;
        for (uint8_t j = 0; j < NICOLA_MAX_OUT; j++) {
            uint8_t code = pgm_read_byte(&table[i].out[j]);
            if (code == 0) {
                break;
            }
            out[n++] = code;
        }
        return n;
    }
    return 0;
}

uint8_t nicola_lookup(uint16_t m_key, uint16_t o_key, bool combined, uint8_t out[NICOLA_MAX_OUT]) {
    if (!combined) {
        return table_find(nicola_table_tap, ARRAY_SIZE(nicola_table_tap), m_key, out);
    }

    /* 同時打鍵は「左右共通の表」と「その親指専用の表」の 2 段引き。
     * 2 つの表にキーの重複はないので、どちらか一方しか当たらない。 */
    uint8_t n = table_find(nicola_table_both, ARRAY_SIZE(nicola_table_both), m_key, out);
    if (n > 0) {
        return n;
    }
    if (o_key == NG_SHFTL) {
        return table_find(nicola_table_shftl, ARRAY_SIZE(nicola_table_shftl), m_key, out);
    }
    if (o_key == NG_SHFTR) {
        return table_find(nicola_table_shftr, ARRAY_SIZE(nicola_table_shftr), m_key, out);
    }
    return 0;
}
