#pragma once

#include "quantum.h"

#define LAYOUT( \
      K00,  K01,  K02,  K03,  K04,  K05,  K06,  K07,  K08,  K09,  K0A,  K0B,  K0C,  K2C,  K4C, \
      K10,  K11,  K12,  K13,  K14,  K15,  K16,  K17,  K18,  K19,  K1A,  K1B,  K1C,  K3C,  K6C, \
      K20,  K21,  K22,  K23,  K24,  K25,  K26,  K27,  K28,  K29,  K2A,  K2B,  K3B,  K4B,  K5C, \
      K30,  K31,  K32,  K33,  K34,  K35,  K36,  K37,  K38,  K39,  K3A,  K4A,        K5A,  K5B, \
      K40,  K41,  K42,  K43,  K44,  K45,  K46,  K47,  K56,  K48,  K49,  K59,        K69,  K6B, \
      K50,  K51,  K52,              K55,                    K67,  K68,        K57,  K58,  K6A  \
) { \
    { K00,   K01,   K02,   K03,   K04,   K05,   K06,   K07,   K08,   K09,   K0A, K0B, K0C },  \
    { K10,   K11,   K12,   K13,   K14,   K15,   K16,   K17,   K18,   K19,   K1A, K1B, K1C },  \
    { K20,   K21,   K22,   K23,   K24,   K25,   K26,   K27,   K28,   K29,   K2A, K2B, K2C },  \
    { K30,   K31,   K32,   K33,   K34,   K35,   K36,   K37,   K38,   K39,   K3A, K3B, K3C },  \
    { K40,   K41,   K42,   K43,   K44,   K45,   K46,   K47,   K48,   K49,   K4A, K4B, K4C },  \
    { K50,   K51,   K52,   KC_NO, KC_NO, K55,   K56,   K57,   K58,   K59,   K5A, K5B, K5C },  \
    { KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, K67,   K68,   K69,   K6A, K6B, K6C },  \
}
