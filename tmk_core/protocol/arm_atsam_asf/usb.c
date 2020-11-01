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

#include <string.h>
#include <udc.h>
#include <udi_hid.h>
#include "host.h"
#include "arm_atsam_asf/usb.h"
#include "arm_atsam_asf/usb/udi_hid_kbd_api.h"

uint8_t keyboard_protocol = 1;
uint8_t keyboard_idle = 0;

static uint8_t keyboard_leds = 0;

void configure_usb(void) {
    udc_start();
}

bool usb_send_keyboard(report_keyboard_t *report_keyboard) {
    irqflags_t flags;

    flags = cpu_irq_save();

    memcpy(udi_hid_kbd_report, report_keyboard->raw, UDI_HID_KBD_REPORT_SIZE);
    udi_hid_kbd_b_report_valid = true;
    udi_hid_kbd_send_report();

    cpu_irq_restore(flags);

    return true;
}

bool arm_atsam_asf_udi_hid_callback_keyboard_enable(void) {
    printf("USB keyboard enabled\r\n");

    return true;
}

void arm_atsam_asf_udi_hid_callback_keyboard_disable(void) {
    printf("USB keyboard disabled\r\n");
}

void arm_atsam_asf_udi_hid_callback_keyboard_led(uint8_t value) {
    keyboard_leds = value;
}

uint8_t udb_get_leds() {
    return keyboard_leds;
}
