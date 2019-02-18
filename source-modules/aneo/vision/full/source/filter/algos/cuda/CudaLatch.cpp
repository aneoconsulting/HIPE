//%HIPE_LICENSE%
#pragma once
#include <algos/cuda/CudaLatch.h>
#include <algos/cuda/Latch.cuh>
#include <cstdlib>
#include <core/HipeStatus.h>


HipeStatus filter::algos::CudaLatch::process()
{
	int *a = (int *)malloc(150);
	int *b = (int *)malloc(150);
	int *c = (int *)malloc(150);

	//kernelAdd(a, b, c, 150);

	return OK;
}

void filter::algos::CudaLatch::dispose()
{
}

void filter::algos::CudaLatch::onLoad(void* interp)
{
}

void filter::algos::CudaLatch::onStart(void* pyThreadState)
{
}
