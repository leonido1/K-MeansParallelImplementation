


#include <stdio.h>
#include <stdlib.h> 
#include <math.h>


#include "CudaSupport.h"
#include "KMeansInnerMethods.h"



#define LOAD_BALANCING_FACTOR 2
#define THREADS_PER_BLOCK 1000

int findnumOfExacutions(int numCords);
float findMaxDistPowerInArr(float* diatArr, int numElements);

// populate the max dist arry only with the power ^2 of the distances, and the actual distances
__global__ void findMaxDistKernel(Coordinates*cords, const int numCords,float* maxDistArr,int exacutionIndex)
{

	int id=  threadIdx.x+ exacutionIndex*THREADS_PER_BLOCK;
 int cordsToCalc = numCords /LOAD_BALANCING_FACTOR ;

 if (id +1> numCords)
   return;

 int count=0 , CordIndex=id+1;
	float tempMax ;


	for (CordIndex; CordIndex < numCords&&count<cordsToCalc; CordIndex++)
	{
		tempMax = (cords[id].x - cords[CordIndex].x)*(cords[id].x - cords[CordIndex].x) + (cords[id].y - cords[CordIndex].y)*(cords[id].y - cords[CordIndex].y);

		if (tempMax>maxDistArr[id]) 
			maxDistArr[id] = tempMax;

		count++;
    
	}

 //cyclic manner 

	for (CordIndex = 0; count < cordsToCalc; CordIndex++)
	{
		tempMax = (cords[id].x - cords[CordIndex].x)*(cords[id].x - cords[CordIndex].x) + (cords[id].y - cords[CordIndex].y)*(cords[id].y - cords[CordIndex].y);

		if (tempMax>maxDistArr[id])
			maxDistArr[id] = tempMax;

		count++;
	       
	
	}
	
}

cudaError_t calcDiameterWithCuda(Coordinates* cords,int numCords,float* resultsPtr)
{

	cudaError_t cudaStatus;
	cudaStatus = cudaSetDevice(0);
	
	Coordinates* cordOnDev;
	float* maxsOnDev,*maxsOnHost;


	if (cudaStatus != cudaSuccess) 
	{
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		return cudaStatus;
	}


	cudaStatus = cudaMalloc((void**)&cordOnDev, numCords * sizeof(Coordinates));
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMalloc failed!");
		cudaFree(cordOnDev);
		return cudaStatus;
	}



	cudaStatus = cudaMalloc((void**)&maxsOnDev, numCords * sizeof(float));
	if (cudaStatus != cudaSuccess) 
	{
		fprintf(stderr, "cudaMalloc failed!");
		cudaFree(cordOnDev);
		cudaFree(maxsOnDev);
		return cudaStatus;
	}

	cudaMemset(maxsOnDev, 0, numCords * sizeof(float));

	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMemset failed!");
		cudaFree(cordOnDev);
		cudaFree(maxsOnDev);
		return cudaStatus;
	}



	cudaStatus = cudaMemcpy(cordOnDev, cords,numCords * sizeof(Coordinates), cudaMemcpyHostToDevice);
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stderr, "cudaMemcpy failed!");
		cudaFree(cordOnDev);
		cudaFree(maxsOnDev);
		return cudaStatus;
	}


	int exacutionIndex;
	int numOfExacutions = findnumOfExacutions(numCords);
 float t1, t2;


	for (exacutionIndex =0; exacutionIndex < numOfExacutions; exacutionIndex++)
	{
 
		findMaxDistKernel << <1, THREADS_PER_BLOCK >> > (cordOnDev, numCords , maxsOnDev, exacutionIndex);

		cudaStatus = cudaGetLastError();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "calcDiameterWithCuda launch failed: %s\n", cudaGetErrorString(cudaStatus));
			cudaFree(cordOnDev);
			cudaFree(maxsOnDev);
			return  cudaStatus;
		}


		cudaStatus = cudaDeviceSynchronize();
		if (cudaStatus != cudaSuccess) {
			fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
			cudaFree(cordOnDev);
			cudaFree(maxsOnDev);
			return cudaStatus;
		}

	
 
 }

	maxsOnHost = (float*)malloc(sizeof(float)*numCords);
	
	cudaStatus = cudaMemcpy(maxsOnHost, maxsOnDev, numCords * sizeof(float), cudaMemcpyDeviceToHost);

             
	*resultsPtr=(float)sqrt((double)findMaxDistPowerInArr(maxsOnHost, numCords));
	
	cudaFree(maxsOnDev);
	cudaFree(cordOnDev);
	free(maxsOnHost);

	return cudaStatus;



}



float findMaxDistPowerInArr (float* diatArr,int numElements)
{
	int i;
	float maxDist = 0;

	for (i = 0; i < numElements; i++) 
	{
	
		if (maxDist < diatArr[i])
			maxDist = diatArr[i];
	
	}


	return maxDist;

}


int findnumOfExacutions(int numCords)
{

	if (numCords%THREADS_PER_BLOCK == 0) 
	{

		return numCords / THREADS_PER_BLOCK;
	}
	else { return (numCords / THREADS_PER_BLOCK) + 1; }
	
}


void freeDeviceMeme(void ** ptrArr,int numPtrs)
{

	int i = 0;

	for (i = 0; i < numPtrs; i++)
		cudaFree(ptrArr[i]);

}




//(int *c, const int *a, const int *b, unsigned int size)
/*  int *dev_a = 0;
int *dev_b = 0;
int *dev_c = 0;
cudaError_t cudaStatus;

// Choose which GPU to run on, change this on a multi-GPU system.
cudaStatus = cudaSetDevice(0);
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
goto Error;
}

// Allocate GPU buffers for three vectors (two input, one output)    .
cudaStatus = cudaMalloc((void**)&dev_c, size * sizeof(int));
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaMalloc failed!");
goto Error;
}

cudaStatus = cudaMalloc((void**)&dev_a, size * sizeof(int));
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaMalloc failed!");
goto Error;
}

cudaStatus = cudaMalloc((void**)&dev_b, size * sizeof(int));
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaMalloc failed!");
goto Error;
}

// Copy input vectors from host memory to GPU buffers.
cudaStatus = cudaMemcpy(dev_a, a, size * sizeof(int), cudaMemcpyHostToDevice);
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaMemcpy failed!");
goto Error;
}

cudaStatus = cudaMemcpy(dev_b, b, size * sizeof(int), cudaMemcpyHostToDevice);
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaMemcpy failed!");
goto Error;
}

// Launch a kernel on the GPU with one thread for each element.
addKernel<<<1, size>>>(dev_c, dev_a, dev_b);

// Check for any errors launching the kernel
cudaStatus = cudaGetLastError();
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
goto Error;
}

// cudaDeviceSynchronize waits for the kernel to finish, and returns
// any errors encountered during the launch.
cudaStatus = cudaDeviceSynchronize();
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
goto Error;
}

// Copy output vector from GPU buffer to host memory.
cudaStatus = cudaMemcpy(c, dev_c, size * sizeof(int), cudaMemcpyDeviceToHost);
if (cudaStatus != cudaSuccess) {
fprintf(stderr, "cudaMemcpy failed!");
goto Error;
}

Error:
cudaFree(dev_c);
cudaFree(dev_a);
cudaFree(dev_b);

return cudaStatus;
*/

