#ifndef BLE51_H
#define BLE51_H

#include <stdbool.h>
#include "host_driver.h"

host_driver_t ble51_driver;

void ble51_init(void);
int16_t ble51_getc(void);
const char *ble51_gets(uint16_t timeout);
void ble51_putc(uint8_t c);
void ble51_puts(char *s);

#endif
