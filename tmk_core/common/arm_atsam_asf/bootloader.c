/* Copyright 2020 Hanyazou
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

#include "bootloader.h"
#include <system/system.h>
#include <wdt/wdt.h>

void bootloader_jump(void)
{
    struct wdt_conf config_wdt;

    printf("Set WDT to invoke the bootloader and stay in programming mode.\r\n");
    wdt_get_config_defaults(&config_wdt);
    config_wdt.always_on = false;
    config_wdt.timeout_period = WDT_PERIOD_2048CLK;
    wdt_set_config(&config_wdt);

    // wait for bite
    while (true);
}
