#ifndef HX711_H_
#define HX711_H_

#include "W7500x.h"

typedef struct _hx711
{
	GPIO_TypeDef* gpioSck;
	GPIO_TypeDef* gpioData;
	uint16_t pinSck;
	uint16_t pinData;
	int gain;
	// 1: channel A, gain factor 128
	// 2: channel B, gain factor 32
  // 3: channel A, gain factor 64
}HX711; //Typedef hx711

void HX711_Init(HX711 sensor);
uint32_t HX711_Read(HX711 sensor);

#endif /* HX711_H_ */
