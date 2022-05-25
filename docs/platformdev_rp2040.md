# Raspberry PI RP2040

The following table shows the current driver status for peripherals on RP2040 MCUs:

| System           | Support                                        |
| ---------------- | ---------------------------------------------- |
| ADC driver       | Support planned (no ETA)                       |
| Audio            | Support planned (no ETA)                       |
| I2C driver       | :heavy_check_mark:                             |
| SPI driver       | :heavy_check_mark:                             |
| WS2812 driver    | :heavy_check_mark: using `PIO` driver          |
| External EEPROMs | :heavy_check_mark: using `I2C` or `SPI` driver |
| EEPROM emulation | Support planned (no ETA)                       |
| serial driver    | :heavy_check_mark: using `SIO` or `PIO` driver |
| UART driver      | Support planned (no ETA)                       |

## GPIO pin nomenclature

<img alt="Raspberry Pi Pico Pinout" src="assets/Pi_Pico_Pinout.png" width="48%"/>
<img alt="Adafruit KB2040 Pinout" src="assets/KB2040_Pinout.png" width="48%"/>

!> The GPIO pins of the RP2040 are not 5V tolerant!

To address individual pins on the RP2040, QMK uses the `GPx` abbreviation -- where the `x` stands for the GPIO number of the pin. This number can likely be found on the official pinout diagram of your board. Note that these GPIO numbers match the RP2040 MCU datasheet, and don't necessarily match the number you see printed on the board. For instance the Raspberry Pi Pico uses numbers from 1 to 40 for their pins, but these are not identical to the RP2040's GPIO numbers. So if you want to use the pin 11 of the Pico for your keyboard, you would refer to it as `GP8` in the config files.

## Double-tap reset boot-loader entry

The double-tap reset mechanism is an alternate way in QMK to enter the embedded mass storage UF2 boot-loader of the RP2040. It works by a fast double-tap of the reset pin on start up, which is similar to the behavior of AVR Pro Micros. This feature is not activated by default and has to be configured. To activate it add the following options to your keyboards `config.h` file:

```c
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET // Activates the double-tap behavior
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_TIMEOUT 200U // Timeout window in ms in which the double tap can occur.
#define RP2040_BOOTLOADER_DOUBLE_TAP_RESET_LED_MASK 0U // Specify a optional status led which blinks when entering the bootloader
```

## Split keyboard support

Split keyboards are fully supported using the [serial driver](serial_driver.md) in both Full-duplex and Half-duplex configurations. For this to work two driver subsystems are supported by the RP2040, the hardware UART based `SIO` and the Programmable IO based `PIO` driver.

| Feature                       | [SIO Driver](serial_driver.md#the-sio-driver) | [PIO Driver](serial_driver.md#the-pio-driver) |
| ----------------------------- | --------------------------------------------- | --------------------------------------------- |
| Half-Duplex operation         |                                               | :heavy_check_mark:                            |
| Full-Duplex operation         | :heavy_check_mark:                            | :heavy_check_mark:                            |
| `TX` and `RX` pin swapping    |                                               | :heavy_check_mark:                            |
| Any GPIO as `TX` and `RX` pin | Only UART capable pins                        | :heavy_check_mark:                            |
| Simple configuration          |                                               | :heavy_check_mark:                            |

The `PIO` driver is much more flexible then the `SIO` driver, the only "downside" is the usage of `PIO` resources which in turn are not available for advanced user programs. Under normal circumstances, this resource allocation will be a non-issue.
