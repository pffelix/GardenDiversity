/* Include ----------------------------------------------------------------- */
#include "microphone_test.h"

#include <stdio.h>
#include <stdint.h>
#include <zephyr.h>
#include <nrfx_power.h>
#include <nrfx_pdm.h>
#include <hal/nrf_pdm.h>
//#include <nrfx_log.h>
#define RMS_BUFFER_SIZE 4096
#define MAX_SOUND_PRESSURE_LEVEL 120 /*<-- change for specific mic*/

#define PDM_BUF_ADDRESS 0x20000000 // buffer address in RAM
#define PDM_BUF_SIZE 512
#define CLK_PIN 6 /*<-- change for specific pin*/
#define DIN_PIN 5 /*<-- change for specific pin*/

int16_t pdm_buf[PDM_BUF_SIZE];
int16_t rms_buffer[RMS_BUFFER_SIZE]; 
uint16_t rms_buffer_cursor = 0;



double sqrt(double value)
{
	int i;
	double sqrt = value / 3;

	if (value <= 0) {
		return 0;
	}

	for (i = 0; i < 6; i++) {
		sqrt = (sqrt + value / sqrt) / 2;
	}

	return sqrt;
}


double calculate_rms(int16_t *buffer, uint16_t num_samples) {
  	uint64_t sum = 0;

  	for (uint16_t j = 0; j < num_samples; j++) {
    	rms_buffer[rms_buffer_cursor] = buffer[j];
  		rms_buffer_cursor = (rms_buffer_cursor + 1) % RMS_BUFFER_SIZE;
  	}

  	for (uint16_t j = 0; j < RMS_BUFFER_SIZE; j++) {
    	sum += rms_buffer[j] * rms_buffer[j];
  	}
	double tosquirt = sum / RMS_BUFFER_SIZE;
  	
	return sqrt(tosquirt); // return sqrt(tosquirt)
}

void audio_callback(int16_t *buffer, uint16_t size) { 

  	// Calculate RMS of last 250 ms 
  	double rms = calculate_rms(buffer, size); 
	printk("RMS value: %f\n", rms);
}



void nrfx_pdm_event_handler(nrfx_pdm_evt_t const *const p_evt)
{
	if (p_evt->buffer_requested) {
		nrfx_pdm_buffer_set(pdm_buf, PDM_BUF_SIZE);						
	}
	if (p_evt->buffer_released != 0) {
		
		audio_callback(pdm_buf, PDM_BUF_SIZE);
		
	}
}

static void pdm_init(void)
{
    nrfx_pdm_config_t pdm_config = NRFX_PDM_DEFAULT_CONFIG(CLK_PIN, DIN_PIN);
    //pdm_config.ratio = NRF_PDM_RATIO_64X; //NRF_PDM_RATIO_80X
    //pdm_config.edge = NRF_PDM_EDGE_LEFTRISING;
    //pdm_config.mode = NRF_PDM_MODE_STEREO;

    nrfx_pdm_init(&pdm_config, nrfx_pdm_event_handler);
   
}

void test_microphone(void)
{
	nrfx_err_t ret;
	printk("Starting PDM program!\n");
	printk("PDM Buffer size: %d\n", (PDM_BUF_SIZE * 4));
	printk("PDM Starting Address: %x\n", PDM_BUF_ADDRESS);
	pdm_init();

	ret = nrfx_pdm_start();
	if (ret == NRFX_SUCCESS)
	{
		printk("Pdm start was successfull\n");
	}
	else {
		printk("Pdm start was NOT successfull\n");
	}
	
	// ret = nrfx_pdm_stop();
	// if (ret == NRFX_SUCCESS)
	// {
	// 	printk("Pdm stop was successfull\n");
	// }
	// else {
	// 	printk("Pdm stop was NOT successfull\n");
	// }

}