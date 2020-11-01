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

#include "timer.h"
#include <clock.h>

static volatile uint64_t tick_count;

void SysTick_Handler(void)
{
    ++tick_count;
}

void set_time(uint64_t time)
{
    tick_count = time;
}

void timer_init(void)
{
    SysTick_Config(system_cpu_clock_get_hz() / 1000);
}

uint16_t timer_read(void)
{
    return (uint16_t)tick_count;
}

uint32_t timer_read32(void)
{
    return (uint32_t)tick_count;
}

uint64_t timer_read64(void)
{
    return tick_count;
}

uint16_t timer_elapsed(uint16_t tlast)
{
    return TIMER_DIFF_16(timer_read(), tlast);
}

uint32_t timer_elapsed32(uint32_t tlast)
{
    return TIMER_DIFF_32(timer_read32(), tlast);
}

void timer_clear(void)
{
    set_time(0);
}
