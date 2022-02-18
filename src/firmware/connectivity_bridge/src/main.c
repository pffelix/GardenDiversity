
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
 * Audio sensor
 */

#include <drivers/i2s.h>
#include <stdlib.h>
#include <string.h>

#define I2S_RX_NODE DT_NODELABEL(i2s_rx)
#define I2S_RX_TIMEOUT 10000
#define AUDIO_SAMPLE_FREQ 44100
#define AUDIO_SAMPLES_PER_CH_PER_BUF 128
#define AUDIO_NUM_CHANNELS 2
#define AUDIO_SAMPLES_PER_BUF (AUDIO_SAMPLES_PER_CH_PER_BUF * AUDIO_NUM_CHANNELS)
#define AUDIO_SAMPLE_BIT_WIDTH 24
#define AUDIO_BUF_BYTES (AUDIO_SAMPLES_PER_BUF * AUDIO_SAMPLE_BIT_WIDTH / 8)
#define AUDIO_BUF_COUNT 64
#define AUDIO_BUF_BYTES_ALIGN 4
K_MEM_SLAB_DEFINE(i2s_rx_mem_slab, AUDIO_BUF_BYTES, AUDIO_BUF_COUNT, AUDIO_BUF_BYTES_ALIGN);
static const struct device *host_i2s_rx_dev;
static struct i2s_config i2s_rx_cfg;
static int ret;

static void audio_init(void)
{
	/*configure rx device*/
	host_i2s_rx_dev = DEVICE_DT_GET(I2S_RX_NODE);
	if (!host_i2s_rx_dev) {
		LOG_ERR("unable to find i2s_rx device\n");
	}

	/* configure i2s for audio playback */
	i2s_rx_cfg.word_size = AUDIO_SAMPLE_BIT_WIDTH;
	i2s_rx_cfg.channels = AUDIO_NUM_CHANNELS;
	i2s_rx_cfg.format = I2S_FMT_DATA_FORMAT_I2S;
	i2s_rx_cfg.options = I2S_OPT_BIT_CLK_MASTER | I2S_OPT_FRAME_CLK_MASTER;
	i2s_rx_cfg.frame_clk_freq = AUDIO_SAMPLE_FREQ;
	i2s_rx_cfg.mem_slab = &i2s_rx_mem_slab;
	i2s_rx_cfg.block_size = AUDIO_BUF_BYTES;
	i2s_rx_cfg.timeout = I2S_RX_TIMEOUT;
	ret = i2s_configure(host_i2s_rx_dev, I2S_DIR_RX, &i2s_rx_cfg);
	if (ret != 0) {
		LOG_ERR("i2s_configure failed with %d error\n", ret);
	}
}

void main(void)
{
        /*
        * Audio sensor
        */

	audio_init();

	/* start i2s rx driver */
	ret = i2s_trigger(host_i2s_rx_dev, I2S_DIR_RX, I2S_TRIGGER_START);
	if (ret != 0) {
		LOG_ERR("i2s_trigger failed with %d error\n", ret);
	}

        void *rx_mem_block;
        rx_mem_block = k_malloc(AUDIO_BUF_BYTES);
        size_t size;
        int16_t sample;
	while (true) {
        	/* receive data */
		ret = i2s_buf_read(host_i2s_rx_dev, rx_mem_block, &size);
        	if (ret != 0) {
        		LOG_ERR("i2s_read failed with %d error\n", ret);
        	}
                for (int i = 0; i < AUDIO_SAMPLES_PER_BUF; ++i) {
                        sample = ((int16_t *)rx_mem_block)[i] >> 16;
                }
	}
        k_free(rx_mem_block);

        /*
        * Connectivity bridge
        */
        if (event_manager_init()) {
		LOG_ERR("Event manager not initialized\n");
	} else {
		module_set_state(MODULE_STATE_READY);
	}
}

