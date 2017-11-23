#include "hx711.h"
#include "W7500x_gpio.h"


void HX711_Init(HX711 sensor)
{
	  GPIO_InitTypeDef GPIO_InitDef;
	  PAD_Type Px;
	  
    /*Init sck gpio*/
    GPIO_InitDef.GPIO_Pin = sensor.pinSck;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_Init(sensor.gpioSck, &GPIO_InitDef);
	  if(sensor.gpioSck == GPIOA)
			Px = PAD_PA;
	  if(sensor.gpioSck == GPIOB)
			Px = PAD_PB;
	  if(sensor.gpioSck == GPIOC)
			Px = PAD_PC;
    PAD_AFConfig(Px,sensor.pinSck, PAD_AF1);
    
		/*Init data gpio*/
		GPIO_InitDef.GPIO_Pin = sensor.pinData;
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;
    GPIO_Init(sensor.gpioData, &GPIO_InitDef);
	  if(sensor.gpioData == GPIOA)
			Px = PAD_PA;
	  if(sensor.gpioData == GPIOB)
			Px = PAD_PB;
	  if(sensor.gpioData == GPIOC)
			Px = PAD_PC;
    PAD_AFConfig(Px,sensor.pinData, PAD_AF1);
}

uint32_t HX711_Read(HX711 sensor)
{
	uint32_t val = 0;
	int i = 0;
	
	GPIO_SetBits(sensor.gpioData, sensor.pinData);
	GPIO_ResetBits(sensor.gpioSck, sensor.pinSck);
	while(GPIO_ReadInputDataBit(sensor.gpioData, sensor.pinData));
	for(i=0;i<24;i++)
	{
		GPIO_SetBits(sensor.gpioSck, sensor.pinSck);
		val = val << 1;
		GPIO_ResetBits(sensor.gpioSck, sensor.pinSck);
		if(GPIO_ReadInputDataBit(sensor.gpioData, sensor.pinData))
			val++;
	}
	for(i=0;i<sensor.gain;i++)
	{
		GPIO_SetBits(sensor.gpioSck, sensor.pinSck);
		GPIO_ResetBits(sensor.gpioSck, sensor.pinSck);
	}
	val = val ^ 0x800000;
	
	return val;
}
