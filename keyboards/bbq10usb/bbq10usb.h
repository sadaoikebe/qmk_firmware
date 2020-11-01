#pragma once

#include "quantum.h"

#define LAYOUT( \
    K_Q, K_W, K_E, K_R, K_T, K_Y, K_U, K_I, K_O, K_P, \
    K_A, K_S, K_D, K_F, K_G, K_H, K_J, K_K, K_L, BSP, \
    ALT, K_Z, K_X, K_C, K_V, K_B, K_N, K_M, DLR, RET, \
    LSH, K_0, SPC, SYM, RSH \
) { \
    { K_Q, K_E, K_R, K_U, K_O }, \
    { K_W, K_S, K_G, K_H, K_L }, \
    { SYM, K_D, K_T, K_Y, K_I }, \
    { K_A, K_P, RSH, RET, BSP }, \
    { ALT, K_X, K_V, K_B, DLR }, \
    { SPC, K_Z, K_C, K_N, K_M }, \
    { K_0, LSH, K_F, K_J, K_K }, \
}
