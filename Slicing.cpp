#include<math.h>
#include<float.h>
#include<stdio.h>
#include<GL\freeglut.h>
#include"RawDataType.h"
#include"Slicing.h"

void DrawBillboard(float center[3], float mtx[16], float size, Res_t res, Ratio_t ratio);

void ViewAlignedSlicing(float eyePos[], float mtx[], int sampleRate, RawInf_t* inf) {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float d;
	float xbound, ybound, zbound;
	xbound = inf->res.x;
	ybound = inf->res.y;
	zbound = inf->res.z;
	float corner[8][3] = {
		{ -xbound / 2.0,-ybound / 2.0,-zbound / 2.0 },
		{ xbound / 2.0,-ybound / 2.0,-zbound / 2.0 },
		{ xbound / 2.0,ybound / 2.0,-zbound / 2.0 },
		{ -xbound / 2.0,ybound / 2.0,-zbound / 2.0 },
		{ -xbound / 2.0,-ybound / 2.0,zbound / 2.0 },
		{ xbound / 2.0,-ybound / 2.0,zbound / 2.0 },
		{ xbound / 2.0,ybound / 2.0,zbound / 2.0 },
		{ -xbound / 2.0,ybound / 2.0,zbound / 2.0 },
	};

	d = mtx[2] * eyePos[0] + mtx[6] * eyePos[1] + mtx[10] * eyePos[2];
	float furthest, nearest;
	furthest = FLT_MIN;
	nearest = FLT_MAX;
	for (int i = 0; i < 8; i++) {
		//sqrt((-mtx[2])^2 + (-mtx[6]^2 + (-mtx[10])^2)) == 1
		float distance = fabs(-mtx[2] * corner[i][0] - mtx[6] * corner[i][1] - mtx[10] * corner[i][2] + d);
		if (distance > furthest) {
			furthest = distance;
		}
		if (distance < nearest) {
			nearest = distance;
		}
	}

	float deltaD = (furthest - nearest) / sampleRate;
	float billboardCenter[3];
	float size = 2 * sqrt(inf->res.x*inf->res.x + inf->res.y*inf->res.y + inf->res.z*inf->res.z);
	for (float i = furthest; i >= nearest; i -= deltaD) {
		billboardCenter[0] = eyePos[0] + i*(-mtx[2]);
		billboardCenter[1] = eyePos[1] + i*(-mtx[6]);
		billboardCenter[2] = eyePos[2] + i*(-mtx[10]);

		DrawBillboard(billboardCenter, mtx, size, inf->res, inf->ratio);
	}
}




void DrawBillboard(float center[3], float mtx[16], float size, Res_t res, Ratio_t ratio) {
	float vertex[4][3];
	float u[3], v[3];
	float texcoord[4][3];
	u[0] = mtx[0];	u[1] = mtx[4];	u[2] = mtx[8];
	v[0] = mtx[1];	v[1] = mtx[5];	v[2] = mtx[9];
	float rtoX, rtoY, rtoZ;
	float max = ratio.x > ratio.y ? (ratio.x > ratio.z ? ratio.x : ratio.z) : (ratio.y > ratio.z ? ratio.y : ratio.z);
	
	rtoX = ratio.x / max;
	rtoY = ratio.y / max;
	rtoZ = ratio.z / max;

	vertex[0][0] = center[0] - u[0] * size / 2 - v[0] * size / 2;
	vertex[0][1] = center[1] - u[1] * size / 2 - v[1] * size / 2;
	vertex[0][2] = center[2] - u[2] * size / 2 - v[2] * size / 2;

	vertex[1][0] = center[0] + u[0] * size / 2 - v[0] * size / 2;
	vertex[1][1] = center[1] + u[1] * size / 2 - v[1] * size / 2;
	vertex[1][2] = center[2] + u[2] * size / 2 - v[2] * size / 2;

	vertex[2][0] = center[0] + u[0] * size / 2 + v[0] * size / 2;
	vertex[2][1] = center[1] + u[1] * size / 2 + v[1] * size / 2;
	vertex[2][2] = center[2] + u[2] * size / 2 + v[2] * size / 2;

	vertex[3][0] = center[0] - u[0] * size / 2 + v[0] * size / 2;
	vertex[3][1] = center[1] - u[1] * size / 2 + v[1] * size / 2;
	vertex[3][2] = center[2] - u[2] * size / 2 + v[2] * size / 2;

	texcoord[0][0] = vertex[0][0] / (res.x*rtoX) + 0.5;
	texcoord[0][1] = vertex[0][1] / (res.y*rtoY) + 0.5;
	texcoord[0][2] = vertex[0][2] / (res.z*rtoZ) + 0.5;

	texcoord[1][0] = vertex[1][0] / (res.x*rtoX) + 0.5;
	texcoord[1][1] = vertex[1][1] / (res.y*rtoY) + 0.5;
	texcoord[1][2] = vertex[1][2] / (res.z*rtoZ) + 0.5;

	texcoord[2][0] = vertex[2][0] / (res.x*rtoX) + 0.5;
	texcoord[2][1] = vertex[2][1] / (res.y*rtoY) + 0.5;
	texcoord[2][2] = vertex[2][2] / (res.z*rtoZ) + 0.5;

	texcoord[3][0] = vertex[3][0] / (res.x*rtoX) + 0.5;
	texcoord[3][1] = vertex[3][1] / (res.y*rtoY) + 0.5;
	texcoord[3][2] = vertex[3][2] / (res.z*rtoZ) + 0.5;
	
	glBegin(GL_POLYGON);
	glTexCoord3fv(texcoord[0]);	glVertex3fv(vertex[0]);
	glTexCoord3fv(texcoord[1]);	glVertex3fv(vertex[1]);
	glTexCoord3fv(texcoord[2]);	glVertex3fv(vertex[2]);
	glTexCoord3fv(texcoord[3]);	glVertex3fv(vertex[3]);
	glEnd();
}