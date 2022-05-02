#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"


#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          //Multisampling

static esp_adc_cal_characteristics_t *adc_chars;
#if CONFIG_IDF_TARGET_ESP32
//static const adc_channel_t channel = ADC_CHANNEL_6;     //GPIO34 if ADC1, GPIO14 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_12;
#elif CONFIG_IDF_TARGET_ESP32S2
// static const adc_channel_t channel = ADC_CHANNEL_6;     // GPIO7 if ADC1, GPIO17 if ADC2
static const adc_bits_width_t width = ADC_WIDTH_BIT_13;
#endif
// static const adc_atten_t atten = ADC_ATTEN_DB_0;
// static const adc_unit_t unit = ADC_UNIT_1;

struct ADC_InPut
{
    adc_channel_t channel; // = ADC_CHANNEL_6;
    adc_atten_t atten; //= ADC_ATTEN_DB_0;
    adc_unit_t unit; // = ADC_UNIT_1;
    uint32_t (*adc_raw) ();
    uint32_t (*adc_voltage)();
};

struct ADC_InPut *newADC_InPut(adc_channel_t, adc_atten_t, adc_unit_t);

static void check_efuse(void);
static void print_char_val_type(esp_adc_cal_value_t val_type);
void init_adc(struct ADC_InPut*);
uint32_t adc_raw (struct ADC_InPut*);
uint32_t adc_voltage(struct ADC_InPut*);