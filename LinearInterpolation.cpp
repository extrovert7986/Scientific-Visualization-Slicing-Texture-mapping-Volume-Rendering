
#include"LinearInterpolation.h"

float Compute_Slope(float x1, float x2, float y1, float y2);

void Interpolate_Channel_By_LinearInterpolation(Channel_t* channel, float tex[256]) {
	Point_t* p1 = channel->first;
	Point_t* p2 = p1->next;
	for (int i = 0; i < channel->N - 1; i++) {
		float slope = Compute_Slope(p1->x, p2->x, p1->y, p2->y);
		for (int i = p1->x; i <= p2->x; i++) {
			float deltax = (i - p1->x>0) ? (i - p1->x) : 0;
			tex[i] = p1->y + (deltax)*slope;
		}
		p1 = p2;
		p2 = p2->next;
	}
}

void Interpolate_Range_From_Start2End(Point_t* p1,Point_t* p2,float tex[256]) {
	
	float slope = Compute_Slope(p1->x, p2->x, p1->y, p2->y);
	for (int i = p1->x; i <= p2->x; i++) {
		float deltax = (i - p1->x>0) ? (i - p1->x) : 0;
		tex[i] = p1->y + (deltax)*slope;
	}
	
}

float Compute_Slope(float x1, float x2, float y1, float y2) {
	return (y2 - y1) / (x2 - x1);
}

