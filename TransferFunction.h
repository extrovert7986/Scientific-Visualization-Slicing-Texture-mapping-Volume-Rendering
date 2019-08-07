#pragma once
#include"TransferFunctionDataType.h"

/***************************************
		transferFunction_t is a data type for storing sample point 
	which user inserted.
		this header file provides following methods for manipulating
	the sample point in the data type:
		1. initial the transfer function sample point into rainbow color
		2. insert sample point into target channel
		3. select a sample point in the transfer function
		4. delete a selected point from transfer function
		5. modify the position of target sample point
		6. merge 2 sample point
*/

#define CHANNEL_R 0
#define CHANNEL_G 1
#define CHANNEL_B 2
#define CHANNEL_A 3
#define BLACK     4

void Initial_TransferFunction(TransferFunction_t** tf);
Point_t* Select_SamplePoint_from_TransferFunction(float x, float y, float bound, Channel_t* channel);
Point_t* Insert_SamplePoint_Into_Channel(float x, float y, Point_t** avaliablePool, Channel_t* channel);
Point_t* Modify_SamplePoint(Point_t* target, float x, float y);
bool Delete_SamplePoint_from_Channel(Point_t** target, Point_t** avaliablePool, Channel_t* channel);
//void Manipulate_Point(unsigned char channel, int point, float x, float y);
//void Merge_Point(unsigned char channel, int i, int j);
//