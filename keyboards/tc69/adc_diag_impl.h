/*
 * TC69 raw-ADC diagnostics — implementation, #included at the end of matrix.c
 * only when TC69_ADC_DIAG is defined (build with TC69_ADC_DIAG=yes). Not a
 * separate translation unit so that it can read matrix.c's static ring index.
 *
 * READ-ONLY OBSERVER. Nothing in here writes adc_ring_buffer, calibration_val
 * or matrix, or changes thresholds / scan order / timing constants. The only
 * diagnostic build that changes sensing is the purge-order experiment
 * (TC69_DIAG_PURGE_AFTER, see acquire_adc() in matrix.c), and every dump of
 * that build says so ("purge=after ... EXPERIMENTAL").
 *
 * The console output is a fixed text format parsed by
 * ts52k docs/m1/m1_analyze.py. Version: fmt=2. Lines (one dump every
 * TC69_ADC_DIAG_INTERVAL_MS, default 1000 ms):
 *
 *   #ADC seq=<n> t=<ms> scans=<n> scan_hz=<n>          (unchanged from fmt=1)
 *   CAL r<row>: 16 x baseline (calibration sum / 3)    (unchanged)
 *   RAW r<row>: 16 x latest single frame               (unchanged)
 *   MIN r<row>: 16 x min single frame in the interval  (unchanged)
 *   MAX r<row>: 16 x max single frame in the interval  (unchanged)
 *   D3  r<row>: 16 x latest (3-frame sum - cal sum)    (unchanged)
 *   PRS r<row>: <hex> matrix row after judgement       (unchanged)
 *   AVG r<row>: 16 x mean single frame x10 (0.1 count) (fmt=2)
 *   SDV r<row>: 16 x std-dev single frame x10          (fmt=2)
 *   #SCN n=<periods> per_min=<us> per_avg=<us> per_max=<us> acq_min=<us> acq_avg=<us> acq_max=<us>
 *   #HST bin_us=<w> <16 counts>    histogram of scan periods (last bin = overflow)
 *   #CFG fmt=2 kb=tc69 purge=<before|after> purge_us=<n> thr=120/30 thr_a=220/80 a_key=r2c7
 *        interval_ms=<n> env_ms=<n> env_th=<n> trc_pre=<n> trc_post=<n> trc_decim=<n> trc_row=<0|1> trc_key=<auto|rRcC>
 *   #WARN EXPERIMENTAL purge order (read->purge, sense.ino) - not the TC69 firmware behaviour
 *                                   (only in the purge-order build, every dump)
 *
 * While a transition segment is being recorded the statistics dump waits
 * (at most 3 intervals), so that its console time does not cut into the trace.
 *
 * Scan periods (#SCN/#HST) are the time between the starts of consecutive scans;
 * periods that contain diagnostic console output are excluded. acq = duration
 * of acquire_adc() alone (all 80 crosspoints).
 *
 * Envelope lines (every TC69_DIAG_ENV_MS, default 50 ms; 0 = off). Only keys
 * whose single-frame min or max in that window left cal +/- TC69_DIAG_ENV_TH
 * are listed; nothing is printed while everything is quiet:
 *   ENV t=<ms> n=<scans> r<row>c<col>=<min>,<max>,<last> ...
 *
 * Transition trace (auto-triggered): the first key whose judged state goes
 * 0->1 while nothing else is pressed becomes the target (or the fixed key from
 * TC69_DIAG_TRACE_KEY_ROW/COL). Two segments are captured around its press
 * edge (seg=P) and release edge (seg=R): up to TC69_DIAG_TRACE_PRE - 1 frames
 * before the edge and TC69_DIAG_TRACE_POST frames from the edge on, at scan
 * resolution (every TC69_DIAG_TRACE_DECIM-th scan). A release segment that ends
 * with the key judged pressed again (chatter, or a new stroke) is discarded and
 * restarted at the next release edge (counted in r_restarts). Both segments are
 * dumped after the release segment is complete and the key's 3-frame delta is
 * back below 3 x TC69_DIAG_ENV_TH (at most 3 s later):
 *   #TRC begin id=<n> seg=<P|R> key=r<row>c<col> cal=<cal> cal3=<cal sum> thr3=<press thr x3> rel3=<release thr x3> pre=<n> post=<n> decim=<n> other=<n> r_restarts=<n> t0_us=<edge time, us clock>
 *   T <seg> <i> <dt_us> <raw> <d3> <prs>[ <16 x raw of the target's row>]
 *   #TRC end id=<n> seg=<P|R>
 *   i = frame index relative to the edge frame (i=0 is the first frame judged
 *   in the new state), dt_us = time of that scan relative to the edge scan,
 *   prs = judged state of the target, other = frames in the segment where some
 *   other key was judged pressed.
 */
#include "print.h"
#include "timer.h"
#include "ch.h"

#ifndef TC69_ADC_DIAG_INTERVAL_MS
#    define TC69_ADC_DIAG_INTERVAL_MS 1000
#endif
#ifndef TC69_DIAG_ENV_MS
#    define TC69_DIAG_ENV_MS 50
#endif
#ifndef TC69_DIAG_ENV_TH
#    define TC69_DIAG_ENV_TH 20
#endif
#ifndef TC69_DIAG_ENV_MAX_KEYS
#    define TC69_DIAG_ENV_MAX_KEYS 12
#endif
#ifndef TC69_DIAG_TRACE_PRE
#    define TC69_DIAG_TRACE_PRE 300
#endif
#ifndef TC69_DIAG_TRACE_POST
#    define TC69_DIAG_TRACE_POST 300
#endif
#ifndef TC69_DIAG_TRACE_DECIM
#    define TC69_DIAG_TRACE_DECIM 1
#endif
#ifndef TC69_DIAG_HIST_BIN_US
#    define TC69_DIAG_HIST_BIN_US 250
#endif
#define DIAG_HIST_BINS 16
#define DIAG_KEYS (MATRIX_COLS * MATRIX_ROWS)

#if defined(TC69_DIAG_TRACE_KEY_ROW) != defined(TC69_DIAG_TRACE_KEY_COL)
#    error "set both TC69_DIAG_TRACE_KEY_ROW and TC69_DIAG_TRACE_KEY_COL (or neither)"
#endif

#ifdef TC69_DIAG_PURGE_AFTER
#    define DIAG_PURGE_NAME "after"
#    define DIAG_PURGE_US TC69_DIAG_PURGE_US
#else
#    define DIAG_PURGE_NAME "before"
#    define DIAG_PURGE_US 20
#endif

static inline uint32_t diag_now_us(void) {
    return (uint32_t)chVTGetSystemTimeX(); /* CH_CFG_ST_FREQUENCY = 1 MHz on RP2040 */
}

static uint8_t diag_pressed(int c, int r) {
    return (matrix[r] >> c) & 1;
}

static int diag_d3(int c, int r) {
    int total = 0;
    for (int k = 0; k < ADC_RING_BUFFER_SIZE; ++k) total += adc_ring_buffer[k][c][r];
    return total - calibration_val[c][r];
}

static uint32_t diag_isqrt64(uint64_t v) {
    uint64_t res = 0, bit = (uint64_t)1 << 62;
    while (bit > v) bit >>= 2;
    while (bit) {
        if (v >= res + bit) {
            v -= res + bit;
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return (uint32_t)res;
}

/* ---- 1 s statistics (fmt=1 lines kept as they were) ---- */
static int16_t  diag_min[MATRIX_COLS][MATRIX_ROWS];
static int16_t  diag_max[MATRIX_COLS][MATRIX_ROWS];
static int32_t  diag_sum[MATRIX_COLS][MATRIX_ROWS];
static uint64_t diag_ssq[MATRIX_COLS][MATRIX_ROWS];
static uint32_t diag_last_dump;
static uint32_t diag_scans;
static uint32_t diag_seq;

/* ---- scan timing ---- */
static uint32_t diag_scan_t0, diag_prev_t0, diag_acq_us;
static bool     diag_have_prev;
static bool     diag_printed; /* console output happened since the last scan start */
static uint32_t per_n, per_min, per_max, acq_n, acq_min, acq_max;
static uint64_t per_sum, acq_sum;
static uint32_t per_hist[DIAG_HIST_BINS];

/* ---- envelope ---- */
static int16_t  env_min[MATRIX_COLS][MATRIX_ROWS];
static int16_t  env_max[MATRIX_COLS][MATRIX_ROWS];
static uint32_t env_last, env_scans;

/* ---- transition trace ---- */
typedef struct {
    uint32_t t;
    uint16_t prs[MATRIX_ROWS];
    int16_t  raw[MATRIX_COLS][MATRIX_ROWS];
    int16_t  d3[MATRIX_COLS][MATRIX_ROWS];
} diag_frame_t;

typedef struct {
    uint32_t t;
    int16_t  raw, d3;
    uint8_t  prs;
#ifdef TC69_DIAG_TRACE_ROWVALS
    int16_t row[MATRIX_COLS];
#endif
} diag_tpoint_t;

typedef struct {
    diag_tpoint_t pt[TC69_DIAG_TRACE_PRE + TC69_DIAG_TRACE_POST];
    uint16_t      npre;  /* frames actually available before the edge */
    uint16_t      npost; /* frames recorded from the edge on */
    uint16_t      other;
    bool          active, done;
} diag_seg_t;

static diag_frame_t hist[TC69_DIAG_TRACE_PRE];
static uint16_t     hist_head, hist_count;
static uint32_t     trc_decim_ctr;
static diag_seg_t   seg_p, seg_r;
static int8_t       trc_c = -1, trc_r = -1;
static uint32_t     trc_id;
static uint16_t     trc_r_restarts; /* release segments discarded because the key was pressed again */
static uint32_t     trc_done_ms;    /* when both segments were complete; the dump waits for the key to settle */

static void adc_diag_reset_minmax(void) {
    for (int c = 0; c < MATRIX_COLS; ++c) {
        for (int r = 0; r < MATRIX_ROWS; ++r) {
            diag_min[c][r] = INT16_MAX;
            diag_max[c][r] = INT16_MIN;
            diag_sum[c][r] = 0;
            diag_ssq[c][r] = 0;
        }
    }
    per_n = acq_n = 0;
    per_min = acq_min = UINT32_MAX;
    per_max = acq_max = 0;
    per_sum = acq_sum = 0;
    for (int i = 0; i < DIAG_HIST_BINS; ++i) per_hist[i] = 0;
}

static void adc_diag_reset_env(void) {
    for (int c = 0; c < MATRIX_COLS; ++c) {
        for (int r = 0; r < MATRIX_ROWS; ++r) {
            env_min[c][r] = INT16_MAX;
            env_max[c][r] = INT16_MIN;
        }
    }
    env_scans = 0;
}

static void adc_diag_init(void) {
    adc_diag_reset_minmax();
    adc_diag_reset_env();
    diag_last_dump = timer_read32();
    env_last       = diag_last_dump;
    diag_scans     = 0;
    diag_seq       = 0;
    diag_have_prev = false;
}

/* called right before acquire_adc() in matrix_scan() */
static void adc_diag_scan_begin(void) {
    diag_scan_t0 = diag_now_us();
    if (diag_have_prev && !diag_printed) {
        uint32_t p = diag_scan_t0 - diag_prev_t0;
        ++per_n;
        per_sum += p;
        if (p < per_min) per_min = p;
        if (p > per_max) per_max = p;
        uint32_t b = p / TC69_DIAG_HIST_BIN_US;
        per_hist[b < DIAG_HIST_BINS ? b : DIAG_HIST_BINS - 1]++;
    }
    diag_prev_t0   = diag_scan_t0;
    diag_have_prev = true;
    diag_printed   = false;
}

/* called right after acquire_adc() in matrix_scan() */
static void adc_diag_scan_acquired(void) {
    diag_acq_us = diag_now_us() - diag_scan_t0;
    ++acq_n;
    acq_sum += diag_acq_us;
    if (diag_acq_us < acq_min) acq_min = diag_acq_us;
    if (diag_acq_us > acq_max) acq_max = diag_acq_us;
}

static void diag_print_cfg(void) {
    uprintf("#CFG fmt=2 kb=tc69 purge=%s purge_us=%d thr=%d/%d thr_a=%d/%d a_key=r2c7 interval_ms=%d env_ms=%d env_th=%d trc_pre=%d trc_post=%d trc_decim=%d",
            DIAG_PURGE_NAME, DIAG_PURGE_US, adc_press_threshold / ADC_RING_BUFFER_SIZE, adc_release_threshold / ADC_RING_BUFFER_SIZE, adc_press_threshold_2 / ADC_RING_BUFFER_SIZE, adc_release_threshold_2 / ADC_RING_BUFFER_SIZE,
            TC69_ADC_DIAG_INTERVAL_MS, TC69_DIAG_ENV_MS, TC69_DIAG_ENV_TH, TC69_DIAG_TRACE_PRE, TC69_DIAG_TRACE_POST, TC69_DIAG_TRACE_DECIM);
#ifdef TC69_DIAG_TRACE_ROWVALS
    uprintf(" trc_row=1");
#else
    uprintf(" trc_row=0");
#endif
#ifdef TC69_DIAG_TRACE_KEY_ROW
    uprintf(" trc_key=r%dc%d\n", TC69_DIAG_TRACE_KEY_ROW, TC69_DIAG_TRACE_KEY_COL);
#else
    uprintf(" trc_key=auto\n");
#endif
#ifdef TC69_DIAG_PURGE_AFTER
    uprintf("#WARN EXPERIMENTAL purge order (read->purge, sense.ino) - not the TC69 firmware behaviour\n");
#endif
}

static void diag_dump_stats(uint32_t elapsed) {
    int last = (adc_ring_index + ADC_RING_BUFFER_SIZE - 1) % ADC_RING_BUFFER_SIZE;

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
        for (int c = 0; c < MATRIX_COLS; ++c) uprintf(" %4d", diag_d3(c, r));
        uprintf("\nPRS r%d: %04X\n", r, matrix[r]);
    }
    /* fmt=2 additions, after the fmt=1 block so old parsers are unaffected */
    uint32_t n = diag_scans ? diag_scans : 1;
    for (int r = 0; r < MATRIX_ROWS; ++r) {
        uprintf("AVG r%d:", r);
        for (int c = 0; c < MATRIX_COLS; ++c) uprintf(" %5ld", (long)((diag_sum[c][r] * 10L + (long)n / 2) / (long)n));
        uprintf("\nSDV r%d:", r);
        for (int c = 0; c < MATRIX_COLS; ++c) {
            /* sd*10 = sqrt(100 * (n*ssq - sum^2)) / n */
            int64_t  s   = diag_sum[c][r];
            uint64_t num = (uint64_t)n * diag_ssq[c][r] - (uint64_t)(s * s);
            uprintf(" %5lu", (unsigned long)((diag_isqrt64(num * 100U) + n / 2) / n));
        }
        uprintf("\n");
    }
    uprintf("#SCN n=%lu per_min=%lu per_avg=%lu per_max=%lu acq_min=%lu acq_avg=%lu acq_max=%lu\n", (unsigned long)per_n, (unsigned long)(per_n ? per_min : 0), (unsigned long)(per_n ? per_sum / per_n : 0), (unsigned long)per_max, (unsigned long)(acq_n ? acq_min : 0), (unsigned long)(acq_n ? acq_sum / acq_n : 0),
            (unsigned long)acq_max);
    uprintf("#HST bin_us=%d", TC69_DIAG_HIST_BIN_US);
    for (int i = 0; i < DIAG_HIST_BINS; ++i) uprintf(" %lu", (unsigned long)per_hist[i]);
    uprintf("\n");
    diag_print_cfg();
}

#if TC69_DIAG_ENV_MS > 0
static void diag_dump_env(uint32_t now_ms) {
    int  listed = 0, more = 0;
    bool head   = false;
    for (int r = 0; r < MATRIX_ROWS; ++r) {
        for (int c = 0; c < MATRIX_COLS; ++c) {
            int cal = calibration_val[c][r] / ADC_RING_BUFFER_SIZE;
            if (env_max[c][r] - cal < TC69_DIAG_ENV_TH && cal - env_min[c][r] < TC69_DIAG_ENV_TH) continue;
            if (listed >= TC69_DIAG_ENV_MAX_KEYS) {
                ++more;
                continue;
            }
            if (!head) {
                uprintf("ENV t=%lu n=%lu", (unsigned long)now_ms, (unsigned long)env_scans);
                head = true;
            }
            int last = (adc_ring_index + ADC_RING_BUFFER_SIZE - 1) % ADC_RING_BUFFER_SIZE;
            uprintf(" r%dc%d=%d,%d,%d", r, c, env_min[c][r], env_max[c][r], adc_ring_buffer[last][c][r]);
            ++listed;
        }
    }
    if (head) {
        if (more) uprintf(" more=%d", more);
        uprintf("\n");
        diag_printed = true;
    }
}
#endif

/* ---- transition trace ---- */
static void seg_put(diag_seg_t *s, int idx, const diag_frame_t *f) {
    diag_tpoint_t *p = &s->pt[idx];
    p->t             = f->t;
    p->raw           = f->raw[trc_c][trc_r];
    p->d3            = f->d3[trc_c][trc_r];
    p->prs           = (f->prs[trc_r] >> trc_c) & 1;
#ifdef TC69_DIAG_TRACE_ROWVALS
    for (int c = 0; c < MATRIX_COLS; ++c) p->row[c] = f->raw[c][trc_r];
#endif
    for (int r = 0; r < MATRIX_ROWS; ++r) {
        uint16_t others = f->prs[r];
        if (r == trc_r) others &= (uint16_t) ~(1u << trc_c);
        if (others) {
            s->other++;
            break;
        }
    }
}

/* start a segment at the edge frame f (already pushed into hist) */
static void seg_start(diag_seg_t *s, const diag_frame_t *f) {
    s->other = 0;
    /* hist holds hist_count frames; the newest one is f itself */
    int avail = hist_count > 0 ? hist_count - 1 : 0;
    if (avail > TC69_DIAG_TRACE_PRE) avail = TC69_DIAG_TRACE_PRE;
    s->npre = (uint16_t)avail;
    for (int i = 0; i < avail; ++i) {
        int h = (hist_head + TC69_DIAG_TRACE_PRE - 1 - avail + i) % TC69_DIAG_TRACE_PRE;
        seg_put(s, TC69_DIAG_TRACE_PRE - avail + i, &hist[h]);
    }
    s->npost  = 0;
    s->active = true;
    s->done   = false;
}

static void seg_add(diag_seg_t *s, const diag_frame_t *f) {
    if (!s->active) return;
    seg_put(s, TC69_DIAG_TRACE_PRE + s->npost, f);
    if (++s->npost >= TC69_DIAG_TRACE_POST) {
        s->active = false;
        s->done   = true;
    }
}

static void seg_dump(diag_seg_t *s, char name) {
    int      c = trc_c, r = trc_r;
    bool     a = is_home_position(c, r);
    uint32_t t0 = s->pt[TC69_DIAG_TRACE_PRE].t;
    uprintf("#TRC begin id=%lu seg=%c key=r%dc%d cal=%d cal3=%d thr3=%d rel3=%d pre=%d post=%d decim=%d other=%d r_restarts=%u t0_us=%lu\n", (unsigned long)trc_id, name, r, c, calibration_val[c][r] / ADC_RING_BUFFER_SIZE, calibration_val[c][r], a ? adc_press_threshold_2 : adc_press_threshold, a ? adc_release_threshold_2 : adc_release_threshold,
            s->npre, s->npost, TC69_DIAG_TRACE_DECIM, s->other, (unsigned)trc_r_restarts, (unsigned long)t0);
    for (int i = TC69_DIAG_TRACE_PRE - s->npre; i < TC69_DIAG_TRACE_PRE + s->npost; ++i) {
        diag_tpoint_t *p = &s->pt[i];
        uprintf("T %c %d %ld %d %d %d", name, i - TC69_DIAG_TRACE_PRE, (long)(int32_t)(p->t - t0), p->raw, p->d3, p->prs);
#ifdef TC69_DIAG_TRACE_ROWVALS
        for (int k = 0; k < MATRIX_COLS; ++k) uprintf(" %d", p->row[k]);
#endif
        uprintf("\n");
    }
    uprintf("#TRC end id=%lu seg=%c\n", (unsigned long)trc_id, name);
}

static bool diag_any_pressed_except(int c0, int r0) {
    for (int r = 0; r < MATRIX_ROWS; ++r) {
        uint16_t m = matrix[r];
        if (r == r0) m &= (uint16_t) ~(1u << c0);
        if (m) return true;
    }
    return false;
}

static void diag_trace_after_scan(int last) {
    /* judged state at the previously recorded frame (edges are found between
     * recorded frames, so decimation cannot skip over an edge) */
    static uint16_t prev_prs[MATRIX_ROWS];

    if (++trc_decim_ctr < TC69_DIAG_TRACE_DECIM) return;
    trc_decim_ctr = 0;

    diag_frame_t *f = &hist[hist_head];
    f->t            = diag_scan_t0;
    for (int r = 0; r < MATRIX_ROWS; ++r) {
        f->prs[r] = matrix[r];
        for (int c = 0; c < MATRIX_COLS; ++c) {
            f->raw[c][r] = adc_ring_buffer[last][c][r];
            f->d3[c][r]  = diag_d3(c, r);
        }
    }
    hist_head = (hist_head + 1) % TC69_DIAG_TRACE_PRE;
    if (hist_count < TC69_DIAG_TRACE_PRE) hist_count++;

    if (trc_c < 0) {
        /* armed: look for a press edge */
        for (int r = 0; r < MATRIX_ROWS && trc_c < 0; ++r) {
            uint16_t rise = matrix[r] & (uint16_t)~prev_prs[r];
            for (int c = 0; c < MATRIX_COLS && rise; ++c) {
                if (!((rise >> c) & 1)) continue;
#ifdef TC69_DIAG_TRACE_KEY_ROW
                if (r != TC69_DIAG_TRACE_KEY_ROW || c != TC69_DIAG_TRACE_KEY_COL) continue;
#endif
                if (diag_any_pressed_except(c, r)) continue;
                trc_c = c;
                trc_r = r;
                break;
            }
        }
        if (trc_c >= 0) {
            seg_r.active = seg_r.done = false;
            seg_start(&seg_p, f);
            seg_add(&seg_p, f);
        }
    } else {
        seg_add(&seg_p, f);
        bool now = diag_pressed(trc_c, trc_r);
        bool was = (prev_prs[trc_r] >> trc_c) & 1;
        if (!seg_r.active && !seg_r.done && was && !now) {
            seg_start(&seg_r, f);
            seg_add(&seg_r, f);
        } else {
            seg_add(&seg_r, f);
            if (seg_r.done && now) {
                /* pressed again (chatter or a new stroke) before the release segment ended:
                 * not the final release; record the next release edge instead */
                seg_r.done = false;
                ++trc_r_restarts;
            }
        }
        if (seg_p.done && seg_r.done && trc_done_ms == 0) trc_done_ms = timer_read32() | 1;
        /* dump once the key is back near its baseline (so a slow release is not cut by
         * the ~1 s of console output), at the latest 3 s after both segments were complete */
        if (seg_p.done && seg_r.done && (diag_d3(trc_c, trc_r) < 3 * TC69_DIAG_ENV_TH || TIMER_DIFF_32(timer_read32(), trc_done_ms) > 3000)) {
            trc_done_ms = 0;
            seg_dump(&seg_p, 'P');
            seg_dump(&seg_r, 'R');
            diag_printed = true;
            ++trc_id;
            trc_r_restarts = 0;
            trc_c = trc_r = -1;
            seg_p.done = seg_r.done = false;
        }
    }
    for (int r = 0; r < MATRIX_ROWS; ++r) prev_prs[r] = matrix[r];
}

/* called at the end of matrix_scan(), after judgement */
static void adc_diag_after_scan(void) {
    /* the frame just written is the one before the (already advanced) ring index */
    int last = (adc_ring_index + ADC_RING_BUFFER_SIZE - 1) % ADC_RING_BUFFER_SIZE;

    ++diag_scans;
    ++env_scans;
    for (int c = 0; c < MATRIX_COLS; ++c) {
        for (int r = 0; r < MATRIX_ROWS; ++r) {
            int v = adc_ring_buffer[last][c][r];
            if (v < diag_min[c][r]) diag_min[c][r] = v;
            if (v > diag_max[c][r]) diag_max[c][r] = v;
            if (v < env_min[c][r]) env_min[c][r] = v;
            if (v > env_max[c][r]) env_max[c][r] = v;
            diag_sum[c][r] += v;
            diag_ssq[c][r] += (uint32_t)(v * v);
        }
    }

    diag_trace_after_scan(last);

    uint32_t now = timer_read32();
#if TC69_DIAG_ENV_MS > 0
    if (TIMER_DIFF_32(now, env_last) >= TC69_DIAG_ENV_MS) {
        diag_dump_env(now);
        env_last = now;
        adc_diag_reset_env();
    }
#endif

    uint32_t elapsed = TIMER_DIFF_32(now, diag_last_dump);
    if (elapsed < TC69_ADC_DIAG_INTERVAL_MS) return;
    /* keep the ~0.1 s statistics dump out of a transition being recorded
     * (deferred at most to 3 intervals; the interval is then just longer) */
    if ((seg_p.active || seg_r.active) && elapsed < 3 * TC69_ADC_DIAG_INTERVAL_MS) return;

    diag_dump_stats(elapsed);
    diag_printed = true;

    ++diag_seq;
    diag_scans     = 0;
    diag_last_dump = timer_read32();
    adc_diag_reset_minmax();
}
