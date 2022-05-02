#include "adc_read.h"
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"

static void check_efuse(void)
{
#if CONFIG_IDF_TARGET_ESP32
    //Check if TP is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("eFuse Two Point: NOT supported\n");
    }
    //Check Vref is burned into eFuse
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK) {
        printf("eFuse Vref: Supported\n");
    } else {
        printf("eFuse Vref: NOT supported\n");
    }
#elif CONFIG_IDF_TARGET_ESP32S2
    if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
        printf("eFuse Two Point: Supported\n");
    } else {
        printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
    }
#else
#error "This example is configured for ESP32/ESP32S2."
#endif
}


static void print_char_val_type(esp_adc_cal_value_t val_type)
{
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        printf("Characterized using Two Point Value\n");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        printf("Characterized using eFuse Vref\n");
    } else {
        printf("Characterized using Default Vref\n");
    }
}

struct ADC_InPut *newADC_InPut(adc_channel_t channel, adc_atten_t atten, adc_unit_t unit)
{
    struct ADC_InPut *self = (struct ADC_InPut*) malloc (sizeof(struct ADC_InPut) + 1);
    self->channel = channel;
    self->atten = atten;
    self->unit = unit;
    self->adc_raw = &adc_raw;
    self->adc_voltage = &adc_voltage;
    init_adc(self);

    return self;
}

void init_adc(struct ADC_InPut *self)
{
    //Check if Two Point or Vref are burned into eFuse
    check_efuse();

    //Configure ADC
    if (self->unit == ADC_UNIT_1) {
        adc1_config_width(width);
        adc1_config_channel_atten(self->channel, self->atten);
    } else {
        adc2_config_channel_atten((adc2_channel_t)self->channel, self->atten);
    }

    //Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(self->unit, self->atten, width, DEFAULT_VREF, adc_chars);
    print_char_val_type(val_type);

    
}

uint32_t adc_raw (struct ADC_InPut *self)
{
    //Continuously sample ADC1
    //while (1) {
        uint32_t adc_reading = 0;
        //Multisampling
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            if (self->unit == ADC_UNIT_1) {
                adc_reading += adc1_get_raw((adc1_channel_t)self->channel);
            } else {
                int raw;
                adc2_get_raw((adc2_channel_t)self->channel, width, &raw);
                adc_reading += raw;
            }
        }
        adc_reading /= NO_OF_SAMPLES;
        //Convert adc_reading to voltage in mV
    //    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    //    printf("Raw: %d\tVoltage: %dmV\n", adc_reading, voltage);
    //    vTaskDelay(pdMS_TO_TICKS(1000));

        return adc_reading;
    //}
}


uint32_t adc_voltage(struct ADC_InPut *self)
{
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_raw(self), adc_chars);
    return voltage;
}