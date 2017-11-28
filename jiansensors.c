
#include "jiansensors.h"
#include "hx711.h"

#define K 450

void JianSensor_Init(JIANBOARD_J JIANBOARD_Jx, int gain)
{
	HX711 JianSensor;
	
	JianSensor.gpioSck = JIANBOARD_Jx.gpioPin1;
	JianSensor.gpioData = JIANBOARD_Jx.gpioPin2;
	JianSensor.pinSck = JIANBOARD_Jx.pin1;
	JianSensor.pinData = JIANBOARD_Jx.pin2;
	JianSensor.gain = gain;
	
	HX711_Init(JianSensor);
}

uint32_t JianSensor_Read(JIANBOARD_J JIANBOARD_Jx, int gain)
{
	uint32_t val;
	HX711 JianSensor;
	
	JianSensor.gpioSck = JIANBOARD_Jx.gpioPin1;
	JianSensor.gpioData = JIANBOARD_Jx.gpioPin2;
	JianSensor.pinSck = JIANBOARD_Jx.pin1;
	JianSensor.pinData = JIANBOARD_Jx.pin2;
	JianSensor.gain = gain;
	
	val = HX711_Read(JianSensor)/K;
	
	return val;
}

