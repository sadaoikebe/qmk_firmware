#include QMK_KEYBOARD_H

#include "analog.h"

#define ADC_RING_BUFFER_SIZE 3

const int CAP_ROW_PINS[] = { GP2, GP4, GP1, GP0, GP7 };
const int CAP_COL_PINS[] = { GP3, GP6, GP27, GP26 };
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

void acquire_adc(int adc_measured_val[][5]) {
  for(int j=0; j<16; ++j) {
    writePin(CAP_COL_PINS[0], (j&1) ? true : false);
    writePin(CAP_COL_PINS[1], (j&2) ? true : false);
    writePin(CAP_COL_PINS[2], (j&4) ? true : false);
    writePin(CAP_COL_PINS[3], (j&8) ? true : false);
    for(int i=0; i<5; ++i) {
       setPinOutput(CAP_PURGE);
       writePinLow(CAP_PURGE);
       wait_us(20);
       setPinInput(CAP_PURGE);

       writePinHigh(CAP_ROW_PINS[i]);
       wait_us(1);
       adc_measured_val[j][i] = analogReadPin(CAP_SENSE);
       writePinLow(CAP_ROW_PINS[i]);
    }
  }
}

void matrix_init(void) {
   setPinOutput(GP2);
   setPinOutput(GP4);
   setPinOutput(GP1);
   setPinOutput(GP0);
   setPinOutput(GP7);
   setPinOutput(GP3);
   setPinOutput(GP6);
   setPinOutput(GP27);
   setPinOutput(GP26);
   setPinInput(GP28);
   setPinInput(GP29);

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
    for(int j=0; j<16; ++j) {
      for(int k=0; k<5; ++k) {
        calibration_val[j][k] += adc_measured_val[j][k];
      }
    }
  }

  matrix_init_kb();
}

uint8_t matrix_scan(void) {
    matrix_row_t curr_matrix[5] = {0, 0, 0, 0, 0};
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

    matrix_scan_kb();

    return changed;
}

matrix_row_t matrix_get_row(uint8_t row) {
  return matrix[row];
}

void matrix_print(void)
{
}
