#ifndef BLE51_TASK_H
#define BLE51_TASK_H

#include <stdbool.h>
#include "ble51.h"

extern uint8_t BLE51_PowerState;  //no_sleep 0, work 1,dozing 2, check battery once when dozing 3, sleeping 4,  lock mode 10

void ble51_task_init(void);
void ble51_task(void);

#endif
