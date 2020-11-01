#include <usart/usart.h>

#include "config.h"
#include "keyboard.h"
#include "arm_atsam_asf/uart.h"

#ifdef CONSOLE_ENABLE

static struct usart_module usart_instance;

void configure_uart(void)
{
#ifdef DEBUG_SOFT_SERIAL
	struct port_config port_init;
	port_get_config_defaults(&port_init);

	port_init.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(DEBUG_SOFT_SERIAL_PIN, &port_init);

	port_pin_set_output_level(DEBUG_SOFT_SERIAL_PIN, 1);
	delay_ms(100);
#else
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
#endif /* DEBUG_SOFT_SERIAL */
}

int _write(void *fd, const char *msg, size_t len)
{
#ifdef DEBUG_SOFT_SERIAL
	debug_out(msg, len);
#else
	usart_write_buffer_wait(&usart_instance, (const uint8_t*)msg, len);
#endif

	return len;
}

int _read(void *fd, char *msg, size_t len)
{
#ifdef DEBUG_SOFT_SERIAL
	/* not implemented */
#else
	usart_read_buffer_wait(&usart_instance, (uint8_t *)msg, len);
#endif

	return len;
}
#else /* CONSOLE_ENABLE */
static void configure_usart(void)
{
}

int _write(void *fd, const char *msg, size_t len)
{
	return len;
}

int _read(void *fd, char *msg, size_t len)
{
	return len;
}
#endif /* CONSOLE_ENABLE */
