#pragma once
#include"KMeanModule.h"

//Initializes clusters and set defualt centroids
Cluster* InitClusters(Point* points,  int numClust);
//groups point to their correct clust retures true if the grouping have changed
boolean groupPointsToClusts(Point* points, Cluster* clusters, int numPts, int numClust);
void groupPointToClust(Point *pt, Cluster* clusters, int numClust);
float distance(Coordinates cord1, Coordinates cord2);
/*this is an optimized distance function used only to compare distances, 
the result needs to be squered rooted in order to obtain the true distance.*/ 
float fastDist(Coordinates cord1, Coordinates cord2);
void setRightCentroidsToClusts(Cluster* clusters, Point* pts, int numClust, int numPts);
void KMeans(Point* points, Cluster* clusts, int numPoints, int numClusts, int iterLim);
void fillRowWithPtsOfTheClust(Cluster* clust, Point* points, int numPoints, Point** pointsRow);
//This function assumes the pts are points belonging to the cluster clust
float getClustDeiameter(Cluster* clust, Point* pts);
float estimateMininalDiameter(Cluster* clusts, Point* points, int numClusts, int numPts);
float getQualty(Cluster* clusts, Point* points, int numClusts, int numPts);
float clalQualty(Point** pointsMat, Cluster* clusts, int numClusts);
float estimateTheQualty(Cluster* clusts, Point* points, int numClusts, int numPts);
void movePointIntime(Point* points, int numPts, float deltaT);
TimedKMeanResult* makeResult(Cluster* clusts, int numClusts, float time, float qualety, float reQualty);
//Update clustes sizes to correct sizes
void updateClustsSize(Cluster*clusters, Point* points, int numClust, int numPts);
Coordinates cordAdd(Coordinates cord1, Coordinates cord2);
Coordinates cordDivByScalar(Coordinates cord, float scalar);
