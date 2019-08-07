#pragma once
#include"RawDataType.h"
#include"CounterDataType.h"

void Read_RawInf(const char* infFileName, RawInf_t* rawInf);
void Read_Raw_Data(RawData_t* raw, RawInf_t* rawInf, Counter_t* count);