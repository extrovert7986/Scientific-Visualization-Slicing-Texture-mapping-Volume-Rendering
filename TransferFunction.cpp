#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"Color.h"
#include"TransferFunction.h"

void Initial_Channel(Channel_t** channel, unsigned char color);

void Initial_TransferFunction(TransferFunction_t** tf) {
	//allocate the memory of transfer function
	(*tf) = (TransferFunction_t*)calloc(1, sizeof(TransferFunction_t));
	//allocate channel's memory
	Initial_Channel(&((*tf)->r), CHANNEL_R);
	Initial_Channel(&((*tf)->g), CHANNEL_G);
	Initial_Channel(&((*tf)->b), CHANNEL_B);
	Initial_Channel(&((*tf)->a), CHANNEL_A);

}

void Initial_Channel(Channel_t** channel, unsigned char color) {
	(*channel) = (Channel_t*)calloc(1, sizeof(Channel_t));
	(*channel)->N = 10;
	Point_t* temp = (*channel)->first = (Point_t*)calloc(1, sizeof(Point_t));
	temp->x = 0;
	temp->y = (color == CHANNEL_A) ? DEFAULT_ALPHA : rainbow_bgyr_35_85_c72_n256[0][color];
	for (int i = 1; i < 10; i++) {
		temp->next = (Point_t*)calloc(1, sizeof(Point_t));
		temp = temp->next;
		temp->x = 255 * i / 9;
		temp->y = (color == CHANNEL_A) ? DEFAULT_ALPHA : rainbow_bgyr_35_85_c72_n256[i][color];
	}
	temp->next = (*channel)->first;
}

Point_t* Select_SamplePoint_from_TransferFunction(float x, float y, float bound, Channel_t* channel) {
	Point_t* target = channel->first;
	for (int i = 0; i < channel->N; i++) {
		if (fabsf(x - target->x) < bound&&fabsf(y - target->y) <= 0.05)return target;
		/*float dist = sqrtf(powf((x - target->x), 2) + 40*fabsf(y - target->y));
		if (dist < bound) return target;*/
		target = target->next;
	}
	return nullptr;
}

Point_t* Insert_SamplePoint_Into_Channel(float x, float y, Point_t** avaliablePool, Channel_t* channel) {
	Point_t* insert;

	//insert data return avaliable pool first point if there has point inside the pool. 
	if (*avaliablePool==NULL)insert = (Point_t*)calloc(1, sizeof(Point_t));
	else {
		insert = *avaliablePool;
		(*avaliablePool) = (*avaliablePool)->next;
	}
	insert->x = x;
	insert->y = y;

	//find out where to insert the point
	Point_t* temp = channel->first;
	while (temp->next != nullptr) {
		//check if point inside the interval
		if (temp->x <= insert->x && temp->next->x >= insert->x) {
			insert->next = temp->next;
			temp->next = insert;
			channel->N += 1;
			return insert;
		}
		temp = temp->next;
	}
	return nullptr;
}

Point_t* Modify_SamplePoint(Point_t* target, float x, float y) {
	if (!target)return nullptr;
	target->x = x;
	target->y = y;
	return target;
}

bool Delete_SamplePoint_from_Channel(Point_t** target,Point_t** avaliablePool,Channel_t* channel) {
	if ((*target)->x == 0 || (*target)->x == 255) {
		printf("head and end node cannot deleted.\n");
		return false;
	}
	Point_t* temp = channel->first;
	for (int i = 0; i < channel->N; i++) {
		if (temp->next == *target) {
			temp->next = (*target)->next;
			(*target)->next = *avaliablePool;
			*avaliablePool = *target;
			channel->N -= 1;
			return true;
		}
		temp = temp->next;
	}
	return false;
}

bool Merge_SamplePoint(Channel_t* channel, int target1, int target2) {

	return true;
}
