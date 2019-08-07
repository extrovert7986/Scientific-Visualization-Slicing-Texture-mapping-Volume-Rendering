#pragma once
/*** type of file information ***/
/************
SAMPLE TYPE : 
	1.unsigned char  : 1 byte
	2.unsigned short : 2 byte
*/
enum SAMPLETYPE { UNSIGNED_CHAR, UNSIGNED_SHORT , FLOAT_POINT};

/************
ENDIAN :
	1.big-endian    : MSB at the front
	2.little-endian : LSB at the front
*/
enum ENDIAN { BIG, LITTLE };

/************
RESOLUTION :
	including resolution of 3 axis, x, y, and z.
	storing type : int
*/
struct Res_t {
	int x;
	int y;
	int z;
};

/************
RATIO :
	including ratio of 3 axis, x, y, and z.
	storing type : float
*/
struct Ratio_t {
	float x;
	float y;
	float z;
};

/************
RAW INFORMATION :
	including : file name, a 50 char size string.
				resolution, recording the resolution.
				size, multiply by resolutions of 3 axis.
				ratio, recording the ratio.
				sample type, recording the sample type of the data.
				endian, recording the endian of the data, this data will influence the reading order.
*/
struct RawInf_t {
	char fileName[50];
	Res_t res;
	int size;
	Ratio_t ratio;
	SAMPLETYPE sampleType;
	ENDIAN endian;
};

/*** type of raw data ***/
/************
GRADIENT :
	including gradient of 3 axis and the magnitude of the gradient;
	storing type : float
*/
struct Gradient_t{
	float x;
	float y;
	float z;
	float magnitude;
};

/************
RAW DATA :
	including : data intensity 
				max, min intensity
				gradient
				max, min gradient
*/
struct RawData_t {
	float* intensity;
	float maxInt, minInt;
	Gradient_t* gradient;
	float maxGra, minGra;
};