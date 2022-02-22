/* Include ----------------------------------------------------------------- */
#include "module_uart.h"
#include <sys/printk.h>
#include <drivers/uart.h>
#include <string.h>
#include <inttypes.h>


/* Private variables ------------------------------------------------------- */
static uint8_t uart_buf[UART_BUF_BYTES]; // same byte size as uart_buf_16
static int16_t uart_buf_16[UART_BUF_BYTES_16]; // same byte size as uart_buf
static int uart_start_byte_repeat = 0;
static int uart_k = 0;
static bool uart_low = true;
static struct device *uart_dev;
static struct uart_config uart_cfg;
static int uart_ret;


/* Public functions -------------------------------------------------------- */

void uart_init(){
	uart_dev = device_get_binding("UART_1");
	if (!uart_dev) {
          printk("Could not get UART\n");
	}
        
	uart_ret = uart_config_get(uart_dev, &uart_cfg);
	uart_cfg.baudrate = 1000000;
	uart_cfg.parity = UART_CFG_PARITY_NONE;
	uart_cfg.stop_bits = UART_CFG_STOP_BITS_1;
	uart_cfg.data_bits = UART_CFG_DATA_BITS_8;
	uart_cfg.flow_ctrl = UART_CFG_FLOW_CTRL_RTS_CTS;
	uart_ret = uart_configure(uart_dev, &uart_cfg);
	uart_irq_callback_set(uart_dev, uart_cb_int16);
	uart_irq_rx_enable(uart_dev);
        printk("UART start!\n");
}

int uart_send_data(const uint8_t *buf, size_t size)
{
	if (size == 0) {
                return 0;
	}
	for(int i = 0; i < size; i++){
                uart_poll_out(uart_dev, buf[i]);
	}
	return 0;
}

void uart_cb(struct device *x)
{
	uart_irq_update(x);
	int data_length = 0;

	if (uart_irq_rx_ready(x)) {
                data_length = uart_fifo_read(x, uart_buf, sizeof(uart_buf));
                uart_buf[data_length] = 0;
	}
	printk("%s", uart_buf);
}

int uart_send_data_int16(int16_t *buf, size_t size)
{
	if (size == 0) {
		return 0;
	}

        // send start byte
        for(int i = 0; i < UART_START_BYTE_REPEAT; i++){
                uart_poll_out(uart_dev, UART_START_BYTE);
        }

        // send bytes
        uint8_t uart_u8[sizeof(uint16_t)];
        uint16_t uart_u16;
	for(int i = 0; i < size; i++){
                uart_u16 = (uint16_t)(buf[i]);
                uart_u8[0] =(uart_u16) & 0xFF;
                uart_u8[1] = (uart_u16 >> 8) & 0xFF;
                //check conversion
                //int16_t uart16;
                //int k2;
                //for(int k = 0; k < 1; k++){
                //  k2 = k * 2;
                //  uart_u16 = (uart_u8[k2] | uart_u8[k2+1] << 8);
                //  uart16 = (int16_t) uart_u16;
                //  uart_buf_16[k] = uart16;
                //  printk("%d \n", uart_buf_16[k]);
                //}
                for(int j = 0; j < sizeof(int16_t); j++){
                        uart_poll_out(uart_dev, uart_u8[j]);
                }
	}
	return 0;
}

void uart_cb_int16(struct device *x)
{
        uart_irq_update(x);
        int data_length = 0;

        if (uart_irq_rx_ready(x)) {
          data_length = uart_fifo_read(x, uart_buf, sizeof(uart_buf));
        }

        uint16_t uart_u16;
        int16_t uart16;
        uint8_t uart_u8;
        for(int k = 0; k < data_length; k++){
                if(uart_buf[k] == UART_START_BYTE){ // update start byte
                        uart_start_byte_repeat += 1;

                }else{
                        uart_start_byte_repeat = 0;
                }

                if(uart_start_byte_repeat == UART_START_BYTE_REPEAT){  // set uart buffer to start
                        uart_k = 0;
                        uart_low = true;
                        continue;
                }
                if(uart_low){ // update lower byte
                        uart_buf_16[uart_k] = (int16_t) uart_buf[k];
                        uart_low = false;
                }else{ // update higher byte
                        uart_u8 = (uint8_t)uart_buf_16[uart_k];
                        uart_u16 = uart_u8 | uart_buf[k] << 8;
                        uart16 = (int16_t) uart_u16;
                        uart_buf_16[uart_k] = uart16;
                        printk("%d \n", uart_buf_16[uart_k]);
                        uart_low = true;
                        if(uart_k < UART_BUF_BYTES_16){ // if start byte missed: wait
                                uart_k += 1; // else, increase uart buffer position
                        }
                }

        }
}
