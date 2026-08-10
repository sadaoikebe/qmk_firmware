/*
 * (C) 2020-2026 Sadao Ikebe @bonyarou
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
 * Duplex matrix scan
 * ==================
 *
 * 各交点にダイオードを 2 個、向きを逆にして配置することで、行ピン 1 本に
 * 論理行 2 本を担わせている。スキャンは 2 パス:
 *
 *   パス1 (COL2ROW): 行ピン i を駆動し列を読む  -> 論理行 i        (i = 0..4)
 *   パス2 (ROW2COL): 列ピン j を駆動し行を読む  -> 論理行 5 + i    (j = 0..5)
 *
 * 旧フォークではこれを quantum/matrix.c に DIODE_DIRECTION = BOTHWAYS として
 * 直接実装していたが、それがフォークを維持する唯一の理由になっていた。
 * CUSTOM_MATRIX = lite でここに置くことで QMK 本体への差分がゼロになる。
 */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "matrix.h"
#include "gpio.h"
#include "util.h"
#include "atomic_util.h"

/* 押されたときの入力ピンの状態。quantum/matrix.c の既定値に合わせる */
#ifndef MATRIX_INPUT_PRESSED_STATE
#    define MATRIX_INPUT_PRESSED_STATE 0
#endif

/* gpio_atomic_* は公開 API ではなく quantum/matrix.c 内の static inline なので、
 * 自前マトリクスからは使えない。同じものをここに置く。
 * 方向とレベルの設定を割り込みで分断されると、一瞬だけ意図しない出力になる。 */
static inline void gpio_atomic_set_pin_output_low(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        gpio_set_pin_output(pin);
        gpio_write_pin_low(pin);
    }
}

static inline void gpio_atomic_set_pin_input_high(pin_t pin) {
    ATOMIC_BLOCK_FORCEON {
        gpio_set_pin_input_high(pin);
    }
}

static const pin_t row_pins[] = MATRIX_ROW_PINS;
static const pin_t col_pins[] = MATRIX_COL_PINS;

/* 実際に配線されている行ピンの本数。論理行数の半分 */
#define NUM_ROW_PINS (MATRIX_ROWS / 2)

_Static_assert(NUM_ROW_PINS * 2 == MATRIX_ROWS, "MATRIX_ROWS must be even for a duplex matrix");
_Static_assert(ARRAY_SIZE(row_pins) == NUM_ROW_PINS, "MATRIX_ROW_PINS must have exactly MATRIX_ROWS/2 entries");
_Static_assert(ARRAY_SIZE(col_pins) == MATRIX_COLS, "MATRIX_COL_PINS must have exactly MATRIX_COLS entries");

static inline uint8_t read_matrix_pin(pin_t pin) {
    if (pin == NO_PIN) {
        return 1;
    }
    return (gpio_read_pin(pin) == MATRIX_INPUT_PRESSED_STATE) ? 0 : 1;
}

static inline void select_row(uint8_t row) {
    gpio_atomic_set_pin_output_low(row_pins[row]);
}

static inline void unselect_row(uint8_t row) {
    gpio_atomic_set_pin_input_high(row_pins[row]);
}

static inline void select_col(uint8_t col) {
    gpio_atomic_set_pin_output_low(col_pins[col]);
}

static inline void unselect_col(uint8_t col) {
    gpio_atomic_set_pin_input_high(col_pins[col]);
}

/* 2 パス方式なので、行ピンも列ピンも「駆動していないときは入力プルアップ」に
 * しておく必要がある。片方だけ unselect すると、もう一方のパスで
 * 出力同士がぶつかる。 */
static void unselect_all(void) {
    for (uint8_t row = 0; row < NUM_ROW_PINS; row++) {
        unselect_row(row);
    }
    for (uint8_t col = 0; col < MATRIX_COLS; col++) {
        unselect_col(col);
    }
}

/* パス1: 行を駆動して列を読む -> 論理行 0..NUM_ROW_PINS-1 */
static void read_cols_on_row(matrix_row_t current_matrix[], uint8_t row) {
    matrix_row_t value = 0;

    select_row(row);
    matrix_output_select_delay();

    matrix_row_t shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col = 0; col < MATRIX_COLS; col++, shifter <<= 1) {
        value |= read_matrix_pin(col_pins[col]) ? 0 : shifter;
    }

    unselect_row(row);
    matrix_output_unselect_delay(row, value != 0);

    current_matrix[row] = value;
}

/* パス2: 列を駆動して行を読む -> 論理行 NUM_ROW_PINS..MATRIX_ROWS-1
 * 列ごとに 1 ビットずつ立てる/落とすので、行単位の代入ではなくビット操作になる。 */
static void read_rows_on_col(matrix_row_t current_matrix[], uint8_t col, matrix_row_t shifter) {
    bool key_pressed = false;

    select_col(col);
    matrix_output_select_delay();

    for (uint8_t row = 0; row < NUM_ROW_PINS; row++) {
        if (read_matrix_pin(row_pins[row]) == 0) {
            current_matrix[NUM_ROW_PINS + row] |= shifter;
            key_pressed = true;
        } else {
            current_matrix[NUM_ROW_PINS + row] &= ~shifter;
        }
    }

    unselect_col(col);
    matrix_output_unselect_delay(col, key_pressed);
}

void matrix_init_custom(void) {
    unselect_all();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    matrix_row_t previous[MATRIX_ROWS];
    memcpy(previous, current_matrix, sizeof(previous));

    for (uint8_t row = 0; row < NUM_ROW_PINS; row++) {
        read_cols_on_row(current_matrix, row);
    }

    matrix_row_t shifter = MATRIX_ROW_SHIFTER;
    for (uint8_t col = 0; col < MATRIX_COLS; col++, shifter <<= 1) {
        read_rows_on_col(current_matrix, col, shifter);
    }

    return memcmp(previous, current_matrix, sizeof(previous)) != 0;
}
