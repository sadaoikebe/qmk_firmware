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

#pragma once

/*
 * Duplex matrix
 * =============
 *
 * 実配線は 5 行 x 6 列 = 11 ピン。各交点にダイオードを 2 個、向きを逆にして
 * 配置することで論理的に 10 行 (= 60 ポジション) を得ている。
 *
 *   論理行 0..4  ... COL2ROW 側 (COL = アノード, ROW = カソード)
 *   論理行 5..9  ... ROW2COL 側 (逆向き)
 *
 * 物理行ピン i が論理行 i (COL2ROW) と論理行 i+5 (ROW2COL) の両方を担当する。
 *
 * QMK 本体は duplex を知らないので matrix.c で自前スキャンする
 * (CUSTOM_MATRIX = lite)。よって keyboard.json には matrix_pins も
 * diode_direction も書かない。
 */
#define MATRIX_ROWS 10
#define MATRIX_COLS 6

/* 実際に配線されているピン。行は 5 本しかないことに注意 */
#define MATRIX_ROW_PINS { D2, D3, B5, B6, C6 }
#define MATRIX_COL_PINS { C7, D4, D5, D6, D7, B4 }

/* 物理行ピンの本数。MATRIX_ROWS の半分 */
#define MATRIX_ROW_PINS_COUNT (MATRIX_ROWS / 2)
