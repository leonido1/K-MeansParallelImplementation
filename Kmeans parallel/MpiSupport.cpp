

#include "stdlib.h"
#include "MpiSupport.h"
#include "KMeanModule.h"
#include  "KMeansInnerMethods.h"
# include "CudaSupport.h"

#define DIMENSIONS 2

void createCordType(MPI_Datatype *cordMPIType)
{
	Coordinates cords;
	MPI_Datatype dimensionsTypes[DIMENSIONS] = { MPI_FLOAT, MPI_FLOAT };
	int numValsInDimension[DIMENSIONS] = { 1, 1 };
	MPI_Aint memDisplacements[DIMENSIONS];
	
	memDisplacements[0] = (char*)(&cords.x)- (char*)&cords;
	memDisplacements[1] = (char*)(&cords.y) - (char*)&cords;

	MPI_Type_create_struct(DIMENSIONS, numValsInDimension, memDisplacements, dimensionsTypes, cordMPIType);
	MPI_Type_commit(cordMPIType);


}

void slaveTask()
{
	commandsToSlave comm;
	MPI_Status status;
	Coordinates*cords ;
	MPI_Datatype cordMPIType;

	int numCords;
	float result, resultCuda;


	MPI_Recv(&comm, SINGAL, MPI_INT, -1, ZERO_TAG, MPI_COMM_WORLD, &status);

	createCordType(&cordMPIType);

	while (comm != finish){


		if (comm == clacDIameter)
		{
			MPI_Recv(&numCords, SINGAL, MPI_INT, MASTER, ZERO_TAG, MPI_COMM_WORLD, &status);

			cords = (Coordinates*)malloc(sizeof(Coordinates)*numCords);
			 
			MPI_Recv(cords, numCords, cordMPIType, MASTER, ZERO_TAG, MPI_COMM_WORLD, &status);

			calcDiameterWithCuda(cords, numCords,&result);
					
			free(cords);

			MPI_Send(&result, SINGAL, MPI_FLOAT, MASTER, ZERO_TAG, MPI_COMM_WORLD);
		}
	

		MPI_Recv(&comm, SINGAL, MPI_INT, MASTER, ZERO_TAG, MPI_COMM_WORLD, &status);

	} 
	

}


void calcDiametersWIthSlaves(Coordinates** cordsMat, Cluster* clusts, int numClusts, float * diameters)
{
	
	
	int i, slave,numCordsInClust ,msg = clacDIameter;

	MPI_Datatype cordMPIType;
	MPI_Status status;

	createCordType(&cordMPIType);

	slave = MASTER + 1;
	for (i = 0; i <numClusts; i++)
	{
		numCordsInClust = clusts[i].numPts;
		MPI_Send(&msg, SINGAL, MPI_INT, slave, ZERO_TAG, MPI_COMM_WORLD);
		MPI_Send(&(numCordsInClust), SINGAL, MPI_INT, slave, ZERO_TAG, MPI_COMM_WORLD);
	    MPI_Send(cordsMat[i], numCordsInClust, cordMPIType, slave, ZERO_TAG, MPI_COMM_WORLD);
	
		slave++;
	}


	slave = MASTER + 1;
	for (i = 0; i < numClusts; i++)
	{
		MPI_Recv(&(diameters[i]), SINGAL, MPI_FLOAT, slave, ZERO_TAG, MPI_COMM_WORLD, &status);
		slave++;
	}
	


}

void releaseSlaves(int numOfSlaves) 
{
	int slave=MASTER+1;
	commandsToSlave command = finish;



	for (slave; slave < numOfSlaves+1; slave++)
	{
		MPI_Send(&command, SINGAL, MPI_INT, slave, ZERO_TAG, MPI_COMM_WORLD);
		
	}
}