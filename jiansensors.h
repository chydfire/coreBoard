#ifndef JIANSENSORS_H_
#define JIANSENSORS_H_

#include "jianboard.h"

typedef enum
{
    Jiansensors_Event_H=0,
    Jiansensors_Event_A,
    Jiansensors_Event_E
}JIANSENSORS_EVENT_Type;

void JianSensor_Init(JIANBOARD_J JIANBOARD_Jx, int gain);
uint32_t JianSensor_Read(JIANBOARD_J JIANBOARD_Jx, int gain);

#endif
