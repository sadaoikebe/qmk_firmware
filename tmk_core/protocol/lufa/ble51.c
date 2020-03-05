#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "host.h"
#include "host_driver.h"
#include "../serial.h"
#include "ble51.h"
#include "debug.h"
#include "timer.h"
#define TIMEOUT 100

/* Host driver */
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

host_driver_t ble51_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

char ble51_buf[30];

__attribute__ ((weak))
void ble51_init(void)
{
    // JTAG disable for PORT F. write JTD bit twice within four cycles.
    MCUCR |= (1<<JTD);
    MCUCR |= (1<<JTD);

    bt_power_init();
    serial_init();
    print("Serial Init\n");
}

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

void ble51_putc(uint8_t c)
{
    serial_send(c);
}

void ble51_puts(char *s)
{
    while (*s)
    serial_send(*s++);
}


static uint8_t leds = 0;
static uint8_t keyboard_leds(void) { return leds; }
void ble51_set_leds(uint8_t l) { leds = l; }

static void send_keyboard(report_keyboard_t *report)
{
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
}
static void send_mouse(report_mouse_t *report)
{
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

static void send_system(uint16_t data)
{
    // system data: 0x81 power 0x82 sleep 0x83 wake
}

static void send_consumer(uint16_t data)
{
    ble51_puts("AT+BLEHIDCONTROLKEY=");
    sprintf(ble51_buf, "%d\n",data);
    ble51_puts(ble51_buf);
    const char *result = ble51_gets(TIMEOUT);
    if (memcmp(result, "ERROR", 5) == 0) {
        dprint("error\n");
    }
}


