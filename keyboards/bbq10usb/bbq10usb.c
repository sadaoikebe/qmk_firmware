/*
 * Copyright 2020 Hanyazou
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

#include "bbq10usb.h"

#define NUM_LEDS 2
#define NUMLOCK_LED 0
#define CAPSLOCK_LED 1
#define LED_INTERVAL 2000 /* milliseconds */

int led_pins[NUM_LEDS] = {
    PIN_PA18,
    PIN_PA28,
};

enum {
    LED_OFF = 0,
    LED_ON = 1,
    LED_BLINK = 2,
    LED_FLASH = 4,
};

int led_status[NUM_LEDS];

void matrix_init_kb(void) {
    matrix_init_user();
    led_init_ports();
};

void led_init_ports(void) {
    int i;
    for (i = 0; i < NUM_LEDS; i++) {
        led_status[i] = LED_OFF;
        setPinOutput(led_pins[i]);
        writePin(led_pins[i], false);
    };
}

void matrix_scan_kb(void) {
    int i;
    int now = timer_read();

    if (LED_INTERVAL < now) {
        timer_clear();
        now = 0;
    }
    for (i = 0; i < NUM_LEDS; i++) {
        bool value = false; // LED off
        if (led_status[i] & LED_FLASH) {
            value = (LED_INTERVAL/8 < (now % (LED_INTERVAL/4)));
        } else
        if (led_status[i] & LED_BLINK) {
            value = (LED_INTERVAL/2 < now);
        } else
        if (led_status[i] & LED_ON) {
            value = true;
        }
        writePin(led_pins[i], value);
    }

    matrix_scan_user();
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    int layer = get_highest_layer(state);
    int i;

    printf("%s: state=%02lx, highest_layer=%d\r\n", __func__, state, layer);
    for (i = 0; i < NUM_LEDS; i++) {
        if (layer & (1 << i))
            led_status[i] |=  LED_ON;
        else
            led_status[i] &=  ~LED_ON;
    }

    return layer_state_set_user(state);
}

void led_set_kb(uint8_t usb_led) {
    printf("%s: usb_led=%02x\r\n", __func__, usb_led);

    if (IS_LED_ON(usb_led, USB_LED_NUM_LOCK))
        led_status[NUMLOCK_LED] |=  LED_BLINK;
    else
        led_status[NUMLOCK_LED] &=  ~LED_BLINK;

    if (IS_LED_ON(usb_led, USB_LED_CAPS_LOCK))
        led_status[CAPSLOCK_LED] |=  LED_FLASH;
    else
        led_status[CAPSLOCK_LED] &=  ~LED_FLASH;
}
