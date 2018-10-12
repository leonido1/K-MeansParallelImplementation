#pragma once
#include "KMeanModule.h"

//The function assumes the meta data is stored in the first line of the text file. 
float* getMetaData(const char* fileName);

//The function assumes the points start at the second line of the text file.
Point* getPoints(const char* fileName,int numberOfPoints);

void storeResults(TimedKMeanResult* resoults,const char* fileName);
