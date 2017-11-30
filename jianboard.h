#ifndef JIANBOARD_H_
#define JIANBOARD_H_

#include "W7500x_gpio.h"

typedef struct _jianboard_j
{
	GPIO_TypeDef* gpioPin2;
	uint16_t pin2;
	GPIO_TypeDef* gpioPin1;
	uint16_t pin1;
}JIANBOARD_J; //Typedef Jx connector, GPIO type and pin number



#endif
