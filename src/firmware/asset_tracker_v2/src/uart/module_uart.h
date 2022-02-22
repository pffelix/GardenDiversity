#ifndef K_MODULE_UART_H
#define K_MODULE_UART_H
/* Include ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>
#include <device.h>

/* UART config ----------------------------------------------------------------- */
#define UART_START_BYTE 0
#define UART_START_BYTE_REPEAT 3
#define UART_BUF_BYTES 1024
#define UART_BUF_BYTES_16 (UART_BUF_BYTES / 2)

/* Function prototypes ----------------------------------------------------- */

void uart_init();
int uart_send_data(const uint8_t *buf, size_t size);
void uart_cb(struct device *x);
int uart_send_data_int16(int16_t *buf, size_t size);
void uart_cb_int16(struct device *x);

#endif