#include <stdio.h>
#include <stdlib.h> //-
#include "esp_log.h" //-
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/adc.h" //-
#include "esp_adc_cal.h" //-
#include "adc_read.h"
// //Definiciones ADC
// static const adc_channel_t adc_channel = ADC_CHANNEL_6;
// static const char *TAG_CH[10] = {"ADC1_CH6"};
// #define ADC_ATTEN ADC_ATTEN_DB_11;
// static const adc_bits_width_t adc_width = ADC_WIDTH_BIT_12;
// static const adc_atten_t adc_attenuation = ADC_ATTEN_DB_0;
// static const adc_unit_t adc_unit = ADC_UNIT_1;

// void set_up()
// {
//     //ad1_config_width
// }

void app_main() 
{
    //init_adc();

    uint32_t raw, voltage;

    struct ADC_InPut *DHT_11 = newADC_InPut(ADC_CHANNEL_6, ADC_ATTEN_DB_0, ADC_UNIT_1);

    while(1)
    {
        raw = DHT_11->adc_raw(DHT_11);
        voltage = DHT_11->adc_voltage(DHT_11);
        printf("-Raw: %d\t-Voltage: %dmV\n", raw, voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

