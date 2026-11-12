/*
 * TC52 (TS52K capacitive / EC) matrix — ported from the rebuilt TC69 matrix (2026-11-12).
 *
 * Origin: firmware-rebuild/keyboards/tc69/matrix.c, itself sadaoikebe/qmk_firmware
 *         branch "capacitive" @ 690c285d83 keyboards/tc69/matrix.c (see firmware-rebuild.md).
 * Details of this port: ts52k-ec/tc52-firmware.md
 *
 * The sensing and judgement logic is TC69's, unchanged:
 *   - per crossing: PURGE output LOW -> wait_us(20) -> PURGE input (Hi-Z) -> row HIGH ->
 *     wait_us(1) -> analogReadPin(SENSE) -> row LOW. Column address outside, rows inside.
 *   - columns are visited in TC69's address order (MUX A X0..X7, then MUX B X0..X7) and the
 *     address pins are written in TC69's order (S1, S2, S3, S0).
 *   - every crossing of the matrix is read, including the 4 crossings without an electrode.
 *   - thresholds 120/30 per sample (x3 frames); the home key (TC69's A key) 220/80.
 *   - start-up calibration: 3 frames discarded, sum of the next 3 frames = baseline.
 *   - judgement: 3-frame ring-buffer sum vs. baseline.
 *   - NOTE (kept on purpose, see firmware-audit.md §3.3): curr_matrix is zeroed on every
 *     scan, so the release threshold branch never has an effect. The effective behaviour is
 *     a single threshold (press threshold) on a 3-frame moving sum. This is how TC69 ran.
 *
 * What differs from TC69 (board only): pins (design-spec.md §6.1), 14 columns x 4 rows with a
 * column -> MUX input table (tc52_matrix.h, generated), home key position, and one extra
 * matrix row (row 4) carrying the encoder push switch, which is a plain GPIO on TC52.
 * Optional raw-ADC diagnostics are compiled in only with TC52_ADC_DIAG (default OFF).
 */
#include QMK_KEYBOARD_H

#include "analog.h"
#include "tc52_matrix.h"

#define ADC_RING_BUFFER_SIZE 3

#define CAP_COLS MATRIX_COLS /* 14 sensed columns */
#define CAP_ROWS 4           /* 4 strobe rows; matrix row 4 is the encoder switch */
#define ENC_SW_ROW 4

_Static_assert(MATRIX_COLS == 14, "TC52 senses 14 columns via 2x 4051");
_Static_assert(MATRIX_ROWS == CAP_ROWS + 1, "4 strobe rows + 1 direct-GPIO row (encoder switch)");

/* design-spec.md §6.1 */
const pin_t CAP_ROW_PINS[] = { GP18, GP19, GP20, GP21 };   /* ROW1..ROW4 */
/* TC69 order: S1, S2, S3 (MUX input X bit0..2), then S0 (MUX select) */
const pin_t CAP_COL_PINS[] = { GP23, GP24, GP25, GP22 };
#define CAP_PURGE GP26
#define CAP_SENSE GP28 /* ADC2 = op-amp output */
#define TIE_LOW_1 GP27 /* unused, driven LOW to keep PURGE / ADC neighbours quiet */
#define TIE_LOW_2 GP29
#define ENC_SW_PIN GP2 /* encoder push switch to GND, internal pull-up */

static const uint8_t col_x[CAP_COLS]     = TC52_COL_X;
static const uint8_t col_mux_b[CAP_COLS] = TC52_COL_MUX_B;
static const uint8_t scan_order[CAP_COLS] = TC52_SCAN_ORDER;

const int adc_press_threshold = 120 * ADC_RING_BUFFER_SIZE;
const int adc_release_threshold = 30 * ADC_RING_BUFFER_SIZE;

const int adc_press_threshold_2 = 220 * ADC_RING_BUFFER_SIZE;
const int adc_release_threshold_2 = 80 * ADC_RING_BUFFER_SIZE;

static int adc_ring_index = 0;

int adc_ring_buffer[ADC_RING_BUFFER_SIZE][CAP_COLS][CAP_ROWS];
int calibration_val[CAP_COLS][CAP_ROWS];
matrix_row_t matrix[MATRIX_ROWS];

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

bool is_home_position(int i, int j) {
  /* TC69: "I often lean my left pinky on the A key" -> A key of TS52K (MX8) */
  return (i == TC52_HOME_COL) && (j == TC52_HOME_ROW);
}

void acquire_adc(int adc_measured_val[][CAP_ROWS]) {
  for(int s=0; s<CAP_COLS; ++s) {
    int j = scan_order[s];
    gpio_write_pin(CAP_COL_PINS[0], (col_x[j]&1) ? true : false);
    gpio_write_pin(CAP_COL_PINS[1], (col_x[j]&2) ? true : false);
    gpio_write_pin(CAP_COL_PINS[2], (col_x[j]&4) ? true : false);
    gpio_write_pin(CAP_COL_PINS[3], col_mux_b[j] ? true : false);
    for(int i=0; i<CAP_ROWS; ++i) {
       gpio_set_pin_output(CAP_PURGE);
       gpio_write_pin_low(CAP_PURGE);
       wait_us(20);
       gpio_set_pin_input(CAP_PURGE);

       gpio_write_pin_high(CAP_ROW_PINS[i]);
       wait_us(1);
       adc_measured_val[j][i] = analogReadPin(CAP_SENSE);
       gpio_write_pin_low(CAP_ROW_PINS[i]);
    }
  }
}

#ifdef TC52_ADC_DIAG
static void adc_diag_init(void);
static void adc_diag_after_scan(void);
#endif

void matrix_init(void) {
   gpio_set_pin_output(GP18);
   gpio_set_pin_output(GP19);
   gpio_set_pin_output(GP20);
   gpio_set_pin_output(GP21);
   gpio_set_pin_output(GP23);
   gpio_set_pin_output(GP24);
   gpio_set_pin_output(GP25);
   gpio_set_pin_output(GP22);
   gpio_set_pin_input(CAP_PURGE);
   gpio_set_pin_input(CAP_SENSE);
   /* TC52 board additions (design-spec.md §6.1): not part of the sensing path */
   gpio_set_pin_output(TIE_LOW_1);
   gpio_write_pin_low(TIE_LOW_1);
   gpio_set_pin_output(TIE_LOW_2);
   gpio_write_pin_low(TIE_LOW_2);
   gpio_set_pin_input_high(ENC_SW_PIN);

  int adc_measured_val[CAP_COLS][CAP_ROWS];

  for(int i=0; i<MATRIX_ROWS; ++i) {
    matrix[i] = 0;
  }
  for(int i=0; i<CAP_ROWS; ++i) {
    for(int j=0; j<CAP_COLS; ++j) {
      adc_measured_val[j][i] = 0;
      calibration_val[j][i] = 0;
      for(int k=0; k<ADC_RING_BUFFER_SIZE; ++k) {
        adc_ring_buffer[k][j][i] = 0;
      }
    }
  }

  for(int i=0; i<ADC_RING_BUFFER_SIZE; ++i) {
    acquire_adc(adc_measured_val);
  }

  for(int i=0; i<ADC_RING_BUFFER_SIZE; ++i) {
    acquire_adc(adc_measured_val);
    for(int j=0; j<CAP_COLS; ++j) {
      for(int k=0; k<CAP_ROWS; ++k) {
        calibration_val[j][k] += adc_measured_val[j][k];
      }
    }
  }

#ifdef TC52_ADC_DIAG
  adc_diag_init();
#endif

  matrix_init_kb();
}

uint8_t matrix_scan(void) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};
    acquire_adc(adc_ring_buffer[adc_ring_index]);
    adc_ring_index = (adc_ring_index + 1) % ADC_RING_BUFFER_SIZE;

    for(int i=0; i<CAP_COLS; ++i) {
      for(int j=0; j<CAP_ROWS; ++j) {
        int adc_total = 0;
        for(int k=0; k<ADC_RING_BUFFER_SIZE; ++k) {
          adc_total += adc_ring_buffer[k][i][j];
        }

        bool home_position = is_home_position(i, j);
        int press_threshold = home_position ? adc_press_threshold_2 : adc_press_threshold;
        int release_threshold = home_position ? adc_release_threshold_2 : adc_release_threshold;

        if(adc_total > calibration_val[i][j] + press_threshold) {
          curr_matrix[j] |= (1 << i);
        }
        else if(adc_total < calibration_val[i][j] + release_threshold) {
          curr_matrix[j] &= ~(1 << i);
        }
      }
    }

    /* Encoder push switch (TC52 only; TC69 had none). Raw GPIO, active low, no debounce
     * (CUSTOM_MATRIX = yes has no debounce stage). Mapped to KC_NO in the TS52K keymaps. */
    if (!gpio_read_pin(ENC_SW_PIN)) {
      curr_matrix[ENC_SW_ROW] |= 1;
    }

    bool changed = memcmp(matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) memcpy(matrix, curr_matrix, sizeof(curr_matrix));

#ifdef TC52_ADC_DIAG
    adc_diag_after_scan();
#endif

    matrix_scan_kb();

    return changed;
}

matrix_row_t matrix_get_row(uint8_t row) {
  return matrix[row];
}

void matrix_print(void)
{
}

/* ------------------------------------------------------------------------
 * Raw-ADC diagnostics (only with TC52_ADC_DIAG; build with TC52_ADC_DIAG=yes)
 *
 * Same observer as the TC69 rebuild. Read-only: it only reads adc_ring_buffer /
 * calibration_val after the judgement above, and never changes thresholds or matrix
 * state. Every TC52_ADC_DIAG_INTERVAL_MS it prints over the QMK console:
 *
 *   #ADC seq=<n> t=<ms> scans=<scans in interval> scan_hz=<scans per second>
 *   CAL r<row>: <14 values>  calibration sum / 3  (= baseline, 10-bit counts)
 *   RAW r<row>: <14 values>  last single frame
 *   MIN r<row>: <14 values>  min single frame since the previous dump
 *   MAX r<row>: <14 values>  max single frame since the previous dump
 *   D3  r<row>: <14 values>  latest (3-frame sum - calibration sum); a key is
 *                            "pressed" when D3 > 360 (home key: > 660)
 *   PRS r<row>: <hex>        matrix row bitmap after judgement (bit = column)
 *   CHK lo=<n> hi=<n>: r<row>c<col>=<baseline> ...
 *                            baselines outside 5..600 (design-spec.md §6.3; report only)
 *
 * Values are indexed by logical column 0..13 (= design-spec c1..c14, see tc52_matrix.h),
 * rows 0..3 (ROW1..ROW4). The 4 crossings without an electrode are printed too.
 * ------------------------------------------------------------------------ */
#ifdef TC52_ADC_DIAG
#    include "print.h"
#    include "timer.h"

#    ifndef TC52_ADC_DIAG_INTERVAL_MS
#        define TC52_ADC_DIAG_INTERVAL_MS 1000
#    endif
#    define DIAG_BASELINE_LO 5
#    define DIAG_BASELINE_HI 600

static int16_t  diag_min[CAP_COLS][CAP_ROWS];
static int16_t  diag_max[CAP_COLS][CAP_ROWS];
static uint32_t diag_last_dump;
static uint32_t diag_scans;
static uint32_t diag_seq;

static void adc_diag_reset_minmax(void) {
    for (int c = 0; c < CAP_COLS; ++c) {
        for (int r = 0; r < CAP_ROWS; ++r) {
            diag_min[c][r] = INT16_MAX;
            diag_max[c][r] = INT16_MIN;
        }
    }
}

static void adc_diag_init(void) {
    adc_diag_reset_minmax();
    diag_last_dump = timer_read32();
    diag_scans     = 0;
    diag_seq       = 0;
}

static void adc_diag_after_scan(void) {
    /* the frame just written is the one before the (already advanced) ring index */
    int last = (adc_ring_index + ADC_RING_BUFFER_SIZE - 1) % ADC_RING_BUFFER_SIZE;

    ++diag_scans;
    for (int c = 0; c < CAP_COLS; ++c) {
        for (int r = 0; r < CAP_ROWS; ++r) {
            int v = adc_ring_buffer[last][c][r];
            if (v < diag_min[c][r]) diag_min[c][r] = v;
            if (v > diag_max[c][r]) diag_max[c][r] = v;
        }
    }

    uint32_t elapsed = timer_elapsed32(diag_last_dump);
    if (elapsed < TC52_ADC_DIAG_INTERVAL_MS) return;

    uprintf("#ADC seq=%lu t=%lu scans=%lu scan_hz=%lu\n", (unsigned long)diag_seq, (unsigned long)timer_read32(), (unsigned long)diag_scans, (unsigned long)(diag_scans * 1000UL / (elapsed ? elapsed : 1)));
    for (int r = 0; r < CAP_ROWS; ++r) {
        uprintf("CAL r%d:", r);
        for (int c = 0; c < CAP_COLS; ++c) uprintf(" %4d", calibration_val[c][r] / ADC_RING_BUFFER_SIZE);
        uprintf("\nRAW r%d:", r);
        for (int c = 0; c < CAP_COLS; ++c) uprintf(" %4d", adc_ring_buffer[last][c][r]);
        uprintf("\nMIN r%d:", r);
        for (int c = 0; c < CAP_COLS; ++c) uprintf(" %4d", diag_min[c][r]);
        uprintf("\nMAX r%d:", r);
        for (int c = 0; c < CAP_COLS; ++c) uprintf(" %4d", diag_max[c][r]);
        uprintf("\nD3  r%d:", r);
        for (int c = 0; c < CAP_COLS; ++c) {
            int total = 0;
            for (int k = 0; k < ADC_RING_BUFFER_SIZE; ++k) total += adc_ring_buffer[k][c][r];
            uprintf(" %4d", total - calibration_val[c][r]);
        }
        uprintf("\nPRS r%d: %04X\n", r, matrix[r]);
    }

    int lo = 0, hi = 0;
    for (int c = 0; c < CAP_COLS; ++c) {
        for (int r = 0; r < CAP_ROWS; ++r) {
            int b = calibration_val[c][r] / ADC_RING_BUFFER_SIZE;
            if (b < DIAG_BASELINE_LO) ++lo;
            if (b > DIAG_BASELINE_HI) ++hi;
        }
    }
    uprintf("CHK lo=%d hi=%d:", lo, hi);
    for (int c = 0; c < CAP_COLS; ++c) {
        for (int r = 0; r < CAP_ROWS; ++r) {
            int b = calibration_val[c][r] / ADC_RING_BUFFER_SIZE;
            if (b < DIAG_BASELINE_LO || b > DIAG_BASELINE_HI) uprintf(" r%dc%d=%d", r, c, b);
        }
    }
    uprintf("\n");

    ++diag_seq;
    diag_scans     = 0;
    diag_last_dump = timer_read32();
    adc_diag_reset_minmax();
}
#endif
