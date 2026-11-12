# TC69 (capacitive / EC)

Seeed XIAO RP2040 based 69-key capacitive keyboard (Sadao Ikebe).
Rebuilt on 2026-11-12 for current QMK from the salvaged `capacitive` branch
(@690c285d). Details: `ts52k-ec/firmware-rebuild.md`.

    make tc69:nicola                       # normal firmware
    make tc69:nicola TC69_ADC_DIAG=yes     # + raw ADC dump over QMK console

Flashing: hold BOOT on the XIAO RP2040 while plugging in USB (or while pressing RESET),
then copy the .uf2 to the RPI-RP2 drive. (Bootmagic is not enabled, as in the original.)
