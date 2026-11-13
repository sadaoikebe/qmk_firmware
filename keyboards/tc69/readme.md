# TC69 (capacitive / EC)

Seeed XIAO RP2040 based 69-key capacitive keyboard (Sadao Ikebe).
Rebuilt on 2026-11-12 for current QMK from the salvaged `capacitive` branch
(@690c285d). Details: `ts52k-ec/firmware-rebuild.md`.

    make tc69:nicola                       # normal firmware
    make tc69:nicola TC69_ADC_DIAG=yes     # + raw ADC diagnostics over QMK console (M1)
    make tc69:nicola TC69_ADC_DIAG=yes TC69_DIAG_PURGE=after   # EXPERIMENTAL purge order (M1 A/B only)

Diagnostics (TC69_ADC_DIAG=yes) are a read-only observer of the sensing: the
normal build is byte-identical with or without this code. Output format
(fmt=2: 1 s statistics, 50 ms envelope, press/release transition traces, scan
timing) is documented in `adc_diag_impl.h`; the host analyser and the M1
procedure live in the ts52k repository, branch tc52, `docs/m1/`.

Diag-only build options: `TC69_DIAG_INTERVAL_MS` (statistics, default 1000),
`TC69_DIAG_ENV_MS` (envelope, default 50, 0 = off), `TC69_DIAG_TRACE_DECIM`
(record every n-th scan, default 1), `TC69_DIAG_TRACE_ROW=yes` (also record
the target key's whole row), `TC69_DIAG_TRACE_KEY=row,col` (fixed trace key,
default: the first key pressed), `TC69_DIAG_PURGE=after` with
`TC69_DIAG_PURGE_US` (default 6, as in the Arduino sense.ino).

Flashing: hold BOOT on the XIAO RP2040 while plugging in USB (or while pressing RESET),
then copy the .uf2 to the RPI-RP2 drive. (Bootmagic is not enabled, as in the original.)
