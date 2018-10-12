#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include"KMeanModule.h"
cudaError_t calcDiameterWithCuda(Coordinates* cord, int numCords, float* resultsPtr);


