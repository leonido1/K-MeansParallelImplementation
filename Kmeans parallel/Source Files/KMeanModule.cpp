#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>

#include "KMeanModule.h"
#include "KMeansInnerMethods.h"

#include "MpiSupport.h"


#define MAX_FLOAT 3.402823e+38

enum  Messages {Msg_job,Msg_finish};

TimedKMeanResult* timedKMeans(Point* points, int numbPts, int numClust, int iterLim, float qualityReq, float endTime, float deltaT)
{
	float time = 0,estMinQualty;
	float quality = MAX_FLOAT;
	TimedKMeanResult* res;
	
	Cluster* clusters = InitClusters(points, numClust);
		
	KMeans(points,clusters, numbPts, numClust, iterLim);

	estMinQualty = estimateMininalDiameter(clusters,points, numClust, numbPts);

	if (!(estMinQualty>qualityReq)||endTime==0)
		quality = getQualty(clusters, points, numClust, numbPts);

	while (quality>qualityReq && time<endTime)
	{
		time += deltaT;
		
		movePointIntime(points, numbPts, deltaT);
	
		free(clusters);

		clusters = InitClusters(points, numClust);

		KMeans(points, clusters, numbPts, numClust, iterLim);

		//not last iteration
		if (time != endTime)
		{
			estMinQualty = estimateTheQualty(clusters, points, numClust, numbPts);
        
			if (estMinQualty > qualityReq)
				continue;
		}
		quality = getQualty(clusters, points, numClust, numbPts);
	}

	res= makeResult(clusters, numClust, time, quality, qualityReq);
	
	free(clusters);

   return res;
}


Cluster* InitClusters(Point* points, int numClust)
{//The function assumes number of clusters is lower than number of points

	int i;

	Cluster* clusters = (Cluster*)malloc(sizeof(Cluster)*numClust);

	for (i = 0; i < numClust; i++)
	{
		clusters[i].centroid = points[i].coordinates;
		clusters[i].numPts = 0;
	}
	return clusters;
}

boolean groupPointsToClusts(Point* points,Cluster* clusters,int numPts,int numClust)
{
	int i;
	boolean ptMoved = False;
	Cluster* clustPtr;

	for (i = 0; i < numPts; i++)
	{
		clustPtr = points[i].cluster;
		groupPointToClust(&points[i], clusters, numClust);

		if (clustPtr != points[i].cluster)//by ref 
			ptMoved = True;

	}


	if (ptMoved)
		updateClustsSize(clusters,points, numClust,numPts);

	
	return ptMoved;
}

void groupPointToClust(Point* pt, Cluster* clusters, int numClust) 
{
	int i=1;
	Cluster *tempClust = &clusters[0];
	double minDist =fastDist(pt->coordinates, tempClust->centroid), tempDist;

	for (i; i < numClust; i++)
	{
		tempDist = fastDist(pt->coordinates, clusters[i].centroid);
		
		if (tempDist<minDist)
		{
			minDist = tempDist;
			tempClust = &clusters[i];
		}
	}

	pt->cluster = tempClust;
}

float fastDist(Coordinates cord1, Coordinates cord2)
{
	return (cord1.x - cord2.x)*(cord1.x - cord2.x) + (cord1.y - cord2.y)*(cord1.y - cord2.y);

}

float distance(Coordinates cord1, Coordinates cord2) 
{
	return sqrt(pow((cord1.x - cord2.x), 2) + pow((cord1.y - cord2.y), 2));
}

void setRightCentroidsToClusts(Cluster* clusters,Point* pts, int numClust,int numPts) 
{
	int i;

	Coordinates zeroCords = {0,0};

	for ( i = 0; i <numClust; i++)
	{
		
		clusters[i].centroid = zeroCords;
	}

	for (i = 0; i < numPts; i++)
	{

		pts[i].cluster->centroid= cordAdd(pts[i].coordinates, pts[i].cluster ->centroid);
	}
	
	for (i = 0; i <numClust; i++)
	{
		clusters[i].centroid = cordDivByScalar(clusters[i].centroid,(float) clusters[i].numPts);
	}

	
}

void KMeans(Point* points,Cluster* clusts,int numPoints ,int numClusts,int iterLim) 
{
	int i = 0;
    boolean ptMoved;
	groupPointsToClusts(points, clusts, numPoints, numClusts);

	do {
		setRightCentroidsToClusts(clusts, points, numClusts, numPoints);
		ptMoved = groupPointsToClusts(points, clusts, numPoints, numClusts);
		if (!ptMoved)
			break;

		i++;
	} while (i < iterLim);

}

void fillRowWithPtsOfTheClust(Cluster* clust, Point* points, int numPoints, Point** pointsRow) 
{
	int PtsInClust,i,currentIndex;
	
	PtsInClust =clust->numPts;

	*pointsRow = (Point*)malloc(sizeof(Point)*PtsInClust);

	currentIndex = 0;
	for (i = 0; i < numPoints; i++)
	{
		if (points[i].cluster == clust)
		{
			(*pointsRow)[currentIndex] = points[i];
			currentIndex++; 
		}
	}

}


float getClustDeiameter(Cluster* clust, Point* pts)
{
	int i, j;
	double maxDist = 0, currentDist;
	int ptsInClust = clust->numPts;

	for (i = 0; i < ptsInClust; i++)
	{
		for (j = i + 1; j < ptsInClust; j++) 
		{	
			currentDist = fastDist(pts[i].coordinates, pts[j].coordinates);
			
			if (currentDist > maxDist)
				maxDist= currentDist;
		}
	
	}

	return sqrt(maxDist);
}

float getQualty(Cluster* clusts, Point* points, int numClusts, int numPts)
{
	int i;
	float qualty = 0;
	Point** pointsMat = (Point**)malloc(sizeof(Point*)*numClusts);


	for (int i = 0; i < numClusts; i++)
		fillRowWithPtsOfTheClust(&(clusts[i]), points, numPts, &(pointsMat[i]));

	qualty = clalQualty(pointsMat, clusts, numClusts);

	for (i = 0; i < numClusts; i++)
		free(pointsMat[i]);
	
	free(pointsMat);

	
	return qualty;


}

float clalQualty(Point** pointsMat, Cluster* clusts,int numClusts)
{
	int i, j;
	float qualty = 0;
	float * diameters = (float *)malloc(sizeof(float)*numClusts);

	double start_time, end_time;
	start_time = omp_get_wtime();

	int tid;
#pragma omp parallel for
	for (i = 0; i < numClusts; i++) {
		tid = omp_get_thread_num();
		printf("Start: tid = %d index=%d \n",tid,i);
		diameters[i] = getClustDeiameter(&clusts[i], pointsMat[i]);
	}
	end_time = omp_get_wtime();
	printf("Time %g\n", end_time - start_time);


	for (i = 0; i < numClusts; i++)
	{
		for (j = 0; j < numClusts; j++)
		{
			if (i != j)
				qualty += diameters[i] / distance(clusts[i].centroid, clusts[j].centroid);
		}
	}

	qualty /= (numClusts*(numClusts - 1));
	free(diameters);
	return qualty;

}

float estimateTheQualty(Cluster* clusts, Point* points, int numClusts, int numPts)
{
	int i,j;
	float estimateTheQualty=0;
	float * diameters = (float *)malloc(sizeof(float)*numClusts);

	for (i = 0; i < numClusts; i++)
		diameters[i] = estimateMininalDiameter(&clusts[i], points, numClusts, numPts);

	for ( i = 0; i < numClusts; i++)
	{
    	for ( j = 0; j < numClusts; j++)
		{
			if (i != j)
				estimateTheQualty += diameters[i] / distance(clusts[i].centroid,clusts[j].centroid);
        }
    }

	estimateTheQualty /= (numClusts*(numClusts - 1));
	free(diameters);

	return estimateTheQualty;
}

float estimateMininalDiameter(Cluster* clust, Point* points, int numClusts, int numPts) 
{//estimates the minimal diameter with the use of a bounding box
	int i;
	Coordinates firstCords = clust->centroid;
	Coordinates left = firstCords;
	Coordinates right = firstCords;
	Coordinates up = firstCords;
	Coordinates bottom = firstCords;


	for ( i = 0; i <numPts; i++)
	{
		if (points[i].cluster == clust) 
		{
			if (left.x > points[i].coordinates.x)
				left = points[i].coordinates;
		
			if (right.x < points[i].coordinates.x)
				right = points[i].coordinates;
		

			if (up.y < points[i].coordinates.y)
				up = points[i].coordinates;
		

			if (bottom.y > points[i].coordinates.y)
				bottom = points[i].coordinates;
       	}
	
	}

	if (distance(up,bottom)>distance(right, left))
	{
		return distance(up, bottom);
	}
	else return distance(right, left);
	
}
void movePointIntime(Point* points,int numPts,float deltaT)
{
	int i;

	for (i = 0; i < numPts; i++)
	{
		points[i].coordinates.x += points[i].velocity.XAxisVelocity*deltaT;
		points[i].coordinates.y += points[i].velocity.YAxisVelocity*deltaT;
	}
}

TimedKMeanResult* makeResult(  Cluster* clusts,int numClusts,float time,float qualety, float reQualty)
{
	int i;

	TimedKMeanResult* theReslauts = (TimedKMeanResult*)malloc(sizeof(TimedKMeanResult));
	theReslauts->coordinates = (Coordinates*)malloc(sizeof(Coordinates)*numClusts);

	if (qualety <= reQualty)
	{
		theReslauts->reached = True;
    }
	else
		theReslauts->reached = False;


	theReslauts->qualty = qualety;
	theReslauts->numCentroids = numClusts;
	theReslauts->time = time;

	for (i = 0; i < numClusts; i++)
		theReslauts->coordinates[i] = clusts[i].centroid;

	return theReslauts;
}

void updateClustsSize(Cluster* clusts ,Point* points,int numClusts,int numPts)
{
	int i;

	for (i = 0; i < numClusts; i++)
		clusts[i].numPts = 0;

	for (i = 0; i < numPts; i++)
		points[i].cluster->numPts+=1;


}

Coordinates cordAdd(Coordinates cord1, Coordinates cord2)
{
	return{ cord1.x + cord2.x, cord1.y + cord2.y };
}

Coordinates cordDivByScalar(Coordinates cord, float scalar)
{
	return{ cord.x / scalar ,cord.y / scalar };
}