
#include "KMeanModule.h"
#include "InPutOutPutModule.h"
#include "MemoryManagerModule.h"
#include "MpiSupport.h"

enum metaDataOrder { N , K  , T , dT , LIMIT , QM };
enum commandLineArgs {ProgramName,InPut,OuPut};

#define MASTER 0

int main(int argc, char **argv)
{

	float tStart, tEnd;

	int myrank, size;
	float* metaData;
	Point* points;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	if (myrank == MASTER)
	{
		tStart = MPI_Wtime();

		metaData = getMetaData("D:/Kmeans parallel/IO files/Input.txt");

    	points = getPoints("D:/Kmeans parallel/IO files/Input.txt", (int)metaData[N]);

		
	
	TimedKMeanResult* res = timedKMeans(points, (int)metaData[N], (int)metaData[K],
		(int)metaData[LIMIT],metaData[QM],metaData[T],metaData[dT],myrank);

		storeResults(res, "D:/Kmeans parallel/IO files/Output.txt");


		freePoints(points);

		freeResults(res);

		releaseSlaves(size-1);
	
		tEnd= MPI_Wtime();
		
		printf("Exacution Time for %d points is %f seconds \n",(int)metaData[N] ,tEnd - tStart);
	
	}
	else {slaveTask(); }
	
	

	MPI_Finalize();
	
	return 0;


}


