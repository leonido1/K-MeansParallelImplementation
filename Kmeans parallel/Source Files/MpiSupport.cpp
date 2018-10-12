
#include "MpiSupport.h"
#include "KMeanModule.h"

#define DIMENSIONS 2

void createCordType()
{
	Coordinates cords;

	MPI_Datatype cordMpiType;
	MPI_Datatype dimensionsTypes[DIMENSIONS] = { MPI_FLOAT, MPI_FLOAT };
	int numValsInDimension[DIMENSIONS] = { 1, 1 };
	MPI_Aint memOffSets[DIMENSIONS];
	
	memOffSets[0] = (char)(1);


}
