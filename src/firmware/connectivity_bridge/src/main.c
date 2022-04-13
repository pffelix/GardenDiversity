
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
#include "pdm_microphone.h"
#include "pdm_microphone_test.h"
 /*
 * UART communication between nrf52840 and nrf9160
 */
#include "module_uart.h"

void main(void)
{


	/*
	* UART communication between nrf52840 and nrf9160
	*/
        // Synchronous
 //       uart_async_init_int16();
	//char hello[] = "hello from 52840 \n";
 //       uint16_t hello_int16[] = {-5284, 5284, -1000, 1000, -32768, 32767};
 //       while(1){
 //               //send_data(hello, sizeof(hello));
 //               uart_async_send_data_int16(hello_int16, sizeof(hello_int16) / sizeof(int16_t));
 //               k_sleep(K_MSEC(1000));
 //       }


        /*
        * Microphone sensor
        */
        test_microphone();


        //bool ret;
        //ret = microphone_inference_start();
        //if(ret){
        //  printk("microphone inference started\n");
        //  while(true){
        //    ret = microphone_inference_record();
        //  }
        //}
        //int16_t* sample_ptr;
        //microphone_inference_get_data_pointer(1, sample_ptr);
        //uart_send_data_int16(sample_ptr, AUDIO_REC_SAMPLES);

        //float *sample = 0;
        //for(size_t t = 0; t < AUDIO_REC_SAMPLES; t++){
          //microphone_inference_get_data(t, 1, sample);
        //}

        /*
        * Connectivity bridge
        */
 //       if (event_manager_init()) {
	//	printk("Event manager not initialized\n");
	//} else {
	//	module_set_state(MODULE_STATE_READY);
	//}


}

