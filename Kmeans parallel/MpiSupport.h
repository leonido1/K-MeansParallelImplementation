#pragma once
#include "mpi.h"
#include "KMeanModule.h"

#define SINGAL 1
#define ZERO_TAG 0
#define MASTER 0

enum  commandsToSlave { clacDIameter, finish };

void slaveTask();
void releaseSlaves(int numOfSlaves);
void calcDiametersWIthSlaves(Coordinates** cordsMat, Cluster* clusts, int numClusts, float * diameters);
void createCordType(MPI_Datatype *cordMPIType);
