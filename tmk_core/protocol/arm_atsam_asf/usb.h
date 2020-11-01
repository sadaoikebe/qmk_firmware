#pragma once

extern void configure_usb(void);
extern uint8_t udb_get_leds(void);
extern bool usb_send_keyboard(report_keyboard_t *report_keyboard);
