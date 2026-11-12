/*
 * TC69 capacitive (EC) matrix — rebuilt for current QMK (2026-11-12).
 *
 * Origin: sadaoikebe/qmk_firmware branch "capacitive" @ 690c285d83
 *         keyboards/tc69/matrix.c (salvaged copy: ts52k-ec/ref/qmk-capacitive/final/matrix.c)
 *
 * The sensing and judgement logic is kept exactly as in the original.
 * Changes are mechanical only (see ts52k-ec/firmware-rebuild.md):
 *   - GPIO API renamed to gpio_*()  (setPinOutput -> gpio_set_pin_output, ...)
 *   - hard-coded 16 / 5 replaced by MATRIX_COLS / MATRIX_ROWS (static-asserted to 16 / 5)
 *   - pin arrays typed pin_t instead of int
 *   - optional raw-ADC diagnostics, compiled in only with TC69_ADC_DIAG (default OFF)
 *
 * NOTE (kept on purpose, see firmware-audit.md §3.3): curr_matrix is zeroed on every
 * scan, so the release threshold branch never has an effect. The effective behaviour is
 * a single threshold (press threshold) on a 3-frame moving sum. This is how TC69 ran.
 */
#include QMK_KEYBOARD_H

#include "analog.h"

#define ADC_RING_BUFFER_SIZE 3

_Static_assert(MATRIX_COLS == 16, "TC69 scans 16 columns via 2x 4051 (4-bit address)");
_Static_assert(MATRIX_ROWS == 5, "TC69 has 5 strobe rows");

const pin_t CAP_ROW_PINS[] = { GP2, GP4, GP1, GP0, GP7 };
const pin_t CAP_COL_PINS[] = { GP3, GP6, GP27, GP26 };
#define CAP_PURGE GP28
#define CAP_SENSE GP29

const int adc_press_threshold = 120 * ADC_RING_BUFFER_SIZE;
const int adc_release_threshold = 30 * ADC_RING_BUFFER_SIZE;

const int adc_press_threshold_2 = 220 * ADC_RING_BUFFER_SIZE;
const int adc_release_threshold_2 = 80 * ADC_RING_BUFFER_SIZE;

static int adc_ring_index = 0;

int adc_ring_buffer[ADC_RING_BUFFER_SIZE][MATRIX_COLS][MATRIX_ROWS];
int calibration_val[MATRIX_COLS][MATRIX_ROWS];
matrix_row_t matrix[MATRIX_ROWS];

__attribute__((weak)) void matrix_init_kb(void) { matrix_init_user(); }

__attribute__((weak)) void matrix_scan_kb(void) { matrix_scan_user(); }

__attribute__((weak)) void matrix_init_user(void) {}

__attribute__((weak)) void matrix_scan_user(void) {}

bool is_home_position(int i, int j) {
  return (i == 7) && (j == 2); // I often lean my left pinky on the A key
}

void acquire_adc(int adc_measured_val[][MATRIX_ROWS]) {
  for(int j=0; j<MATRIX_COLS; ++j) {
    gpio_write_pin(CAP_COL_PINS[0], (j&1) ? true : false);
    gpio_write_pin(CAP_COL_PINS[1], (j&2) ? true : false);
    gpio_write_pin(CAP_COL_PINS[2], (j&4) ? true : false);
    gpio_write_pin(CAP_COL_PINS[3], (j&8) ? true : false);
    for(int i=0; i<MATRIX_ROWS; ++i) {
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

#ifdef TC69_ADC_DIAG
static void adc_diag_init(void);
static void adc_diag_after_scan(void);
#endif

void matrix_init(void) {
   gpio_set_pin_output(GP2);
   gpio_set_pin_output(GP4);
   gpio_set_pin_output(GP1);
   gpio_set_pin_output(GP0);
   gpio_set_pin_output(GP7);
   gpio_set_pin_output(GP3);
   gpio_set_pin_output(GP6);
   gpio_set_pin_output(GP27);
   gpio_set_pin_output(GP26);
   gpio_set_pin_input(GP28);
   gpio_set_pin_input(GP29);

  int adc_measured_val[MATRIX_COLS][MATRIX_ROWS];

  for(int i=0; i<MATRIX_ROWS; ++i) {
    matrix[i] = 0;
    for(int j=0; j<MATRIX_COLS; ++j) {
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
    for(int j=0; j<MATRIX_COLS; ++j) {
      for(int k=0; k<MATRIX_ROWS; ++k) {
        calibration_val[j][k] += adc_measured_val[j][k];
      }
    }
  }

#ifdef TC69_ADC_DIAG
  adc_diag_init();
#endif

  matrix_init_kb();
}

uint8_t matrix_scan(void) {
    matrix_row_t curr_matrix[MATRIX_ROWS] = {0};
    acquire_adc(adc_ring_buffer[adc_ring_index]);
    adc_ring_index = (adc_ring_index + 1) % ADC_RING_BUFFER_SIZE;

    for(int i=0; i<MATRIX_COLS; ++i) {
      for(int j=0; j<MATRIX_ROWS; ++j) {
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

    bool changed = memcmp(matrix, curr_matrix, sizeof(curr_matrix)) != 0;
    if (changed) memcpy(matrix, curr_matrix, sizeof(curr_matrix));

#ifdef TC69_ADC_DIAG
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
 * Raw-ADC diagnostics (only with TC69_ADC_DIAG; build with TC69_ADC_DIAG=yes)
 *
 * Read-only observer: it only reads adc_ring_buffer / calibration_val after the
 * judgement above has been done, and never changes thresholds or matrix state.
 * Every TC69_ADC_DIAG_INTERVAL_MS it prints over the QMK console (qmk console /
 * hid_listen), one block per dump:
 *
 *   #ADC seq=<n> t=<ms> scans=<scans in interval> scan_hz=<scans per second>
 *   CAL r<row>: <16 values>  calibration sum / 3  (= baseline, 10-bit counts)
 *   RAW r<row>: <16 values>  last single frame
 *   MIN r<row>: <16 values>  min single frame since the previous dump
 *   MAX r<row>: <16 values>  max single frame since the previous dump
 *   D3  r<row>: <16 values>  latest (3-frame sum - calibration sum); a key is
 *                            "pressed" when D3 > 360 (A key [r2,c7]: > 660)
 *   PRS r<row>: <hex>        matrix row bitmap after judgement (bit = column)
 *
 * Values are indexed by column 0..15 (4051 address), rows 0..4 (strobe).
 * ------------------------------------------------------------------------ */
#ifdef TC69_ADC_DIAG
#    include "print.h"
#    include "timer.h"

#    ifndef TC69_ADC_DIAG_INTERVAL_MS
#        define TC69_ADC_DIAG_INTERVAL_MS 1000
#    endif

static int16_t  diag_min[MATRIX_COLS][MATRIX_ROWS];
static int16_t  diag_max[MATRIX_COLS][MATRIX_ROWS];
static uint32_t diag_last_dump;
static uint32_t diag_scans;
static uint32_t diag_seq;

static void adc_diag_reset_minmax(void) {
    for (int c = 0; c < MATRIX_COLS; ++c) {
        for (int r = 0; r < MATRIX_ROWS; ++r) {
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
    for (int c = 0; c < MATRIX_COLS; ++c) {
        for (int r = 0; r < MATRIX_ROWS; ++r) {
            int v = adc_ring_buffer[last][c][r];
            if (v < diag_min[c][r]) diag_min[c][r] = v;
            if (v > diag_max[c][r]) diag_max[c][r] = v;
        }
    }

    uint32_t elapsed = timer_elapsed32(diag_last_dump);
    if (elapsed < TC69_ADC_DIAG_INTERVAL_MS) return;

    uprintf("#ADC seq=%lu t=%lu scans=%lu scan_hz=%lu\n", (unsigned long)diag_seq, (unsigned long)timer_read32(), (unsigned long)diag_scans, (unsigned long)(diag_scans * 1000UL / (elapsed ? elapsed : 1)));
    for (int r = 0; r < MATRIX_ROWS; ++r) {
        uprintf("CAL r%d:", r);
        for (int c = 0; c < MATRIX_COLS; ++c) uprintf(" %4d", calibration_val[c][r] / ADC_RING_BUFFER_SIZE);
        uprintf("\nRAW r%d:", r);
        for (int c = 0; c < MATRIX_COLS; ++c) uprintf(" %4d", adc_ring_buffer[last][c][r]);
        uprintf("\nMIN r%d:", r);
        for (int c = 0; c < MATRIX_COLS; ++c) uprintf(" %4d", diag_min[c][r]);
        uprintf("\nMAX r%d:", r);
        for (int c = 0; c < MATRIX_COLS; ++c) uprintf(" %4d", diag_max[c][r]);
        uprintf("\nD3  r%d:", r);
        for (int c = 0; c < MATRIX_COLS; ++c) {
            int total = 0;
            for (int k = 0; k < ADC_RING_BUFFER_SIZE; ++k) total += adc_ring_buffer[k][c][r];
            uprintf(" %4d", total - calibration_val[c][r]);
        }
        uprintf("\nPRS r%d: %04X\n", r, matrix[r]);
    }

    ++diag_seq;
    diag_scans     = 0;
    diag_last_dump = timer_read32();
    adc_diag_reset_minmax();
}
#endif
