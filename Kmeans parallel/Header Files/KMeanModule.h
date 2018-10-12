#pragma once

struct TimedKMeanResult;
struct Cluster;
struct Point;
struct Coordinates;
struct Velocity;

TimedKMeanResult* timedKMeans(Point* points, int numbPts, int numKlust, int iterLim, float qualtyRequ, float endT, float deltaT);

enum boolean { False, True };

struct Coordinates
{
	float x;
	float y;
};

struct Velocity
{
	float  XAxisVelocity;
	float  YAxisVelocity;
};

struct Point
{
	Cluster* cluster;
	Coordinates coordinates;
	Velocity velocity;
};

struct Cluster
{
	Coordinates centroid;
	int numPts;
};

struct TimedKMeanResult
{
	float time;
	float qualty;
	int numCentroids;
	Coordinates *coordinates;
	boolean reached;
};





