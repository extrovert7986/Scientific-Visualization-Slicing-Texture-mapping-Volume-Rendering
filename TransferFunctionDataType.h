#pragma once

struct Point_t {
	float x;
	float y;
	Point_t* next;
};

struct Channel_t {
	int N;
	Point_t* first;
};

struct TransferFunction_t
{
	Channel_t* r;
	Channel_t* g;
	Channel_t* b;
	Channel_t* a;
};