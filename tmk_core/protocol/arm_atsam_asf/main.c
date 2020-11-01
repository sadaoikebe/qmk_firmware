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

#include <delay/delay.h>
#include <system/system.h>

#include "config.h"
#include "keyboard.h"
#include "print.h"
#include "host.h"
#include "arm_atsam_asf/usb.h"
#include "arm_atsam_asf/uart.h"

static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *);
static void send_mouse(report_mouse_t *);
static void send_system(uint16_t);
static void send_consumer(uint16_t);

host_driver_t host_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

int main(void) {

    system_init();
    delay_init();
    configure_uart();

    println("Hello world!");

    configure_usb();
    keyboard_setup();
    keyboard_init();
    host_set_driver(&host_driver);

    while (1) {
        keyboard_task();
    }

    return 0;
}

static uint8_t keyboard_leds(void) {
    return udb_get_leds();
}

static void send_keyboard(report_keyboard_t *report_keyboard) {
    usb_send_keyboard(report_keyboard);
}

static void send_mouse(report_mouse_t *report_mouse) {
#ifdef MOUSEKEY_ENABLE
#error mousekey handling is not yet implemented
#endif
}

static void send_system(uint16_t report) {
#ifdef EXTRAKEY_ENABLE
#error extrakey handling is not yet implemented
#endif
}

static void send_consumer(uint16_t report) {
#ifdef EXTRAKEY_ENABLE
#error extrakey handling is not yet implemented
#endif
}
