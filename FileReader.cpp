#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include"FileReader.h"

void SearchInf(char *returnInf, char *target, char *inf);
void GetResolution(char* input, Res_t* res);
void GetSampleType(char* input, SAMPLETYPE* sampleType);
void GetRatio(char* input, Ratio_t* ratio);
void GetEndian(char* input, ENDIAN* endian);
/*****************************************/
void AllocateRawData(RawData_t* raw, int size);
float BinaryToFloat(unsigned char* data, int index, RawInf_t* rawInf);
void Record_Max_and_Min_Amount(RawData_t* raw, RawInf_t* rawInf, int secmax, int secmin, Counter_t* counter);
void ComputeGradient(RawData_t* raw, RawInf_t* rawInf);
int  Index(int i, int j, int k, Res_t* res);

/*this function is used to read raw information file*/
void Read_RawInf(const char* infFileName, RawInf_t* rawInf) {
	int fileSize;
	char *infText;
	char temp[100];

	FILE *fp = fopen(infFileName, "r");

	if (!fp) {
		printf("File not found.\n");
		system("PAUSE");
		exit(0);
	}
	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	rewind(fp);

	infText = (char*)malloc(fileSize * sizeof(char));
	if (infText == NULL) {
		printf("the text file allocate memory failed.\n");
		system("PAUSE");
		exit(0);
	}
	fread(infText, sizeof(char), fileSize, fp);
	fclose(fp);

	/*raw file name*/
	SearchInf(rawInf->fileName, "raw-file", infText);

	if (!strcmp(rawInf->fileName, "\0")) {
		printf("The raw information file didn't have raw file name.\n");
		system("PAUSE");
		exit(0);
	}
	/*rawfile resolution*/
	SearchInf(temp, "resolution", infText);
	GetResolution(temp, &(rawInf->res));
	rawInf->size = rawInf->res.x*rawInf->res.y*rawInf->res.z;

	/*rawfile sample type*/
	SearchInf(temp, "sample-type", infText);
	GetSampleType(temp, &(rawInf->sampleType));

	/*rawfile ratio*/
	SearchInf(temp, "ratio", infText);
	GetRatio(temp, &rawInf->ratio);

	/*rawfile endian*/
	SearchInf(temp, "endian", infText);
	GetEndian(temp, &rawInf->endian);

	//free inf text
	free(infText);
	printf("********************  raw information  ********************\n");
	printf("raw file name: %s\n", rawInf->fileName);
	printf("resolution: %d,%d,%d\n", rawInf->res.x, rawInf->res.y, rawInf->res.z);
	printf("sample type number: %d\n", rawInf->sampleType);
	printf("ratio: %f:%f:%f\n", rawInf->ratio.x, rawInf->ratio.y, rawInf->ratio.z);
	printf("endian: %d\n", rawInf->endian);
	printf("***********************************************************\n");
}

void SearchInf(char *returnInf, char *target, char *inf)
{
	char *ptr;
	ptr = strstr(inf, target);
	if (ptr == NULL) {
		strcpy(returnInf, "\0");
	}
	else {
		sscanf(ptr, "%*[^=]=%[\"0-9a-zA-Z, .:_]", returnInf);
	}
}

void GetResolution(char* input, Res_t* res) {
	if (!strcmp(input, "\0")) {
		printf("The raw information file without resoluation definition.\n");
		system("PAUSE");
		exit(0);
	}
	sscanf(input, "%dx%dx%d", &res->x, &res->y, &res->z);
}

void GetSampleType(char* input, SAMPLETYPE* sampleType) {
	if (!strcmp(input, "\0")) {
		printf("The raw information file without sample type definition or the sample type is not define, set default unsigned char.\n");
		*sampleType = UNSIGNED_CHAR;
	}
	if (!strcmp(input, "unsigned char")) {
		*sampleType = UNSIGNED_CHAR;
	}
	else if (!strcmp(input, "unsigned short")) {
		*sampleType = UNSIGNED_SHORT;
	}
	else if (!(strcmp(input, "float"))) {
		*sampleType = FLOAT_POINT;
	}
}

void GetRatio(char* input, Ratio_t* ratio) {
	if (!strcmp(input, "\0")) {
		printf("The raw information file without ratio definition, set default 1.0:1.0:1.0.\n");
		ratio->x = 1.0;
		ratio->y = 1.0;
		ratio->z = 1.0;
	}
	sscanf(input, "%f:%f:%f", &ratio->x, &ratio->y, &ratio->z);
}

void GetEndian(char* input, ENDIAN* endian) {
	if (!strcmp(input, "\0")) {
		printf("The raw information file without endian definition, set dafault little endian.\n");
		*endian = LITTLE;
	}
	if (!strcmp(input, "big")) {
		*endian = BIG;
	}
	else if (!strcmp(input, "little")) {
		*endian = LITTLE;
	}
}

/*****************************************************************
**                        read raw data                         **
*****************************************************************/

void Read_Raw_Data(RawData_t* raw, RawInf_t* rawInf, Counter_t* counter) {
	unsigned char* temp;
	float* ftemp;

	//allocate memory
	AllocateRawData(raw, rawInf->size);

	//open file
	//put raw data in the folder under project
	char fileName[60] = ".\\RawData\\";
	strcat(fileName, rawInf->fileName);
	FILE *fp = fopen(fileName, "rb");
	if (!fp) {
		printf("Can not find the raw data under the path of project.\n");
		system("PAUSE");
		exit(0);
	}
	fseek(fp, sizeof(unsigned char), SEEK_END);
	int rawSzie = ftell(fp);
	rewind(fp);
	//read data into temp
	if (rawInf->sampleType != FLOAT_POINT) {
		temp = (unsigned char*)malloc(rawSzie * sizeof(unsigned char));
		fread(temp, sizeof(unsigned char), rawSzie, fp);
	}
	else {
		ftemp = (float*)malloc(rawInf->size * sizeof(float));
		fread(ftemp, sizeof(float), rawInf->size, fp);
	}
	fclose(fp);


	//insert data number into count array
	float secMin, secMax;
	secMax = raw->maxInt = -1.0;
	secMin = raw->minInt = 65535.0;
	for (int index = 0; index < rawInf->size; index++) {
		float current;
		if (rawInf->sampleType != FLOAT_POINT) {
			current = BinaryToFloat(temp, index, rawInf);
		}
		else {
			current = ftemp[index];
		}
		raw->intensity[index] = current;

		//find out the second max number
		if (current > raw->maxInt) {
			secMax = raw->maxInt;
			raw->maxInt = current;
		}
		else if (current > secMax && current < raw->maxInt) {
			secMax = current;
		}
		//find out the second small number
		if (current < raw->minInt) {
			secMin = raw->minInt;
			raw->minInt = current;
		}
		else if (current < secMin && current > raw->minInt) {
			secMin = current;
		}
	}
	//count each single intensity
	Record_Max_and_Min_Amount(raw, rawInf, secMax, secMin, counter);
	printf("max count:%d\n", counter->max);

	free(temp);
	printf("max intensity:%f min intensity:%f\n", raw->maxInt, raw->minInt);
	ComputeGradient(raw, rawInf);
	printf("max gradient:%f min gradient:%f\n", raw->maxGra, raw->minGra);
	
}

void AllocateRawData(RawData_t* raw, int size) {
	raw->intensity = (float*)malloc(size * sizeof(float));

	raw->gradient = (Gradient_t*)malloc(size * sizeof(Gradient_t));
	if (raw->gradient == NULL) {
		printf("Allocate raw data gradient memory failed.\n");
		system("PAUSE");
		exit(0);
	}
}

float BinaryToFloat(unsigned char* data, int index, RawInf_t* rawInf) {
	if (rawInf->sampleType == UNSIGNED_CHAR) {
		return (float)data[index];
	}

	else if (rawInf->sampleType == UNSIGNED_SHORT) {
		if (rawInf->endian == BIG) {
			return (float)(data[index * 2] * 256 + data[index * 2 + 1]);
		}
		else { //little endian
			return (float)(data[index * 2] + data[index * 2 + 1] * 256);
		}
	}
	return 0;
}

void Record_Max_and_Min_Amount(RawData_t* raw,RawInf_t* rawInf,int secmax,int secmin,Counter_t* counter) {
	counter->range = (int)(raw->maxInt - raw->minInt) + 1;
	if (counter->range <=0 )return;
	counter->count = (int*)calloc(counter->range, sizeof(int));
	//fill each count shell
	
	for (int i = 0; i < rawInf->size; i++) {
		float current = raw->intensity[i];
		counter->count[(int)(current - raw->minInt)]++;
	}
	//find out max
	counter->max = counter->count[secmin];
	for (int i = 0; i <= (int)(secmax-secmin); i++) {
		if (counter->max < counter->count[secmin + i])
			counter->max = counter->count[secmin + i];
	}
}

void ComputeGradient(RawData_t* raw, RawInf_t* rawInf) {
	int index;
	int x1, x2, y1, y2, z1, z2;
	int dx, dy, dz;
	int max_num = 0;

	raw->maxGra = 0.0;
	raw->minGra = 255.0;

	for (int k = 0; k < rawInf->res.z; k++) {
		for (int j = 0; j < rawInf->res.y; j++) {
			for (int i = 0; i < rawInf->res.x; i++) {
				index = i + j*rawInf->res.x + k*rawInf->res.x*rawInf->res.y;
				/*x axis*/
				if (i == 0) {
					x1 = i + 1;
					x2 = i;
					dx = 1;
				}
				else if (i == (rawInf->res.x - 1)) {
					x1 = i;
					x2 = i - 1;
					dx = 1;
				}
				else {
					x1 = i + 1;
					x2 = i - 1;
					dx = 2;
				}
				/*y axis*/
				if (j == 0) {
					y1 = j + 1;
					y2 = j;
					dy = 1;
				}
				else if (j == (rawInf->res.y - 1)) {
					y1 = j;
					y2 = j - 1;
					dy = 1;
				}
				else {
					y1 = j + 1;
					y2 = j - 1;
					dy = 2;
				}
				/*z axis*/
				if (k == 0) {
					z1 = k + 1;
					z2 = k;
					dz = 1;
				}
				else if (k == (rawInf->res.z - 1)) {
					z1 = k;
					z2 = k - 1;
					dz = 1;
				}
				else {
					z1 = k + 1;
					z2 = k - 1;
					dz = 2;
				}
				/*Compute Gradient*/

				raw->gradient[index].x = (raw->intensity[Index(x1, j, k, &(rawInf->res))] - raw->intensity[Index(x2, j, k, &(rawInf->res))]) / dx;
				raw->gradient[index].y = (raw->intensity[Index(i, y1, k, &(rawInf->res))] - raw->intensity[Index(i, y2, k, &(rawInf->res))]) / dy;
				raw->gradient[index].z = (raw->intensity[Index(i, j, z1, &(rawInf->res))] - raw->intensity[Index(i, j, z2, &(rawInf->res))]) / dz;

				raw->gradient[index].magnitude = sqrt((raw->gradient[index].x)*(raw->gradient[index].x) + (raw->gradient[index].y)*(raw->gradient[index].y) + (raw->gradient[index].z)*(raw->gradient[index].z));
				if (raw->gradient[index].magnitude > raw->maxGra) {
					raw->maxGra = raw->gradient[index].magnitude;
				}
				if (raw->gradient[index].magnitude < raw->minGra) {
					raw->minGra = raw->gradient[index].magnitude;
				}
			}
		}
	}
}

int Index(int i, int j, int k, Res_t* res) {
	return i + j*res->x + k*res->x*res->y;
}