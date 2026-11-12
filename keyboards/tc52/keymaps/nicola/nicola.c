/* NICOLA 親指シフト 状態機械
 *
 * Copyright 2018-2019 eswai <@eswai>          (原型)
 * Copyright 2020-2026 Sadao Ikebe @bonyarou   (NICOLA 規格のタイミング判定・再実装)
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * 挙動の仕様は docs/NICOLA-SPEC.md。この実装は仕様書の第4節の遷移表を
 * そのままなぞっている。表と読み比べて検証できるようにするため、
 * 状態ごとの分岐は畳まずに素直な switch で書いてある。
 *
 * 解放時の逐次/同時識別は NICOLA 規格書 5.5.2（図7）に合わせる:
 *   is_sequential = (t1 > t2 * n) || (t2 < T_abs)
 *
 * 守ること:
 *
 *   - かな入力で「な」を押しっぱなしにすると「なあああああ」とリピートする。
 *     press/release を物理キーに厳密対応させた結果として必然的にこうなる。
 *   - tap_code() / tap_code16() は使わない。押しっぱなしが効かなくなる。
 */

#include <string.h>

#include "nicola.h"
#include "nicola_table.h"

#ifndef DEFERRED_EXEC_ENABLE
#    error "NICOLA requires DEFERRED_EXEC_ENABLE = yes in rules.mk"
#endif

/* ------------------------------------------------------------------
 * タイミング定数
 * ------------------------------------------------------------------ */

/* 同時打鍵とみなす最大間隔（規格の「同時打鍵判定時間」） */
#define NICOLA_TIMEOUT_MS 80

/* 重なり絶対下限 T_abs。t2 がこれ未満なら t1 によらず逐次打鍵
 * （規格 5.5.2 の補助案） */
#define NICOLA_OVERLAP_MS 20

/* 比係数 n。t1 > t2 * n なら逐次打鍵（規格 5.5.2 の本判定）。
 * 整数のみ。初期値 1 */
#define NICOLA_SEQUENTIAL_N 1

/* 追い越し防止バッファの段数 */
#define NICOLA_MAX_PENDING 8

/* ------------------------------------------------------------------
 * 状態
 * ------------------------------------------------------------------ */

typedef enum {
    S_INIT, /* 待機。保留中の打鍵なし */
    S_M,    /* 文字キー(M)を 1 つ保留中 */
    S_O,    /* 親指キー(O)を 1 つ保留中 */
    S_MO,   /* M → O の順。同時打鍵候補 */
    S_OM,   /* O → M の順。同時打鍵候補 */
} nicola_state_e;

static nicola_state_e nicola_int_state = S_INIT;

static bool    is_nicola    = false;
static uint8_t nicola_layer = 0;
/* かなモード中の modifier 数。英数モード中は使わず、ON 時に再取得する。 */
static uint8_t n_modifier   = 0;

static uint16_t nicola_m_key = KC_NO;
static uint16_t nicola_o_key = KC_NO;
static uint16_t nicola_m_time;
static uint16_t nicola_o_time;

/* ------------------------------------------------------------------
 * 出力の保持
 *
 * ここが press/release 対応の要。press で実際に register したキーコード列を
 * そのまま覚えておき、release ではテーブルを引き直さずにこれを unregister する。
 *
 * 引き直してはいけない理由: press と release の間に is_nicola、モディファイア、
 * 保留中キーが変わりうる。引き直すと press と release が食い違い、
 * キーが押されっぱなしで残る。
 *
 * active は「この枠は出力済み」の意味。テーブルに対応がなくて 1 個も
 * register しなかった場合 (count == 0) も出力済みとして扱う。
 * ------------------------------------------------------------------ */

typedef struct {
    uint8_t codes[NICOLA_MAX_OUT];
    uint8_t count;
    bool    active;
} nicola_held_t;

static nicola_held_t held_m;  /* M 単独 */
static nicola_held_t held_o;  /* O 単独 */
static nicola_held_t held_om; /* 同時打鍵 */

static void held_emit(nicola_held_t *h, const uint8_t *codes, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        register_code(codes[i]);
    }
    memcpy(h->codes, codes, count);
    h->count  = count;
    h->active = true;
}

/* register した順と同じ順で unregister する (旧実装と同じ) */
static void held_clear(nicola_held_t *h) {
    if (!h->active) {
        return;
    }
    for (uint8_t i = 0; i < h->count; i++) {
        unregister_code(h->codes[i]);
    }
    h->count  = 0;
    h->active = false;
}

/* 新しい出力を出す前に、まだ register されたままの出力を解放する。
 *
 * 必要な経路が実在する: S_M でタイムアウトして M を単独出力した後
 * (この時点で held_m は出力済み、状態は S_M のまま) に親指キーが来ると
 * S_MO に進む。その後の同時打鍵出力で held_m が残っていると二重に出る。 */
static void held_release_stale(void) {
    held_clear(&held_m);
    held_clear(&held_o);
    held_clear(&held_om);
}

/* ------------------------------------------------------------------
 * 追い越し防止バッファ
 *
 * 状態機械が動作中 (S_INIT 以外) に非 NICOLA の basic / QK_MODS キーが
 * 来たとき、それを先に出力すると順序が逆転する。register_code16 で再生できる
 * 出力だけをバッファに積み、保留中の打鍵が確定したときに一緒に吐き出す。
 *
 * 吐き出しは出力を伴う全ての操作の末尾から呼ぶ。
 * ------------------------------------------------------------------ */

typedef struct {
    uint16_t keycode;
    bool     pressed;
} nicola_pending_t;

static nicola_pending_t nicola_pending[NICOLA_MAX_PENDING];
static uint8_t          nicola_pending_count = 0;

static void nicola_flush_pending(void) {
    for (uint8_t i = 0; i < nicola_pending_count; i++) {
        if (nicola_pending[i].pressed) {
            register_code16(nicola_pending[i].keycode);
        } else {
            unregister_code16(nicola_pending[i].keycode);
        }
    }
    nicola_pending_count = 0;
}

/* ------------------------------------------------------------------
 * キー分類
 * ------------------------------------------------------------------ */

static bool is_nicola_m_key(uint16_t keycode) {
    return keycode >= NG_M_TOP && keycode <= NG_M_BOTTOM;
}

static bool is_nicola_o_key(uint16_t keycode) {
    return keycode == NG_SHFTL || keycode == NG_SHFTR;
}

static bool is_nicola_key(uint16_t keycode) {
    return keycode >= NG_TOP && keycode <= NG_BOTTOM;
}

static bool is_replayable_key(uint16_t keycode) {
    return IS_QK_BASIC(keycode) || IS_QK_MODS(keycode);
}

static bool is_nicola_left_finger(uint16_t keycode) {
    return keycode >= NG_LEFT_FINGER_TOP && keycode <= NG_LEFT_FINGER_BOTTOM;
}

static bool is_nicola_right_finger(uint16_t keycode) {
    return keycode >= NG_RIGHT_FINGER_TOP && keycode <= NG_RIGHT_FINGER_BOTTOM;
}

static bool is_nicola_eisu(uint16_t keycode) {
    return (keycode >= NG_EISU1_TOP && keycode <= NG_EISU1_BOTTOM) || (keycode >= NG_EISU2_TOP && keycode <= NG_EISU2_BOTTOM);
}

/* 打鍵した指と反対側の親指を使ったか。
 * カーソルキーは左右どちらの範囲にも入らないので常に false になる (仕様) */
static bool is_cross_shift(uint16_t m, uint16_t o) {
    return (is_nicola_left_finger(m) && o == NG_SHFTR) || (is_nicola_right_finger(m) && o == NG_SHFTL);
}

/* ------------------------------------------------------------------
 * 小指シフト (通常の Shift キー) の反転
 *
 * 親指シフトと小指シフトの組み合わせを扱うための処理。詳細は
 * NICOLA-SPEC.md 第5-3節。旧実装は (cross != is_nicola) の XOR 1 行だったが、
 * 意図が読めないのでモードごとに分けて書く。挙動は同一。
 * ------------------------------------------------------------------ */

static bool nicola_compute_invert(void) {
    if (is_nicola_eisu(nicola_m_key)) {
        switch (nicola_m_key) {
            /* カーソルキーを出す 4 つは例外。かなモードのときだけ反転 */
            case NG_E_C:
            case NG_E_V:
            case NG_E_N:
            case NG_E_M:
                return is_nicola;
            default:
                break;
        }

        bool cross = is_cross_shift(nicola_m_key, nicola_o_key);
        if (is_nicola) {
            /* かなモード: この経路に来た時点で小指シフトが押されている
             * (IME がかなモード中に Shift で一時的に英数入力になるため)。
             * 同側シフトなら小指シフトを打ち消して素直に数字を出す。 */
            return !cross;
        }
        /* 英数モード: クロスシフトのときだけ Shift を足して記号を出す */
        return cross;
    }

    /* かなキー側の限定的な反転。左親指 + [ ] / で "_" "+" "?" を出す */
    if (nicola_o_key == NG_SHFTL) {
        switch (nicola_m_key) {
            case NG_LBRC:
            case NG_RBRC:
            case NG_SLSH:
                return true;
            default:
                break;
        }
    }
    return false;
}

/* ------------------------------------------------------------------
 * 出力アクション
 *
 * *_press()   … 押しっぱなしで出力する (register するだけ)
 * *_release() … 確定して離す。まだ出していなければ出してから離す
 * ------------------------------------------------------------------ */

static void nicola_m_press(void) {
    held_release_stale();

    uint8_t out[NICOLA_MAX_OUT];
    uint8_t n = nicola_lookup(nicola_m_key, KC_NO, false, out);
    held_emit(&held_m, out, n);

    nicola_flush_pending();
}

static void nicola_m_release(void) {
    if (!held_m.active) {
        nicola_m_press();
    }
    held_clear(&held_m);
    nicola_flush_pending();
}

static void nicola_o_press(void) {
    held_release_stale();

    uint8_t out[NICOLA_MAX_OUT];
    uint8_t n = 0;
    if (is_nicola_o_key(nicola_o_key)) {
        out[n++] = KC_SPC;
    }
    held_emit(&held_o, out, n);

    nicola_flush_pending();
}

static void nicola_o_release(void) {
    if (!held_o.active) {
        nicola_o_press();
    }
    held_clear(&held_o);
    nicola_flush_pending();
}

static void nicola_om_press(void) {
    held_release_stale();

    bool    invert     = nicola_compute_invert();
    uint8_t saved_mods = 0;
    if (invert) {
        saved_mods         = get_mods();
        uint8_t new_mods   = saved_mods;
        uint8_t shift_bits = MOD_BIT(KC_LSFT) | MOD_BIT(KC_RSFT);
        if (saved_mods & shift_bits) {
            new_mods &= ~shift_bits;
        } else {
            new_mods |= MOD_BIT(KC_LSFT);
        }
        set_mods(new_mods);
    }

    uint8_t out[NICOLA_MAX_OUT];
    uint8_t n = nicola_lookup(nicola_m_key, nicola_o_key, true, out);
    held_emit(&held_om, out, n);

    /* モディファイアは出力直後に戻す。キー自体は押されたまま残るが、
     * シフトが効くのは register した瞬間のレポートだけでよい (旧実装と同じ) */
    if (invert) {
        set_mods(saved_mods);
    }

    nicola_flush_pending();
}

static void nicola_om_release(void) {
    if (!held_om.active) {
        nicola_om_press();
    }
    held_clear(&held_om);
    nicola_flush_pending();
}

/* ------------------------------------------------------------------
 * タイムアウト
 *
 * 旧実装は AVR の TIMER1 を直接叩いて ISR から呼んでいたため、本処理と
 * 競合しないよう key_process_guard という排他フラグが必要だった。
 *
 * defer_exec() のコールバックはメインループ (housekeeping) から呼ばれる。
 * process_record_user() と同じ文脈なので割り込むことがなく、排他は不要。
 * ------------------------------------------------------------------ */

static deferred_token nicola_timeout_token = INVALID_DEFERRED_TOKEN;

/* 保留中の打鍵を「押しっぱなしのまま」出力する。状態は変えない。
 *
 * 状態を変えないのが重要。ユーザが物理キーを離したときに解放側の遷移が走り、
 * *_release() が「既に出力済み」を見て unregister だけ行う。
 * これで出力キーの押下期間が物理キーの押下期間と一致する。 */
static void nicola_trigger(void) {
    switch (nicola_int_state) {
        case S_INIT:
            break;
        case S_M:
            nicola_m_press();
            break;
        case S_O:
            nicola_o_press();
            break;
        case S_MO:
        case S_OM:
            nicola_om_press();
            break;
    }
}

static uint32_t nicola_timeout_callback(uint32_t trigger_time, void *cb_arg) {
    nicola_timeout_token = INVALID_DEFERRED_TOKEN;
    nicola_trigger();
    return 0; /* 繰り返さない */
}

static void nicola_timer_restart(void) {
    if (nicola_timeout_token != INVALID_DEFERRED_TOKEN) {
        if (extend_deferred_exec(nicola_timeout_token, NICOLA_TIMEOUT_MS)) {
            return;
        }
        nicola_timeout_token = INVALID_DEFERRED_TOKEN;
    }
    nicola_timeout_token = defer_exec(NICOLA_TIMEOUT_MS, nicola_timeout_callback, NULL);
}

static void nicola_timer_cancel(void) {
    if (nicola_timeout_token != INVALID_DEFERRED_TOKEN) {
        cancel_deferred_exec(nicola_timeout_token);
        nicola_timeout_token = INVALID_DEFERRED_TOKEN;
    }
}

static void nicola_goto_init(void) {
    nicola_int_state = S_INIT;
    nicola_timer_cancel();
}

/* ------------------------------------------------------------------
 * 3 キー判定
 *
 * 保留中の 2 打鍵のうち、先に押した方を first、後を second として
 *   t1 = 2 打鍵の間隔
 *   t2 = 後の打鍵から今までの間隔
 * を求める。t1 < t2 なら「保留中の 2 つは結合する」、
 * t1 >= t2 なら「後の打鍵は次のキーと結合する」。
 *
 * 旧実装は S4_MO と S5_OM で参照する時刻が入れ替わり、同じ t1/t2 という
 * 名前が別の意味を持っていた。ここでは呼ぶ側が first/second を渡す。
 * ------------------------------------------------------------------ */

static void nicola_intervals(uint16_t first, uint16_t second, uint16_t now, uint16_t *t1, uint16_t *t2) {
    *t1 = second - first;
    *t2 = now - second;
}

/* 規格 5.5.2（図7）: 逐次打鍵と同時打鍵の識別。
 *
 *   t1 = 先キー ON → 後キー ON
 *   t2 = 後キー ON → 今（離したキーの OFF 時刻）
 *
 * ON 間隔だけ見ると判定時間内は必ず同時に見えるが、短い重なりは
 * 英文タイプのロールオーバー（文字→Space など）でも起きる。
 * 比判定を主、絶対短い t2 を補助とする。 */
static bool nicola_is_sequential(uint16_t t1, uint16_t t2) {
    /* t2 * n は uint32 で計算し、n を上げたときのオーバーフローを避ける */
    return ((uint32_t)t1 > (uint32_t)t2 * NICOLA_SEQUENTIAL_N) || (t2 < NICOLA_OVERLAP_MS);
}

/* ------------------------------------------------------------------
 * 遷移: M キー押下   (NICOLA-SPEC.md 4-1)
 * ------------------------------------------------------------------ */

static void nicola_on_m_press(uint16_t keycode, uint16_t now) {
    uint16_t t1, t2;

    switch (nicola_int_state) {
        case S_INIT:
            nicola_int_state = S_M;
            break;

        case S_M:
            /* 前の M は単独で確定 */
            nicola_m_release();
            nicola_int_state = S_M;
            break;

        case S_O:
            nicola_int_state = S_OM;
            break;

        case S_MO:
            nicola_intervals(nicola_m_time, nicola_o_time, now, &t1, &t2);
            if (t1 < t2) {
                /* 間の O は前の M と結合。今の M を新規保留 */
                nicola_om_release();
                nicola_int_state = S_M;
            } else {
                /* 先頭 M は単独。O は今の M と結合させる */
                nicola_m_release();
                nicola_int_state = S_OM;
            }
            break;

        case S_OM:
            nicola_om_release();
            nicola_int_state = S_M;
            break;
    }

    nicola_m_key  = keycode;
    nicola_m_time = now;
    nicola_timer_restart();
}

/* ------------------------------------------------------------------
 * 遷移: O キー押下   (NICOLA-SPEC.md 4-2)
 * ------------------------------------------------------------------ */

static void nicola_on_o_press(uint16_t keycode, uint16_t now) {
    uint16_t t1, t2;

    switch (nicola_int_state) {
        case S_INIT:
            nicola_int_state = S_O;
            break;

        case S_M:
            nicola_int_state = S_MO;
            break;

        case S_O:
            /* 前の O は単独で確定 */
            nicola_o_release();
            nicola_int_state = S_O;
            break;

        case S_MO:
            nicola_om_release();
            nicola_int_state = S_O;
            break;

        case S_OM:
            nicola_intervals(nicola_o_time, nicola_m_time, now, &t1, &t2);
            if (t1 < t2) {
                nicola_om_release();
                nicola_int_state = S_O;
            } else {
                /* 先頭 O は単独。M は今の O と結合させる */
                nicola_o_release();
                nicola_int_state = S_MO;
            }
            break;
    }

    nicola_o_key  = keycode;
    nicola_o_time = now;
    nicola_timer_restart();
}

/* ------------------------------------------------------------------
 * 遷移: M / O キー解放   (NICOLA-SPEC.md 4-3 / 規格 5.5.2)
 *
 * S_MO / S_OM では nicola_is_sequential() で逐次と同時を分ける。
 * 逐次なら離した側だけ単独確定し、残りのキーを 1 段戻して保留する。
 * 後キー解放にも同じ式を対称適用する（現行は後キー解放を常に同時にしていた）。
 * ------------------------------------------------------------------ */

static void nicola_on_release(uint16_t keycode, uint16_t now) {
    uint16_t t1, t2;

    switch (nicola_int_state) {
        case S_INIT:
            break;

        case S_M:
            if (nicola_m_key == keycode) {
                nicola_m_release();
                nicola_goto_init();
            }
            break;

        case S_O:
            if (nicola_o_key == keycode) {
                nicola_o_release();
                nicola_goto_init();
            }
            break;

        case S_MO:
            if (nicola_m_key == keycode) {
                /* 図7本線: M→O のあと先キー(M)を離す */
                nicola_intervals(nicola_m_time, nicola_o_time, now, &t1, &t2);
                if (nicola_is_sequential(t1, t2)) {
                    nicola_m_release();
                    nicola_int_state = S_O;
                } else {
                    nicola_om_release();
                    nicola_goto_init();
                }
            } else if (nicola_o_key == keycode) {
                /* 後キー(O)解放。同じ識別式を対称適用 */
                nicola_intervals(nicola_m_time, nicola_o_time, now, &t1, &t2);
                if (nicola_is_sequential(t1, t2)) {
                    nicola_o_release();
                    nicola_int_state = S_M;
                } else {
                    nicola_om_release();
                    nicola_goto_init();
                }
            }
            break;

        case S_OM:
            if (nicola_o_key == keycode) {
                /* O→M のあと先キー(O)を離す */
                nicola_intervals(nicola_o_time, nicola_m_time, now, &t1, &t2);
                if (nicola_is_sequential(t1, t2)) {
                    nicola_o_release();
                    nicola_int_state = S_M;
                } else {
                    nicola_om_release();
                    nicola_goto_init();
                }
            } else if (nicola_m_key == keycode) {
                /* 後キー(M)解放。同じ識別式を対称適用 */
                nicola_intervals(nicola_o_time, nicola_m_time, now, &t1, &t2);
                if (nicola_is_sequential(t1, t2)) {
                    nicola_m_release();
                    nicola_int_state = S_O;
                } else {
                    nicola_om_release();
                    nicola_goto_init();
                }
            }
            break;
    }
}

/* ------------------------------------------------------------------
 * 遷移: NICOLA 以外のキー   (NICOLA-SPEC.md 4-5)
 * ------------------------------------------------------------------ */

static bool nicola_on_other(uint16_t keycode, bool pressed) {
    if (nicola_int_state == S_INIT) {
        return true; /* 保留がないのでそのまま QMK に渡す */
    }

    if (nicola_pending_count < NICOLA_MAX_PENDING) {
        nicola_pending[nicola_pending_count].keycode = keycode;
        nicola_pending[nicola_pending_count].pressed = pressed;
        nicola_pending_count++;
        return false; /* 飲み込む。保留が確定したときに一緒に出る */
    }

    /* バッファが溢れた。保留中の打鍵を吐き出してからこのキーを通す */
    nicola_trigger();
    return true;
}

/* ------------------------------------------------------------------
 * 公開 API
 * ------------------------------------------------------------------ */

bool process_nicola(uint16_t keycode, keyrecord_t *record) {
    if (!is_nicola_key(keycode) && !is_replayable_key(keycode)) {
        return true;
    }

    uint16_t now = timer_read();

    if (record->event.pressed) {
        if (is_nicola_m_key(keycode)) {
            nicola_on_m_press(keycode, now);
            return false;
        }
        if (is_nicola_o_key(keycode)) {
            nicola_on_o_press(keycode, now);
            return false;
        }
        return nicola_on_other(keycode, true);
    }

    if (is_nicola_key(keycode)) {
        nicola_on_release(keycode, now);
        return false;
    }
    return nicola_on_other(keycode, false);
}

/* [ゲート2] かなモード中に modifier を押している間は NICOLA レイヤーを外し、
 * 素の QWERTY を露出させる。Ctrl+Shift+X のような複数同時でも戻せるよう
 * 押されている数を数える。英数中の変化は nicola_on() で反映する。 */
void nicola_mode(uint16_t keycode, keyrecord_t *record) {
    if (!is_nicola) {
        return;
    }

    switch (keycode) {
        case KC_LCTL:
        case KC_LSFT:
        case KC_LALT:
        case KC_LGUI:
        case KC_RCTL:
        case KC_RSFT:
        case KC_RALT:
        case KC_RGUI:
            if (record->event.pressed) {
                if (n_modifier == 0) {
                    layer_off(nicola_layer);
                }
                n_modifier++;
            } else if (n_modifier > 0) {
                n_modifier--;
                if (n_modifier == 0) {
                    layer_on(nicola_layer);
                }
            }
            break;
        default:
            break;
    }
}

void nicola_clear(void) {
    switch (nicola_int_state) {
        case S_INIT:
            break;
        case S_M:
            nicola_m_release();
            break;
        case S_O:
            nicola_o_release();
            break;
        case S_MO:
        case S_OM:
            nicola_om_release();
            break;
    }
    nicola_goto_init();
}

void set_nicola(uint8_t layer) {
    nicola_layer = layer;
    nicola_m_key = KC_NO;
    nicola_o_key = KC_NO;
}

void nicola_on(void) {
    nicola_clear();
    n_modifier = bitpop(get_mods());
    is_nicola = true;
    if (n_modifier == 0) {
        layer_on(nicola_layer);
    } else {
        layer_off(nicola_layer);
    }
}

void nicola_off(void) {
    nicola_clear();
    is_nicola = false;
    layer_off(nicola_layer);
}

bool nicola_state(void) {
    return is_nicola;
}
