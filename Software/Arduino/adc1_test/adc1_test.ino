//ESP32-S2 ADC CH0-8 test sketch
//Created: 9/21/2020
//Pins: 
//    IO1 4 I/O/T RTC_GPIO1, GPIO1, TOUCH1, ADC1_CH0
//    IO2 5 I/O/T RTC_GPIO2, GPIO2, TOUCH2, ADC1_CH1
//    IO3 6 I/O/T RTC_GPIO3, GPIO3, TOUCH3, ADC1_CH2
//    IO4 7 I/O/T RTC_GPIO4, GPIO4, TOUCH4, ADC1_CH3
//    IO5 8 I/O/T RTC_GPIO5, GPIO5, TOUCH5, ADC1_CH4
//    IO6 9 I/O/T RTC_GPIO6, GPIO6, TOUCH6, ADC1_CH5
//    IO7 10 I/O/T RTC_GPIO7, GPIO7, TOUCH7, ADC1_CH6
//    IO8 11 I/O/T RTC_GPIO8, GPIO8, TOUCH8, ADC1_CH7
//
//Attenuation must be set to 11dB to read up to 3.9V! All other attenuation settings are too small
//Test only uses RTC mode, DMA mode beyond our requirements

#include "driver/adc.h"
#include "esp_adc_cal.h"

static esp_adc_cal_characteristics_t *adc_chars;
static const adc_unit_t unit = ADC_UNIT_1;

#define DEFAULT_VREF    1100        //Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES 64 //Multisampling
#define atten ADC_ATTEN_DB_11
//Options: ADC_ATTEN_DB_0, ADC_ATTEN_DB_2.5, ADC_ATTEN_DB_6, ADC_ATTEN_DB_11

#define width ADC_WIDTH_BIT_13
//Options: ADC_WIDTH_BIT_9, ADC_WIDTH_BIT_10, ADC_WIDTH_BIT_11, ADC_WIDTH_BIT_12, ADC_WIDTH_BIT_13, ADC_WIDTH_BIT_MAX

void configADC(){
//Set ADC1 resolution to 13-bits, Set ADC1 attenuation to 11dB on all 8 channels, Characterize ADC
    
    esp_err_t width_status = adc1_config_width(width);
    if ( width_status == ESP_OK ) {
      Serial.println("ADC width set!");
    }else{
      Serial.println("Failed to set ADC width!");
    }
 
    for (int channel = 0; channel < 8; channel++){ 
        esp_err_t atten_status = adc1_config_channel_atten((adc1_channel_t)channel,atten);
          if ( atten_status == ESP_OK ) {
            Serial.print("ADC CH");
            Serial.print(channel);
            Serial.println(" attenuation set!");
         }else{
            Serial.print("Failed to set ADC CH");
            Serial.print(channel);
            Serial.println(" attenuation!");
          }
    }
    
    //Characterize ADC
  adc_chars = (esp_adc_cal_characteristics_t *)calloc(1, sizeof(esp_adc_cal_characteristics_t));
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);

}

void readADC(){

    uint32_t adc_reading = 0;
    uint32_t voltage = 0 ;
    
    for (int channel = 0; channel < 8; channel++){ 
      for (int i = 0; i < NO_OF_SAMPLES; i++) { //Gather 64 samples for each channel then average
        adc_reading += adc1_get_raw((adc1_channel_t)channel);
      }
      adc_reading /= NO_OF_SAMPLES;
      
      voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
      Serial.print("ADC Channel ");
      Serial.print(channel);
      Serial.print(":  Voltage: ");
      Serial.print(voltage);
      Serial.print(" mV  Raw: ");
      Serial.println(adc_reading);

      adc_reading = 0;
      voltage = 0;
    }

    delay(5000);
    Serial.println("");
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  if (esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK) {
    printf("eFuse Two Point: Supported\n");
  } else {
    printf("Cannot retrieve eFuse Two Point calibration values. Default calibration values will be used.\n");
  }
  configADC();

}

void loop() {
  readADC();
}
