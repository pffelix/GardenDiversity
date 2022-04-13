/* Include ----------------------------------------------------------------- */
#include "pdm_microphone.h"
#include <nrfx_pdm.h>
#include <string.h>
#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>

/* Buffers for receiving PDM mic data */
static int16_t pdm_buffer_temp[2][AUDIO_DSP_SAMPLE_BUFFER_SIZE] = {0};
int16_t *current_buff;
bool write_data = false;

/** Status and control struct for inferencing struct */
typedef struct {
    int16_t *buffers[2];
    uint8_t buf_select;
    uint8_t buf_ready;
    uint32_t buf_count;
    uint32_t n_samples;
} inference_t;


/* Private variables ------------------------------------------------------- */
static bool record_ready = false;
static inference_t inference;
static int16_t max_audio_lvl = 0;


/* Private functions ------------------------------------------------------- */


/**
 * @brief      Inference audio callback, store samples in ram buffer
 *             Signal when buffer is full, and swap buffers
 * @param      buffer   Pointer to source buffer
 * @param[in]  n_bytes  Number of bytes to write
 */
static void audio_buffer_inference_callback(void *buffer, uint32_t n_bytes)
{
    int16_t *samples = (int16_t *)buffer;

    for(uint32_t i = 0; i < (n_bytes >> 1); i++) {
        inference.buffers[inference.buf_select][inference.buf_count++] = samples[i];

        if(inference.buf_count >= inference.n_samples) {
            inference.buf_select ^= 1;
            inference.buf_count = 0;
            inference.buf_ready = 1;
        }
    }
}

/**
 * @brief Get the max diff value from the sample buffer
 * @param buffer Pointer to source buffer
 * @param n_bytes Number of bytes in buffer
 */
static void audio_sanity_check_callback(void *buffer, uint32_t n_bytes)
{
    int16_t *samples = (int16_t *)buffer;
    int16_t prev_sample = samples[0];

    for(uint32_t i = 1; i < (n_bytes >> 1); i++) {

        int16_t diff_sample = abs(prev_sample - samples[i]);
        if(max_audio_lvl < diff_sample) {
            max_audio_lvl = diff_sample;
        }
        prev_sample = samples[i];
    }
    record_ready = false;
}

/**
 * @brief      PDM receive data handler
 * @param[in]  p_evt  pdm event structure
 */
static void pdm_data_handler(nrfx_pdm_evt_t const * p_evt)
{
    nrfx_err_t err = NRFX_SUCCESS;
    static uint8_t buf_toggle = 0;

    if(p_evt->error != 0){
        printk("PDM handler error ocured\n");
        printk("pdm_data_handler error: %d, %d  \n", p_evt->error, p_evt->buffer_requested);
        return;
    }
    if(true == p_evt->buffer_requested){
        buf_toggle ^= 1;
        err = nrfx_pdm_buffer_set(pdm_buffer_temp[buf_toggle], AUDIO_DSP_SAMPLE_BUFFER_SIZE);
        if(err != NRFX_SUCCESS){
            printk("PDM buffer init error: %d\n", err);
        }
    }
    if(p_evt->buffer_released != NULL){
            write_data = true;
            current_buff = pdm_buffer_temp[buf_toggle];
    }
}

/**
 * @brief      PDM receive data handler
 * @param[in]  p_evt  pdm event structure
 */
static void pdm_inference_data_handler(nrfx_pdm_evt_t const * p_evt)
{
    nrfx_err_t err = NRFX_SUCCESS;
    static uint8_t buf_toggle = 0;

    if(p_evt->error != 0){
        printk("PDM handler error ocured\n");
        printk("pdm_data_handler error: %d, %d  \n", p_evt->error, p_evt->buffer_requested);
        return;
    }
    if(true == p_evt->buffer_requested){
        buf_toggle ^= 1;
        err = nrfx_pdm_buffer_set(pdm_buffer_temp[buf_toggle], AUDIO_DSP_SAMPLE_BUFFER_SIZE);
        if(err != NRFX_SUCCESS){
            printk("PDM buffer init error: %d\n", err);
        }
    }
    if(p_evt->buffer_released != NULL){
        current_buff = pdm_buffer_temp[buf_toggle];
        audio_buffer_inference_callback(&pdm_buffer_temp[buf_toggle], sizeof(pdm_buffer_temp)/2);
    }
}

/**
 * @brief      Capture 2 channel pdm data every 100 ms.
 *             Waits for new data to be ready.
 *             Creates a 1 channel pdm array and calls callback function
 * @param[in]  callback  Callback needs to handle the audio samples
 */
static void get_dsp_data(void (*callback)(void *buffer, uint32_t n_bytes))
{
    //TODO: check the number of bytes
    if(write_data == true){
        callback((void *)current_buff, sizeof(pdm_buffer_temp)/2);
        write_data = false;
    }
}



/**
 * @brief Get a full sample buffer and run sanity check
 * @return true microphone is working
 * @return false stops audio and return
 */
static bool do_sanity_check(void)
{
    max_audio_lvl = 0;
    record_ready = true;
    write_data = false;

    while (record_ready == true) {
        get_dsp_data(&audio_sanity_check_callback);
    }

    if(max_audio_lvl < 10) {
        printk("\r\nERR: No audio recorded, is the microphone connected?\r\n");
        nrfx_err_t err;
        err = nrfx_pdm_stop();
        printk("kStateFinished\n");
        return false;
    }
    else {
        return true;
    }
}

/**
 * @brief PDM clock frequency calculation based on 32MHz clock and
 * decimation filter ratio 80
 * @details For more info on clock generation:
 * https://infocenter.nordicsemi.com/index.jsp?topic=%2Fps_nrf5340%2Fpdm.html
 * @param sampleRate in Hz
 * @return uint32_t clk value
 */
static uint32_t pdm_clock_calculate(uint64_t sampleRate)
{
    const uint64_t PDM_RATIO = 80ULL;
    const uint64_t CLK_32MHZ = 32000000ULL;
    uint64_t clk_control = 4096ULL * (((sampleRate * PDM_RATIO) * 1048576ULL) / (CLK_32MHZ + ((sampleRate * PDM_RATIO) / 2ULL)));

    return (uint32_t)clk_control;
}

/**
 * @brief Set the up nrf pdm object, call pdm init
 *
 * @param event_handler
 * @param sample_rate in Hz
 * @return false on error
 */
static bool setup_nrf_pdm(nrfx_pdm_event_handler_t  event_handler, uint32_t sample_rate)
{
    nrfx_err_t err;

    // turn on PDM device
    //const struct device* dev = device_get_binding("GPIO_0");
    //gpio_pin_configure(dev, PDM_CLK_PIN, GPIO_OUTPUT_ACTIVE); 
    //gpio_pin_configure(dev, PDM_DIN_PIN, GPIO_INPUT); 
    //gpio_pin_set(dev, PDM_CLK_PIN, 1);
    //k_msleep(100);

    /* PDM driver configuration */
    nrfx_pdm_config_t config_pdm = NRFX_PDM_DEFAULT_CONFIG(PDM_CLK_PIN, PDM_DIN_PIN);
    config_pdm.clock_freq = (nrf_pdm_freq_t)pdm_clock_calculate(sample_rate);
    config_pdm.ratio = NRF_PDM_RATIO_64X; //NRF_PDM_RATIO_80X
    config_pdm.edge = NRF_PDM_EDGE_LEFTRISING; // NRF_PDM_EDGE_LEFTRISING
    config_pdm.gain_l = NRF_PDM_GAIN_MAXIMUM;
    config_pdm.gain_r = NRF_PDM_GAIN_MAXIMUM;

    /* PDM interrupt configuration necessary for Zephyr */
    IRQ_DIRECT_CONNECT(PDM_IRQn, 5, nrfx_pdm_irq_handler, 0);


    err = nrfx_pdm_init(&config_pdm, event_handler);
    if(err != NRFX_SUCCESS){
        return false;
    }
    else{
        return true;
    }
}

/* Public functions -------------------------------------------------------- */


bool microphone_inference_start(void)
{
    nrfx_err_t err;

    nrfx_pdm_uninit();
    if(!setup_nrf_pdm(pdm_data_handler, AUDIO_SAMPLING_FREQ)) {
        return false;
    }
    err = nrfx_pdm_start();
    if(err != NRFX_SUCCESS){
        return false;
    }

    k_msleep(2000);
    /* Since we have no feedback from the PDM sensor, do a sanity check on the data stream */
    if(do_sanity_check() == false) {
        //return false;
    }
    err = nrfx_pdm_stop();

    inference.buffers[0] = (int16_t *)k_malloc(AUDIO_REC_SAMPLES * sizeof(int16_t));

    if(inference.buffers[0] == NULL) {
        return false;
    }

    inference.buffers[1] = (int16_t *)k_malloc(AUDIO_REC_SAMPLES * sizeof(int16_t));

    if(inference.buffers[1] == NULL) {
        k_free(inference.buffers[0]);
        return false;
    }

    inference.buf_select = 0;
    inference.buf_count  = 0;
    inference.n_samples  = AUDIO_REC_SAMPLES;
    inference.buf_ready  = 0;

    nrfx_pdm_uninit();
    if(!setup_nrf_pdm(pdm_inference_data_handler, AUDIO_SAMPLING_FREQ)) {
        return false;
    }
    err = nrfx_pdm_start();
    if(err != NRFX_SUCCESS){
        return false;
    }

    return true;
}

bool microphone_inference_record(void)
{
    bool ret = true;

    if (inference.buf_ready == 1) {
        printk(
            "Error sample buffer overrun. Decrease the number of slices per model window "
            "(k_CLASSIFIER_SLICES_PER_MODEL_WINDOW)\n");
        ret = false;
    }

    while (inference.buf_ready == 0) {
    };
 
    inference.buf_ready = 0;

    return ret;
}

/**
 * @brief      Reset buffer counters for non-continuous inferencing
 */
bool microphone_inference_reset_buffers(void)
{
    inference.buf_ready = 0;
    inference.buf_count = 0;

    return true;
}

/**
 * Get raw audio signal data in float format
 */
void microphone_inference_get_data(size_t offset, size_t length, float *out_ptr)
{
    size_t i;

    for(i = 0; i < length; i++) {
        *(out_ptr + i) = (float)inference.buffers[inference.buf_select ^ 1][offset + i]
        / ((float)(1 << 15));
    }
}

/**
 * Get raw audio signal data pointer in int16_t format
 */
void microphone_inference_get_data_pointer(uint8_t channel, int16_t *out_ptr)
{
    out_ptr = inference.buffers[channel];
}



bool microphone_inference_end(void)
{
    uint32_t nrfx_err;
    record_ready = false;

    nrfx_err = nrfx_pdm_stop();
    if(nrfx_err != NRFX_SUCCESS)
    {
        printk("PDM Could not stop PDM sampling, error = %d", nrfx_err);
    }

    k_free(inference.buffers[0]);
    k_free(inference.buffers[1]);
    return true;
}
