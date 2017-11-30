#include "hx711.h"
#include "W7500x_gpio.h"

/**
 * @brief  GPIOs Config of hx711.
 * @param  hx711 sensor
 * @retval None
 */
void HX711_Init(HX711 sensor)
{
	  GPIO_InitTypeDef GPIO_InitDef;
	  PAD_Type Px;
	  
    /*Init sck gpio*/
    GPIO_InitDef.GPIO_Pin = sensor.pinSck;                  //SCK pin number
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_OUT;                 //SCK pin output mode
    GPIO_Init(sensor.gpioSck, &GPIO_InitDef);               //SCK pin init
	  if(sensor.gpioSck == GPIOA)
			Px = PAD_PA;
	  if(sensor.gpioSck == GPIOB)
			Px = PAD_PB;
	  if(sensor.gpioSck == GPIOC)
			Px = PAD_PC;
    PAD_AFConfig(Px,sensor.pinSck, PAD_AF1);                //SCK pin set function2
    
		/*Init data gpio*/
		GPIO_InitDef.GPIO_Pin = sensor.pinData;                 //DT pin number
    GPIO_InitDef.GPIO_Mode = GPIO_Mode_IN;                  //DT pin input mode
    GPIO_Init(sensor.gpioData, &GPIO_InitDef);              //DT pin init
	  if(sensor.gpioData == GPIOA)
			Px = PAD_PA;
	  if(sensor.gpioData == GPIOB)
			Px = PAD_PB;
	  if(sensor.gpioData == GPIOC)
			Px = PAD_PC;
    PAD_AFConfig(Px,sensor.pinData, PAD_AF1);               //DT pin set function2
}

/**
 * @brief  Data read of hx711.
 * @param  hx711 sensor
 * @retval Data of hx711 sensor
 */
uint32_t HX711_Read(HX711 sensor)
{
	uint32_t val = 0;
	int i = 0;
	
	GPIO_SetBits(sensor.gpioData, sensor.pinData);                        //DT = 1
	GPIO_ResetBits(sensor.gpioSck, sensor.pinSck);                        //SCK = 0
	while(GPIO_ReadInputDataBit(sensor.gpioData, sensor.pinData));        //Wait DT = 0;
	for(i=0;i<24;i++)
	{
		GPIO_SetBits(sensor.gpioSck, sensor.pinSck);
		val = val << 1;
		GPIO_ResetBits(sensor.gpioSck, sensor.pinSck);
		if(GPIO_ReadInputDataBit(sensor.gpioData, sensor.pinData))
			val++;
	}
	for(i=0;i<sensor.gain;i++)                                            //Through these gain sequences, select next sampling channel
	{
		GPIO_SetBits(sensor.gpioSck, sensor.pinSck);
		GPIO_ResetBits(sensor.gpioSck, sensor.pinSck);
	}
	val = val ^ 0x800000;
	
	return val;
}
