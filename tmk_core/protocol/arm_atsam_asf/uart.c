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

#include <usart/usart.h>

#include "config.h"
#include "keyboard.h"
#include "arm_atsam_asf/uart.h"

#ifdef CONSOLE_ENABLE

static struct usart_module usart_instance;

void configure_uart(void) {
    struct usart_config config_usart;
    usart_get_config_defaults(&config_usart);

    config_usart.baudrate    = 115200;
    config_usart.mux_setting = CONF_UART_SERCOM_MUX_SETTING;
    config_usart.pinmux_pad0 = CONF_UART_SERCOM_PINMUX_PAD0;
    config_usart.pinmux_pad1 = CONF_UART_SERCOM_PINMUX_PAD1;
    config_usart.pinmux_pad2 = CONF_UART_SERCOM_PINMUX_PAD2;
    config_usart.pinmux_pad3 = CONF_UART_SERCOM_PINMUX_PAD3;

    while (usart_init(&usart_instance, CONF_UART_MODULE, &config_usart) != STATUS_OK) { }
    usart_enable(&usart_instance);
}

int _write(void *fd, const char *msg, size_t len)
{
    usart_write_buffer_wait(&usart_instance, (const uint8_t*)msg, len);

    return len;
}

int _read(void *fd, char *msg, size_t len)
{
    usart_read_buffer_wait(&usart_instance, (uint8_t *)msg, len);

    return len;
}

#else /* CONSOLE_ENABLE */

static void configure_usart(void) {
}

int _write(void *fd, const char *msg, size_t len) {
    return len;
}

int _read(void *fd, char *msg, size_t len) {
    return len;
}

#endif /* CONSOLE_ENABLE */
