#pragma once
#include"TransferFunctionDataType.h"


void Interpolate_Channel_By_LinearInterpolation(Channel_t* channel, float tex[256]);
void Interpolate_Range_From_Start2End(Point_t* p1, Point_t* p2, float tex[256]);