#ifndef K_MODULE_UART_H
#define K_MODULE_UART_H
/* Include ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>
#include <device.h>
#include <drivers/uart.h>

/* UART config ----------------------------------------------------------------- */
#define UART_START_BYTE 0
#define UART_START_BYTE_REPEAT 3
#define UART_BUF_BYTES 1024
#define UART_LABEL DT_NODELABEL(uart1)
#define UART_BAUDRATE 1000000
#define UART_TIMEOUT 100

/* Function prototypes ----------------------------------------------------- */

void uart_init();
int uart_send_data(const uint8_t *buf, size_t size);
void uart_irq_callback(struct device *x, void *user_data);
void uart_init_int16();
int uart_send_data_int16(int16_t *buf, size_t size);
void uart_irq_callback_int16(struct device *x, void *user_data);
void uart_process_buffer_int16(uint8_t* buffer, int buffer_length);
void uart_async_init_int16(void);
void uart_async_callback_int16(const struct device *uart_dev, struct uart_event *evt, void *user_data);
int uart_async_send_data_int16(int16_t *buf, size_t size);

#endif