#ifndef K_PDM_MICROPHONE_H
#define K_PDM_MICROPHONE_H

/* Include ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Read nordic id from the registers */
#define PDM_CLK_PIN                         6 //28
#define PDM_DIN_PIN                         5 //30

/* Recording config */
#define AUDIO_SAMPLING_FREQ                 24000 // 16667
#define AUDIO_REC_MS                        50
#define AUDIO_DSP_SAMPLE_LENGTH_MS          16

/* Audio sampling config */
#define AUDIO_SAMPLES_PER_MS                (AUDIO_SAMPLING_FREQ / 1000)
#define AUDIO_REC_SAMPLES                   ((AUDIO_SAMPLING_FREQ * AUDIO_REC_MS) / 1000)
#define AUDIO_DSP_SAMPLE_RESOLUTION         (sizeof(short))
#define AUDIO_DSP_SAMPLE_BUFFER_SIZE        (AUDIO_SAMPLES_PER_MS * AUDIO_DSP_SAMPLE_LENGTH_MS * AUDIO_DSP_SAMPLE_RESOLUTION) //4096


/* Function prototypes ----------------------------------------------------- */
bool microphone_init(void);
bool microphone_inference_start(void);
bool microphone_inference_record(void);
bool microphone_inference_reset_buffers(void);
void microphone_inference_get_data(size_t offset, size_t length, float *out_ptr);
void microphone_inference_get_data_pointer(uint8_t channel, int16_t *out_ptr);
bool microphone_inference_end(void);


#endif