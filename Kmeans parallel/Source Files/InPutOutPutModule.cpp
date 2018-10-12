
#include <stdio.h>
#include <stdlib.h>

#include "KMeanModule.h"


#define METADATASIZE 6

//--------------------------- Inner functions--------------------------------
//This function assumes the file is open and ready to be read
void populateTheArrayWithMetaDataFromFile(FILE* file, float*metaData);

//This function assumes the file is open and ready to be read from the first line
void populateTheArrayWitPointsFromFile(FILE* file,int numberOfPoints,Point* points);

//This function assumes the file is open and ready to be read from the first line
void skipTheFirstLineInFile(FILE* file);
//--------------------------- End of inner functions-------------------------


float* getMetaData(const char* fileName) {

	FILE* file;
	float* metaData;

	file = fopen(fileName, "r");

	if (file == NULL)
	{
		printf("----Unable to open the file---");
		return NULL;
	}

	metaData = (float*)malloc(METADATASIZE*sizeof(float));

	populateTheArrayWithMetaDataFromFile(file, metaData);

	fclose(file);


	return metaData;

}

void populateTheArrayWithMetaDataFromFile(FILE * file, float * metaData)
{
	int i;

	for (i = 0; i < METADATASIZE; i++)
	{
		fscanf(file,"%f", &metaData[i]);
	}

}

Point* getPoints(const char* fileName, int numberOfPoints) 
{
	FILE* file;
	Point* points;

	file = fopen(fileName, "r");

	if (file == NULL)
	{
		printf("----Unable to open the file---");
		return NULL;
	}

	points = (Point*)malloc(sizeof(Point)*numberOfPoints);

	populateTheArrayWitPointsFromFile(file, numberOfPoints, points);

	fclose(file);

	return points;

}

void storeResults(TimedKMeanResult* resoults,const char * fileName)
{
	int i;
	FILE* file=fopen(fileName,"w");

	if (file==NULL)
	{
		printf("----Unable to open the file---");
		return;
    }

	fprintf(file,"time %f qualty %f \n", resoults->time, resoults->qualty);

    for (i=0;i<resoults->numCentroids;i++)
		fprintf(file, "%f %f\n", resoults->coordinates[i].x, resoults->coordinates[i].y);


	fclose(file);

}

void populateTheArrayWitPointsFromFile(FILE * file, int numberOfPoints, Point * points)
{
	int i;

	skipTheFirstLineInFile(file);

	for (i = 0; i < numberOfPoints; i++)
	{
		fscanf(file,"%f %f",&(points[i].coordinates.x) ,&(points[i].coordinates.y));
		fscanf(file, "%f %f", &(points[i].velocity.XAxisVelocity), &(points[i].velocity.YAxisVelocity));
	}


}

void skipTheFirstLineInFile(FILE * file)
{
	int i;
	double grindVar;

	for (i = 0; i < METADATASIZE; i++)
	{
		fscanf(file, "%lf", &grindVar);
	}


}
