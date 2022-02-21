
/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 /*
 * Connectivity Bridge
 */

#include <zephyr.h>
#include <event_manager.h>
#define MODULE main
#include "module_state_event.h"
#include <logging/log.h>
LOG_MODULE_REGISTER(MODULE);

#define USB_SERIALNUMBER_TEMPLATE "THINGY91_%04X%08X"

static uint8_t usb_serial_str[] = "THINGY91_12PLACEHLDRS";

/* Overriding weak function to set iSerialNumber at runtime. */
uint8_t *usb_update_sn_string_descriptor(void)
{
	snprintk(usb_serial_str, sizeof(usb_serial_str), USB_SERIALNUMBER_TEMPLATE,
				(uint32_t)(NRF_FICR->DEVICEADDR[1] & 0x0000FFFF)|0x0000C000,
				(uint32_t)NRF_FICR->DEVICEADDR[0]);

	return usb_serial_str;
}

 /*
 * Microphone sensor
 */
#include "microphone.h"

 /*
 * UART communication between nrf52840 and nrf9160
 */

#include <sys/printk.h>
#include <drivers/uart.h>
#include <string.h>

static uint8_t uart_buf[1024];
static struct device *uart_dev; 
struct uart_config uart_cfg;
int uart_ret;

int send_data(const uint8_t *buf, size_t size)
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


void main(void)
{


        /*
        * UART communication between nrf52840 and nrf9160
        */

	uart_dev = device_get_binding("UART_0");
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
        uart_irq_callback_set(uart_dev, uart_cb);
	uart_irq_rx_enable(uart_dev);
	printk("UART 52840 start!\n");
	char hello[] = "hello from 52840 \n";
	
	while (1) {
		send_data(hello, sizeof(hello));
		k_sleep(K_MSEC(1000));
	}

        /*
        * Microphone sensor
        */
        
        bool ret;
        ret = microphone_inference_start();
        if(ret){
          ret = microphone_inference_record();
        }
        float *sample = 0;
        for(size_t t = 0; t < AUDIO_REC_SAMPLES; t++){
          microphone_inference_get_data(t, 1, sample);
        }

        /*
        * Connectivity bridge
        */
        if (event_manager_init()) {
		printk("Event manager not initialized\n");
	} else {
		module_set_state(MODULE_STATE_READY);
	}


}

