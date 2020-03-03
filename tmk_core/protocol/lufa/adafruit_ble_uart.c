#include "adafruit_ble.h"
#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include <util/delay.h>
#include <util/atomic.h>
#include "debug.h"
#include "pincontrol.h"
#include "timer.h"
#include "action_util.h"
// #include "ringbuffer.hpp"
#include <string.h>
#include "analog.h"
#include "../serial.h"

#ifndef BLE_POWER_PIN
#define BLE_POWER_PIN D5
#endif

#define TIMEOUT 100

uint32_t kb_idle_timer = 0;
uint8_t BLE51_PowerState = 1;
uint8_t ble_init_done = 0;
uint8_t ble_init_stage = 0;

char ble51_buf[30];

int16_t ble51_getc(void)
{
    return serial_recv2();
}

const char *ble51_gets(uint16_t timeout)
{
    static char s[14];
    uint16_t t = timer_read();
    uint8_t i = 0;
    int16_t c;
    while (i < 13 && timer_elapsed(t) < timeout) {
        if ((c = ble51_getc()) != -1) {
            if ((char)c == '\r') continue;
            if ((char)c == '\n') break;
            s[i++] = c;
        }
    }
    s[i] = '\0';
    return s;
}

void ble51_puts(char *s)
{
    while (*s) {
        serial_send(*s++);
    }
}


/* Instruct the module to enable HID keyboard support and reset */
bool adafruit_ble_enable_keyboard(void) {
    printf("BLE stage %u\n", ble_init_stage);
    if (ble_init_stage < 100) {
        ble_init_stage++;
    }
    switch (ble_init_stage) {
    case 1:
        setPinOutput(BLE_POWER_PIN);
        writePin(BLE_POWER_PIN, 0);
        break;
    case 20:
        print("ble init\n");
        ble51_puts("ATZ\n");
        ble51_gets(TIMEOUT);
        break;
    case 30:
        print("ble clear bonds\n");
        ble51_puts("AT+GAPDELBONDS\n");
        ble51_gets(TIMEOUT);
    case 40:
        print("ble discoverable\n");
        ble51_puts("AT+GAPSETADVDATA=02-01-06\n");
        ble51_gets(TIMEOUT);
        ble_init_done = 1;
        break;
    }
    return false;
}

/* Query to see if the BLE module is connected */
bool adafruit_ble_query_is_connected(void) {
    return true;
}

/* Returns true if we believe that the BLE module is connected.
 * This uses our cached understanding that is maintained by
 * calling ble_task() periodically. */
bool adafruit_ble_is_connected(void) {
    return true;
}

/* Call this periodically to process BLE-originated things */
void adafruit_ble_task(void) {
    // Nothing to do here.
    if (!ble_init_done) {
        adafruit_ble_enable_keyboard();
    }
    return;
}

/* Generates keypress events for a set of keys.
 * The hid modifier mask specifies the state of the modifier keys for
 * this set of keys.
 * Also sends a key release indicator, so that the keys do not remain
 * held down. */
bool adafruit_ble_send_keys(report_keyboard_t *report) {
    printf("ble key: %d\n", (int)(ble_init_stage));
    if (!ble_init_done) return false;
    printf("ble key %d %d\n", (int)(ble_init_stage), (int)(report->keys[0]));
    print("[k]");
    ble51_puts("AT+BLEKEYBOARDCODE=");
    sprintf(ble51_buf, "%02X-00-%02X-%02X-%02X-%02X-%02X-%02X\n",
        report->mods,
        report->keys[0],
        report->keys[1],
        report->keys[2],
        report->keys[3],
        report->keys[4],
        report->keys[5]);
    ble51_puts(ble51_buf);
    const char *result = ble51_gets(TIMEOUT);
    if (memcmp(result, "ERROR", 5) == 0) {
        dprint("error\n");
    }
    return true;
}

/* Send a consumer keycode, holding it down for the specified duration
 * (milliseconds) */
bool adafruit_ble_send_consumer_key(uint16_t keycode, int hold_duration) {
    if (!ble_init_done) return false;
    ble51_puts("AT+BLEHIDCONTROLKEY=");
    sprintf(ble51_buf, "%d\n", keycode);
    ble51_puts(ble51_buf);
    const char *result = ble51_gets(TIMEOUT);
    if (memcmp(result, "ERROR", 5) == 0) {
        dprint("error\n");
    }
    return true;
}

#    ifdef MOUSE_ENABLE
/* Send a mouse/wheel movement report.
 * The parameters are signed and indicate positive of negative direction
 * change. */
// bool adafruit_ble_send_mouse_move(int8_t x, int8_t y, int8_t scroll, int8_t pan, uint8_t buttons) {
bool adafruit_ble_send_mouse_move(report_keyboard_t *report) {
    if (!ble_init_done) return false;
    ble51_puts("AT+BLEHIDMOUSEMOVE=");
    sprintf(ble51_buf, "%d,%d,%d,%d\n",
        report->x,
        report->y,
        report->v,
        report->h);
    ble51_puts(ble51_buf);
    const char *result = ble51_gets(TIMEOUT);
    if (memcmp(result, "ERROR", 5) == 0) {
        dprint("error\n");
    }

    ble51_puts("AT+BLEHIDMOUSEBUTTON=");
    sprintf(ble51_buf, "%d\n", report->buttons);
    ble51_puts(ble51_buf);
    if (memcmp(ble51_gets(TIMEOUT), "ERROR", 5) == 0) {
        dprint("error\n");
    }
}
#    endif

/* Compute battery voltage by reading an analog pin.
 * Returns the integer number of millivolts */
uint32_t adafruit_ble_read_battery_voltage(void) {
    dprint("read battery not implemented.\n");
    // not used?
    return 3700;
}

bool adafruit_ble_set_mode_leds(bool on) {
    dprint("set mode leds not implemented.\n");
    // not used?
    return true;
}

bool adafruit_ble_set_power_level(int8_t level) {
    dprint("ble set power level not implemented.\n");
    // Not used?
    return true;
}
