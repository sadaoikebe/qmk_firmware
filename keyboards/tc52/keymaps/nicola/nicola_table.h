/* NICOLA 出力テーブル
 *
 * Copyright 2020-2026 Sadao Ikebe @bonyarou
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * 旧実装は press 用と release 用に同じ表を 2 回ずつ、
 * さらに単独打鍵と同時打鍵で計 4 回書いていた (1091 行中の 6〜7 割)。
 *
 * ここでは表を 1 組だけ持ち、press では register_code、release では
 * unregister_code を回す。**ただし release は表を引き直さない。**
 * press したときに実際に register したキーコード列を記録しておき、
 * release ではそれをそのまま unregister する (nicola.c の nicola_held_t)。
 *
 * 引き直さない理由: press と release の間に is_nicola やモディファイア、
 * 保留中キーの内容が変わりうる。引き直すと press と release が非対称になり、
 * キーが押されっぱなしで残る。表の共通化で press/release の対応を
 * 崩さないための、これが肝になる。
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

/* 1 つの NICOLA キーが送出する基本キーコードの最大数。
 * "xya" (きゃ) や "xtu" (っ) の 3 個が最長 */
#define NICOLA_MAX_OUT 3

typedef struct {
    uint16_t ng;                    /* NG_* キーコード */
    uint8_t  out[NICOLA_MAX_OUT];   /* 送出する基本キーコード列。0 終端 */
} nicola_entry_t;

/* m_key (+ combined なら o_key) に対応する出力列を out[] に取り出す。
 *
 * @param combined false = 単独打鍵、true = 親指キーとの同時打鍵
 * @return 取り出したキーコードの個数。対応がなければ 0
 */
uint8_t nicola_lookup(uint16_t m_key, uint16_t o_key, bool combined, uint8_t out[NICOLA_MAX_OUT]);
