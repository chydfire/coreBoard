#ifndef JIANSENSORS_H_
#define JIANSENSORS_H_

#include "jianboard.h"


void JianSensor_Init(JIANBOARD_J JIANBOARD_Jx, int gain);
uint32_t JianSensor_Read(JIANBOARD_J JIANBOARD_Jx, int gain);

#endif
