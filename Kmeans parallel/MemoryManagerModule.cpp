
#include "MemoryManagerModule.h"

#include <stdlib.h>


void freePoints(Point* pts) 
{
	free(pts);
}


void freeResults(TimedKMeanResult* results)
{

	free(results->coordinates);
	free(results);

}